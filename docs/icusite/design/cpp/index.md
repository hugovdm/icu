# C++

[TOC]

We use a lot of old-fashioned C and C++, which makes it cumbersome and
error-prone to implement ICU4C. It seems like we should be able, in 2009, to
rely on C++ in a way that we couldn't in 1999. For example, it should be
possible to assume that compilers support namespaces, (carefully) use templates
in the implementation, and (carefully) use STL strings and containers.

Using better, simpler C++ would require turning C files into C++ files where we
want to use such techniques. We would not need to change everything in a "big
bang", we could just start using one or the other new technique where we touch a
file and this stuff helps.

## Details

### Exceptions

We still can't use exceptions as long as we mix C and C++ code because
exceptions don't travel through C code. Although, in principle, it should be
possible to use exceptions if we catch all of them (and turn them into
UErrorCode) before they leak into C code or out from public API.

Possible path:

> Convert the remaining plain C library code to C++.

> try/catch for C API functions, with catch setting an error code or state, as
> suggested above. This would be more reliable in returning errors than our
> existing code, and would allow the removal of a lot of tedious error checking
> in our implementation code.

> Memory Allocation - Get rid of the ability to hook the allocator? Or, if not,
> have our wrapper throw on failure instead of returning NULL. Use of exceptions
> on allocation could also improve the reliability of handling failures in
> constructors.

### Namespaces

Update: Done. Namespaces are required, as described.

Can we just assume now that all relevant compilers support namespaces, and that
STL classes are in std:: ?

We would still have to allow to disable renaming, that is, sometimes the ICU
namespace is icu_4_2 (with an alias of icu) and sometimes it's just icu. But
there should be no other choice, and we should always be able to use icu:: when
referring to an ICU C++ type. (Only when *declaring* one we can't use the
alias.)

### Containers

We have our own, home-grown vector and hash table classes. Wouldn't it be nice
if we could just use variants of the STL containers? For what this might look
like, take a look at the [subpage about the C++ string class](string.md).

Problems with our home grown classes:

> Very Limited functionality.

> Often awkward to use.

> Any effort expended in extending them would be better spent on actual ICU
> functionality, and is an unnecessary vector for introducing bugs.

Obstacles to STL in ICU:

> Memory Allocation. The standard C++ allocators throw exceptions when they
> fail, and are not compatible with ICU's memory hooking.

> Availability of libraries on all platforms. Android was questionable, but is
> probably no longer a problem.

> Collections appearing in APIs, need to understand mixed runtime, ownership
> implications. Purely internal use would be easier.

> Exceptions.

> #### Issues

> libstdc++ is fairly large, and instantiating containers for multiple types
> will increase the compiled size of ICU. For a discussion and alternative, see
> [uSTL](http://ustl.sourceforge.net/). We could consider just using STL anyway,
> or using it with a limitation of the number of types we use in its containers,
> or using uSTL or something like it, or writing our own icu_string and
> continuing to use our existing vector and hash table classes.

### Cruft & Complication

ICU supports a lot configurations and options, leading to complex, hard to
understand code and, most likely, undiscovered bugs. Anything that can be
removed or reduced would be helpful.

*   Builds for ancient and obscure platforms.
*   Service Registration
*   Memory Allocation Hooking
*   Windows XP.

### C++11

> #### char16_t, char32_t

> Using char16_t and char32_t for ICU's UChar and UChar32 types would seem to be
> a natural. u"literals" would be wonderful.
> There were issues when we tried it, details? Overload resolution of numeric
> values?

> #### Range based for loops

> These look a lot like modern Java for loops, e.g.

> ` for (const UnicodeString &str: someSet) {`

> ` ...`

> UnicodeSet, UnicodeString, StringEnumeration are naturals. There are probably
> may other places.

> #### std::string

> Add function overloads taking std::string in places we can currently accept a
> UTF-8 (char \*) string. Maybe StringPiece could help.

> Requires that we can use the C++ standard library within ICU, and resolve any
> issues with passing objects across the library boundary.

> #### nullptr

> New C++11 type to replace NULL. Safer, in that it is a distinct type that is
> compatible only with pointers. I don't think there is much to do in the short
> term, other than perhaps to show its use in examples and sample code.
> Redefining NULL is dicey; it may be a macro. clang gives compiler warnings on
> bogus coercion of NULL to an integer type, so errors in ICU implementation
> code will be found.

#### display of ICU objects

> ` cout << some_icu_object`

> ought to do something similar to Java

> ` System.out.print(some_icu_object)`

> Some combination of toString(), possible conversion, possible overloads of
> operator <<, ???. However it's done, it should just work.
> (Not C++11)

### More Details

See also the PMC-internal email thread "Some lessons learned from testing ICU
error recovery" from February 2008.

For more details see the subpages.
