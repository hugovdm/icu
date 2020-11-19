# Smart Pointers

[TOC]

In C and C++ functions with multiple exit points (multiple return statements),
we sometimes have allocated objects that need to be released, and the set of
objects grows as we get further down the function execution. We manage this by
either releasing a growing set of objects at each return statement, or by
collecting all cleanup at the end of the function and doing "goto cleanup" or
similar.

A cleaner way of doing this is via a "smart pointer". You create a
smart_ptr<UnicodeString> variable, initialize it with a new UnicodeString, and
when the smart_ptr goes out of scope, the UnicodeString is automatically
deleted. We would also have something like a smart_uprv_malloc() which would
call uprv_free().

We should also use (and offer publicly) variations that "close" our C API
objects. For example, smart_ucnv cnv(ucnv_open("Shift-JIS", &errorCode)); would
automatically ucnv_close() the converter when the cnv variable goes out of
scope.

The smart pointer class could be kept internal, but since it is useful, it
should be public, except for pieces that require ICU-internal definitions
(uprv_free()).

## Goals

*   Smart pointer owns the object and releases it when it goes out of scope.
*   Transfer of ownership via copy/assignment seems messy and unnecessary;
    better not to have it to reduce misuse. Simpler & more robust.
*   ICU-compatible: No exceptions.
*   Need to be able to orphan/release the pointer and its ownership.
    *   It would be very useful to use a smart_ptr while building an ICU service
        object, simply return when there is an error (and the smart pointer
        releases the partially built object) and return ptr.orphan(); at the end
        when the service object was built successfully. Equivalently, a
        constructor could hold sub-objects in smart pointers until everything is
        done, at which point it orphans them into raw member pointers.
*   Need variants for normal C++ object pointers, C++ arrays, and ICU C service
    objects.

## Status

Implemented in ICU 4.4. Public API
([unicode/localpointer.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/unicode/localpointer.h))
available in ICU 4.4 Milestone 3 (4.3.3), internal LocalMemory
([cmemory.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/cmemory.h))
available in Milestone 4.

## Prior Art

### std::auto_ptr

*   Part of the STL, well understood.
*   Has release() method.
*   Does not throw exceptions.
*   Drawback: Offers transfer of ownership via non-standard semantics of copy
    constructor and assignment operator.
*   Drawback: No variant even for C++ arrays.

### boost::scoped_ptr & boost::scoped_array

*   Never transfers ownership of the object.
*   Does not throw exceptions.
*   Drawback: It intentionally does not have a release() or orphan() method.

### Taligent Safe Pointers (TOnlyPointerTo)

