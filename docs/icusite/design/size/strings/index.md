# Resource Bundle Strings

[TOC]

## Issue

String values are a large portion of data in resource bundles. ICU data as a
whole is large, partly because we have a large number of bytes in resource
bundle strings.

## Current Storage Format (up to ICU 4.2)

Strings are stored in a way that maximizes runtime performance, but they are not
compact.

All resource items have a 32-bit word with a 4-bit data type field and a 28-bit
value field. Except for single 28-bit integers, which are stored directly in
this resource item word, the value field contains an offset for where the actual
resource is stored. The offset is shifted left by 2, and all resources are
aligned at least at 4-byte boundaries.

A string resource has an int32_t length, the UTF-16 string contents, a NUL
terminator (1 UChar = 2 bytes), and, like all resources, sometimes padding to
get to a multiple of 4 bytes total.

An empty or one-UChar string takes up a total of 12 bytes: 4 in the resource
item (type & offset), 4 in the length, 0 or 2 for the contents, 2 for the NUL,
and 0 or 2 for the padding.

2..3 UChars=16 bytes, 4..5 UChars=20 bytes, etc.

## Low-Hanging Fruit (Implemented in ICU 4.4)

We should **eliminate duplicate strings**. (Store only one copy.) Judging from a
prototype, this will save about 700kB.

## More Compact UTF-16 (Implemented in ICU 4.4)

We could store relatively short UTF-16 strings **without the length integer**,
and only with the NUL terminator. Longer strings would be prefixed with one, two
or three 16-bit units for the length (with the first unit in a special range,
e.g., a trail surrogate). We could store all such strings in a common block
early in the bundle so that we don't need per-string padding.

This would require a new string resource type but would work with memory-mapping
and not require any heap allocations. Retrieval of short strings would require
calling u_strlen(). We would want to define "short" such that we save a lot of
space while keeping the average time for u_strlen() short. Reasonable cut-off
lengths for "short" are probably somewhere between 16 and 64 code units. This
cut-off would be set by genrb, and the runtime code would not need to know it
nor depend on it. (That is, we can change the cut-off without changing the data
format, even as a command-line option.)

If we can avoid storing the length (4-byte integer) for most of the 140,000
strings and save an average of 1 byte per string in padding, then that would
save about 700kB.

In addition, strings that are not preceded by a length value could use
**suffix-sharing**. (See the [Keys page](../keys.md).)

We should re-introduce the encoding of **empty strings** as a resource item of 0
(UTF-16 string type with offset 0). This was part of the original design but got
omitted from the implementation. Given the opportunity of a new formatVersion,
we should bring this back. It makes a small difference, for example in how
res_index.txt files are stored (key-value pairs with all values being empty
strings), at trivial cost.

## Simple Compression (Not implemented as of ICU 4.4)

A couple of simple techniques can get very close to 1 byte per character for
most scripts and 2 bytes per character for CJK, with less overhead (fewer bytes
for length, no terminator, no padding) than the current format. It could be much
simpler than SCSU. (4-byte padding still needs to be applied, unless we
radically rethink the resource bundle data format.)

*   In each bundle, determine the 3 SCSU-style "windows" (128-character ranges)
    that cover the largest number of characters.
*   One new resource type could be used to store "mini-strings" directly in the
    resource item word, instead of an offset: 0..4 ASCII characters, 2..3
    same-window characters, or one arbitrary code point.
*   Another new resource type could hold a byte string with a
    variable-length-encoded string length followed by the compressed string
    bytes. The first length byte would have 2 bits for the type of compression.
    Type 0..2 would indicate single-byte mode with the initial state set to one
    of the three SCSU-style windows, with ASCII and window characters in single
    bytes. Type 3 would be a CJK mode, with direct encoding of the main Han and
    Hangul blocks and one byte per ASCII character.
*   All of the compressed non-mini-strings would be stored at the beginning of
    the bundle, right after the keys, to avoid per-string padding.

A patch with this implementation is attached to this page. It only calculates
the compressed length but does not actually write strings in compressed form.

