# C++ string class

For char \* strings we manually manage stack buffers and allocated buffers, use
strcpy(), memcpy(), strcat(), ... The udata.c code even uses a "tiny string"
poor man's string "class", all written in C.

Wouldn't it be nice if we could use STL string?

## Status

As of ICU 4.6 Milestone 1 (4.5.1), the old CharString class has been turned into
a minimalist but fully usable C++ char \* string class for ICU-internal use.
(The old CharString class only served for invariant-character conversion from
UnicodeString to const char \*.)

See [ticket 7496](http://bugs.icu-project.org/trac/ticket/7496) and its commits.

## Old design idea

It should be possible to define

class icu_string : public UMemory, stl::basic_string<char, char_traits<char>,
icu_allocator<char> > {};

Note:

*   Inheriting UMemory means that we can do "new icu_string" and not use global
    new and delete.
*   Providing our own icu_allocator means that when the icu_string allocates its
    internal buffer, it won't use global new and delete but rather our own
    uprv_malloc() and uprv_free().
*   We could actually provide our own traits class which might use int32_t for
    string indexes instead of streampos.

Question: What should happen in an out-of-memory situation? The icu_allocator
could

*   either throw a bad_alloc exception like the std::allocator, and all
    icu_string-using code would have to catch that exception,
*   or return NULL and let the icu_string implementation crash.

References

*   http://www.sgi.com/tech/stl/basic_string.html
*   http://www.cplusplus.com/reference/string/
*   http://www.cplusplus.com/reference/std/memory/allocator/
*   http://www.sgi.com/tech/stl/character_traits.html