*   Naming and semantics (alias/adopt/orphan) similar to ICU's, due to common
    heritage. (Otherwise it's a lot like std::auto_ptr.)
*   Has orphan() method.
*   Drawback: Offers transfer of ownership.
*   Drawback: Value-and-ownership change via assignment operator seems obscure.
*   Show-stopper: Throws exceptions.

## Design

### Base+Variants

One common base class with all of the common methods, plus variants for C++
pointers (delete), C++ arrays (delete\[\]) and C objects (ICU C service objects
and other types with "close" functions).

*Issue: This can be done in several ways:*

*   Variants are subclasses
    *   Non-virtual base class
        *   Every subclass must override multiple methods which delete the owned
            object (destructor, adoptInstead()).
        *   Pro: Simplest to implement. No vtable.
        *   Con: If the set of deleting methods ever increases, then subclasses
            need to be modified to override those new methods. However, I don't
            see what kind of deleting method we would add given the limited
            goals for these smart pointer classes, so this seems like a minor
            disadvantage.
        *   Con: It would be forbidden but possible to use the base class
            directly, unlike in the other approaches. Clear documentation should
            help.
    *   Virtual base class
        *   Virtual destructor, virtual void handleDelete() = 0 used by deleting
            methods.
        *   Pro: Any subclass need override only handleDelete().
        *   Con: vtable, doubles the size of the object, slightly larger inline
            code, and the linker needs to locate the vtable implementation.
*   Variants are template specializations, not subclasses
    *   Close functor
        *   Add a template parameter to the base class with a Close functor
            typename (a class with operator() which deletes its pointer
            argument).
        *   Pro: Base class definition handles deletions; no vtable.
        *   Con: Need to define a Close functor for every type for which we want
            a smart pointer.
    *   Close function type
        *   Add a template parameter to the base class with a Close function
            typename (a function typedef). (Does this work as a template
            parameter?)
        *   Pro: Base class definition handles deletions; no vtable.
        *   Con: Not sure "close" function signatures are all identical (return
            values, calling conventions).

*Chosen option: Non-virtual base class.*

### Details

template<class T> class LocalPointerBase (common base class for all variants;
never deletes/frees)
and
template<class T> class LocalPointer : public LocalPointerBase<T> (uses standard
delete)

*   explicit LocalPointer(T \*p = NULL) -- takes ownership
*   ~LocalPointer() -- deletes the object it owns
*   UBool isNull() const, UBool isValid() const -- NULL checks
*   operator==(const T \*), operator!=(const T \*) -- comparisons with simple
    pointers, but not with other LocalPointers
*   T \*getAlias() const -- access without ownership change
*   operator\*, operator-> -- access without ownership change
*   No operator T \*() -- the programmer must decide whether to use getAlias()
    (without transfer of ownership) or orpan() (with transfer of ownership and
    NULLing of the pointer).
*   T \*orphan() -- gives up ownership; the internal pointer becomes NULL
*   T \*adoptInstead(T \*p) -- deletes the object it owns and adopts (takes
    ownership of) the one passed in

Properties of these classes:

*   No ownership transfer: No copy constructor, no assignment operator
*   Does not throw exceptions
*   Declares private new and new\[\] operators to enforce usage as stack objects

The following are variants for arrays and C service objects. Do not use them via
the base class interface since none of the methods are virtual. (We could try to
use template specialization rather than subclassing, but that might not work
when adding further methods.)

template<class T> class LocalArray: public LocalPointerBase<T>

*   Destructor and adoptInstead() use delete\[\] not delete
*   operator\[ptrdiff_t\] -- array element accessor

template<class T> class LocalMemory: public LocalPointer<T> -- internal only,
for uprv_malloc()/uprv_free()

*   Destructor and adoptInstead() use uprv_free() not delete
*   operator\[index\] -- array element accessor
*   allocateInstead(int32_t n) -- delete the memory it owns and adopt (take
    ownership of) (T \*)uprv_malloc(n\*sizeof(T))

#define U_DEFINE_LOCAL_OPEN_POINTER(LocalPointerClassName, Type, closeFunction)
\\
class /\* U_COMMON_API \*/ LocalPointerClassName : public LocalPointerBase<Type>
{ \\
...
U_DEFINE_LOCAL_OPEN_POINTER(LocalStdioFilePointer, FILE, fclose);
U_DEFINE_LOCAL_OPEN_POINTER(LocalUBiDiPointer, UBiDi, ubidi_close);
...

*   For ICU C service objects
*   For any type (can be "incomplete") with a "close" function

*Issue:* Should we assume that all "close" functions check for NULL (at least
most of them do), or should we define the class to check for NULL before calling
the "close" function?

*   We could start with assuming that all "close" functions check for NULL, and
    change it later if necessary. This change would be compatible. Doing it the
    other way around would be an incompatible change.

ICU headers should predefine a LocalXyzPointer class for each Xyz ICU C service
object type. (Xyz=UBiDi, UCaseMap, UConverter, ...)

*Issue:* Should these classes all be defined in a new header file
(localpointer.h?), or should each of them be defined in the service-specific
header file (ubidi.h, ucasemap.h, ucnv.h)?

*   Pro each in specific header file: The smart pointer type is defined next to
    the basic type. \[With #ifdef XP_CPLUSPLUS...#endif\] Easily found.
*   Pro all in one header file: No dependency from the specific header files on
    the new, template-using smart pointer header file.
*   Con all in one header file: This would require #defining a macro together
    with each service object typedef, and require the guarded typedef and macro
    definition in both the specific header file and in the smart pointer header
    file. Also, it is easy to forget adding a new smart pointer type for a new
    service object type.

*Chosen option:* Define each ICU service object smart pointer class next to its
base typedef, in the service-specific header file.

References

*   <http://www.cplusplus.com/reference/std/memory/auto_ptr/>
*   <http://www.boost.org/doc/libs/1_39_0/libs/smart_ptr/scoped_ptr.htm>
    *   <http://www.boost.org/doc/libs>
    *   <http://www.boost.org/users/license.html>
    *   Would we just use select parts of the Boost library? It appears to have
        a compatible license: <http://www.boost.org/LICENSE_1_0.txt>
*   [Taligent's Safe
    Pointers](http://www.cubik.org/mirrors/taligent/Docs/books/FS/FS_57.html#HEADING121)
