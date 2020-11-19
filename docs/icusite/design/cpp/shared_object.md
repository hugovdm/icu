# Shared Objects

[TOC]

Many C++ ICU objects are a composition of several other ICU objects (think of
MessageFormat). Because of memory management, each ICU object typically owns its
own copies of its members. Creating an ICU object means creating fresh copies of
several smaller ICU objects that make up that object; deleting an ICU object
means deleting those smaller ICU objects. Some ICU objects, like DecimalFormat,
have a memory footprint of several kilobytes and can take on the order of 45
microseconds to create on a modern x64, multi core, linux workstation. Creating
a PluralRules object can take 31 microseconds on the same machine. Therefore
bigger objects that use DecimalFormat and PluralRules, like MessageFormat, take
even longer to create. Likewise, copying a large ICU object like MessageFormat
becomes very inefficient because its copy constructor must make fresh copies of
all its constituent objects, some of which are quite large.

Because callers often create stock instances of ICU objects by locale, it would
be best if ICU objects could share common parts rather than needing to create
their own copies of each constituent object.

## Goals

*   Speed up creation of large ICU objects by multiple orders of magnitude
*   Speed up copying of large ICU objects by at least an order of magnitude
*   Thread safety with const: No exceptions.
*   ICU compatible: No exceptions.
*   Must work in public header files without being explicitly defined there: No
    exceptions.
*   Conserve memory

## Status

Implemented in ICU 53.
[sharedobject.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/sharedobject.h)
and
[lrucache.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/lrucache.h)

## Design

### High Level

Introduce a new breed of "shared" pointers that collectively own shared objects
and manage those objects via reference counting. When reference count goes to
zero, object is freed. For key ICU classes, introduce an LRUCache of these
shared pointers keyed by locale Id.

*Issue: Smart pointers can be implemented in several ways*

*   Use a templated class similar to std::shared_ptr (can't be the actual STL
    class because we must be ICU compatible)
    *   Pro: Reference counts incremented and decremented automatically as
        pointer objects go in and out of scope.
    *   Pro: Can manage any type.
    *   Con: Reference count for each object stored separately in its own heap
        space. Each shared object requires 2 heap allocations.
    *   Show stopper: Requires full definition of shared pointer in ICU public
        header files. The only way to use this method is to make the the shared
        pointer definition public from the start.
*   Use a "shared object" base class that includes a reference counter
    *   Pro: Reference count stored within the shared object. No extra heap
        allocation needed for reference counter.
    *   Pro: Pointers to shared object subclasses work like any other forward
        declared, "dumb" pointer in public header files. No need for full
        declaration in public header files.
    *   Con: Reference counts must be incremented and decremented manually.
    *   Con: Only subclasses of the shared object base class can be shared.

*Chosen option: "shared object" approach*

### Details

class SharedObject : public UObject

*   void addRef() const -- increments the reference count by 1 using a memory
    barrier.
*   void removeRef() const -- decrements the reference count by 1 using a memory
    barrier. removeRef() deletes the object if reference count reaches 0.
*   int32_t getRefCount() const -- gets the reference count.
*   static void copyPtr(const T \*src, const T \*&dest) -- Makes dest point to
    src's object while adjusting reference counting as necessary. dest must be a
    reference counted pointer, see below; src need not be.
*   static void clearPtr(const T \*&ptr) - Makes ptr point to NULL while
    adjusting reference counting. ptr must be a reference counted pointer.
*   static T \*copyOnWrite(const T \*&ptr) - If ptr is the sole reference to its
    object, leaves ptr unchanged. Otherwise copies the object via copy
    constructor and makes ptr the sole reference to the copy. Returns a writable
    version of ptr.

Highlights of SharedObject:

*   Definition: Reference counted pointers are either NULL or they are included
    in the reference count of what they point to.
*   Reference counted pointers are always of the const T \* variety because they
    point to shared objects.
*   SharedObject extends UObject, so every SharedObject must define a virtual
    destructor in a separate .cpp file.
*   Newly created SharedObjects always have zero reference count.
*   On error initializing a created SharedObject, delete it like any other
    object
*   Once the SharedObject is fully initialized, assign it to a reference counted
    pointer via copyPtr() (or make the current pointer a reference counted
    pointer by calling addRef() on it)