In the resource bundle header, we would probably store the number of compressed
strings (1 int), the number of UChars that they will decompress to (incl. 1 or 2
for the length and 1 for the NUL; 1 int), and the three window start code points
(3 \* uint16_t, omitting the bottom 7 bits of each).

There will be some unused byte combinations. We might want to store a version
number for the compression scheme in a few header bits, outside the .res
formatVersion.

Results from an almost-ICU4C 4.2 build from a slightly modified svn r25919:

**.dat file size: 16,011,616 bytes**
**number of resource bundles: 660**
**total size without headers: 10665468 bytes**
**number of strings: 140202**
**strings size including resource items: 6589592 bytes**
**size reduction: 2942546 (minus about 660\*16=10560 bytes in header)**
**total padding after compressed strings: 570 bytes**
**number of strings that fit mini format: 25033**
**savings from those: 297624**
**14 strings are incompressible (longer in "compressed" form than in original UTF-16)**
**savings is about 18.4% of .dat file size**

## General-Purpose Compression (Not implemented as of ICU 4.4)

We should consider general-purpose compression for very long, hard-to-compress
strings, at least ones that are rarely accessed, like the CJK collation
tailoring rules. See the [General-Purpose Compression](../compression.md) page
for a general discussion.

We could probably use the same new string resource type as for the simpler
compression scheme(s), and distinguish in the first byte of the compressed
string. We would want to store both the UTF-16 length (for allocation of the
decompressed string) and the number of bytes (to pass to the decompressor)
before the compressed bytes.

## Runtime Issues for Compression

The ICU4C API returns a string as a pointer to its NUL-terminated UChars, as
stored in the current format. If we compress strings in any way, then we need to
decompress them at runtime and cache them until the resource bundle is released.
Decompression and caching need to be synchronized (mutexed).

*   If we decompress all strings when we load a resource bundle, then we waste
    time and heap space on many strings that will never be accessed.
*   If we decompress each string the first time it is accessed, then we need to
    synchronize in the getString() function.
*   As a compromise, it might be reasonable to decompress strings when their
    containing array or table is accessed. Except that for collation data it is
    very common to access the binary data and very rare to access the rules
    string (which can be large), and they are in the same table.
*   We may need to maintain a hash table with the resource item word as a key
    (or similar). Per-array/per-table decompression might simplify this a bit
    because arrays and tables usually contain a dense list of string resources,
    or no strings at all.

For incremental decompression we should try to do incremental memory allocation
so that only a small amount of heap memory is used when few strings are
accessed.

We could try to not synchronize and cache strings if they are accessed via APIs
that do not simply return the const UChar \* pointer, for example
ures_getUTF8String() and ures_getUnicodeString().

## Possible Additions and Alternatives

We should look at **alias resource items** which are currently stored just like
strings in UTF-16. Do they need to be in Unicode? Can they be in invariant
characters (char \*) like keys? Or in ASCII or UTF-8? (They are relatively few
and small though.)

If we store compressed strings with a NUL terminator instead of their length,
then we can use **suffix-sharing**. (See the [Keys page](../keys.md).)

*   We would have to store the 2 bits for the initial-window/CJK state somewhere
    else. For example, take another 2 bits from the resource item (reduces the
    offset from 28 to 26 bits and thus the maximum number of string bytes from
    256MB to 64MB), or take 4 string types instead of 1.
*   We would have to modify the encoding to avoid the terminator byte in trail
    bytes of all multi-byte character encodings.
*   At runtime, we would have to preflight the length; for short strings we
    could decode into a stack buffer and copy the decoded string into the cache.
    For long strings, we would probably use pure preflighting and read the
    compressed bytes twice.

We could store all "container" types (tables and arrays) in one contiguous
block, most easily at the end of the bundle. At runtime, we could copy this part
into heap memory, and when we decompressed a string and stored it in the cache,
we could modify its resource item word in the heap copy so that next time we see
a standard UTF-16 string resource type. We would probably have to be careful
with memory ordering issues on some CPUs. (That is, access to a resource item in
a heap table/array would probably have to be guarded by UMTX_CHECK().)
