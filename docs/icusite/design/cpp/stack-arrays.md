# Stack Arrays

A lot of ICU implementation code writes strings and other contents to buffers
that require a fair bit of boilerplate memory management. Usually, a
fixed-length stack buffer is used to cover the most common cases. An initial
guess for the needed capacity may exceed the stack buffer size, requiring
immediate heap allocation. An overflow may occur during processing, requiring
heap allocation or reallocation, with or without copying the current contents.
Some functions need a destination buffer with the same capacity as one provided
on input, such as string case mapping functions when source and destination are
the same buffer. When the function exits, a heap-allocated buffer must be freed,
and there might be several exit points from the function.

I want to encapsulate all of this into a templatized class which frees the heap
memory in the destructor.

This would be ICU-internal, only for arrays of Plain Old Datatypes, and using
uprv_malloc() and uprv_free().

## Status

Implemented in the trunk after ICU 4.4 Milestone 3 (4.3.3), see MaybeStackArray
in
[cmemory.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/cmemory.h).
There is also a MaybeStackHeaderAndArray class in ICU 4.6 Milestone 1 (4.5.1),
combining a header struct with a contiguously adjacent array.

### Not always the best solution

In some cases where MaybeStackArray would be a good fit as a replacement of the
existing C or C-style code, it would actually be much simpler to just use
UnicodeString, UVector or similar existing C++ classes. (ICU 4.6 Milestone 1
(4.5.1) adds a CharString class for char \* strings.) We should try to use those
if possible.

## class MaybeStackArray

```none
template<typename T, int32_t stackCapacity>  // object has an array T[stackCapacity]
class MaybeStackArray: public UMemory {
public:
    MaybeStackArray();  // starts with its stack array
    explicit MaybeStackArray(int32_t capacity);  // heap allocation if capacity>stackCapacity
    MaybeStackArray(T *bufferAlias, int32_t capacity);  // alias the buffer if the arguments are ok
    int32_t getCapacity() const;
    T *getAlias() const;
    T *getLimit() const;  // returns getAlias()+getCapacity(): often used in ICU code
    T &operator[](int32_t index);
    T *reallocate(int32_t capacity);  // throws away old contents; returns NULL if the allocation failed
    T *resize(int32_t capacity, int32_t length);  // reallocates and copies the first length items
    T *orphanOrClone(int32_t length);  // orphans its heap array or clones its stack array; resets itself to the internal stack buffer
    ~MaybeStackArray();
private:
    T stackBuffer[stackCapacity];
    // forbid assignment and copy construction
};
```