*   After that access SharedObject only through const T \* pointers.
*   Right before a reference counted pointer goes out of scope, call removeRef()
    or clearPtr() on it.
*   SharedObjects should be designed to be initialized once and then never
    change after that. If a SharedObject must change after initialization,
    provide a copy constructor so that copyOnWrite will work or provide some
    other way to get a deep copy of the SharedObject.

class LRUCache : public UObject // designed to be subclassed

*   get(const char \*localeId, const T \*&ptr, UErrorCode &status) - Makes ptr
    point to the SharedObject in cache that corresponds to localeId. If no such
    object exists in the cache yet, get creates one by calling create()
*   protected virtual SharedObject \*create(const char \*localeId, UErrorCode
    &status)=0 - Creates the shared object for localeId. Note that create
    returns a non const pointer. This is a reminder that create is only
    responsible for creating and initializing the SharedObject. create must
    never change the reference count of the SharedObject it creates from 0.
*   protected LRUCache(int32_t maxSize, UErrorCode status)

class SimpleLRUCache : public LRUCache // concrete class

*   Constructor takes a pointer to a function in addition to the cache size.

Write a custom subclass of LRUCache if creating a SharedObject by locale
involves having some closure beyond simply the localeId.

Highlights of LRUCache

*   Every SharedObject in LRUCache has a reference count of at least one. The
    LRUCache itself counts as one reference.
*   When get() is successful at retrieving a SharedObject from the LRUCache, it
    ALWAYS increments the reference count of the retrieved SharedObject by 1.
    Therefore, every successful call go get() must be followed by a call to
    removeRef() or clearPtr() to avoid leaks. get() works this way because
    SharedObjects can be evicted from the LRUCache at any time. By incrementing
    the reference count, get() ensures that the fetched SharedObject will remain
    valid at least until the caller calls removeRef() or similar on it.
*   LRUCaches have a fixed number of SharedObjects they can hold. Once this size
    is reached, the least recently accessed SharedObjects are evicted to make
    room for new ones.
*   When the LRUCache evicts a SharedObject, it doesn't actually delete it until
    there are no other references to it.
*   Each class can create one LRUCache for itself using the umtx_initOnce
    mechanism
*   An LRUCache object is not threadsafe. Access to it must be guarded by a
    mutex.

Sharing objects like NumberlFormat that do not inherit from SharedObject

The solution is to build a new class that extends SharedObject and contains the
an instance of the object to shared. See
[sharednumberformat.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/i18n/sharednumberformat.h)
as an example.

Highlights:

*   SharedNumberFormat contains only a pointer to NumberFormat because
    NumberFormat is polymorphic.
*   SharedNumberFormat adopts its NumberFormat object to avoid the cost of
    copying large subclasses of NumberFormat
*   SharedNumberFormat destructor deletes its SharedNumberFormat object
*   SharedNumberFormat provides only const methods that provide only const views
    of the underlying NumberFormat object.
*   Accessing the NumberFormat object requires a double dereferencing.

## Best Practices

Rather than store the same information in multiple LRUCaches across ICU, we
expose LRUCaches containing common data as internal API. For example,
NumberFormat has the following internal API:

/\*\*

\* ICU use only.

\* Returns handle to the shared, cached NumberFormat instance for given

\* locale. On success, caller must call removeRef() on returned value

\* once it is done with the shared instance.

\* @internal

\*/

static const SharedNumberFormat\* U_EXPORT2 createSharedInstance(

const Locale& inLocale, UNumberFormatStyle style, UErrorCode& status);

While an ICU class can and should store information specific to it in its own
LRUCache, an ICU class should avoid storing references to SharedObjects from
other caches in its LRUCache. Doing so would create long term references making
it nearly impossible to reclaim memory when a SharedObject is evicted from
cache. Doing that is like storing the same information in multiple caches.
Instead an ICU object should grab the shared objects it needs from each LRU
cache in its constructor and then call removeRef() on them in its destructor.

Example of Shared objects in use:
[measfmt.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/i18n/unicode/measfmt.h)
[measfmt.cpp](http://bugs.icu-project.org/trac/browser/icu/trunk/source/i18n/measfmt.cpp)

References

*   <http://en.cppreference.com/w/cpp/memory/shared_ptr>
