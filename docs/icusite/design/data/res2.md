# Resource Bundle Format v2

## Status

This is currently a proposal for .res formatVersion 2, intended for ICU 4.4. The
motivation for the new, incompatible resource bundle file format is a
significant [ICU data size reduction](../size/index.md).

The prototype in
<http://bugs.icu-project.org/trac/browser/icu/branches/markus/smallres> is
complete for Windows and passes all tests, but needs some more work for
non-Windows, big-endian and EBCDIC platforms, and code cleanup.

## Results

The ICU 4.2 .dat package data file is 16,012,800 bytes large. (Default Windows
build.)

In the prototype branch, the .dat package is 13,632,048 bytes large. This
represents a reduction by 2,380,752 bytes, or 14.9% of the .dat file size.

The .zip-compressed version of the .dat file is reduced from 5.63 MB (5,912,310
bytes) to 4.97 MB (5,217,935 bytes), that is, by about 11.7%.

Other changes are planned beyond the resource bundle file format changes here,
for further ICU data size reductions. See the [ICU data size
reduction](../size/index.md) pages for ideas.

## Changes and Non-Changes

genrb can write resource bundles with either formatVersion 1.3 (new minor
version) or 2.0 (new major version). The runtime code can use resource bundles
with all formatVersions 1.x and 2.x.

*   It is useful to be able to generate version-1 resource bundles for use with
    older versions of ICU, such as when we provide new time zone data.
*   Similarly, it is useful for the runtime to be able to use old resource
    bundle files so existing data need not be regenerated.

All changes are invisible to users of the public API. New resource types are
entirely internal, and are mapped to existing public resource types when queried
via the API. (This was already done with URES_TABLE32.)

*   Exception: On EBCDIC platforms, in formatVersion 2, the order of table items
    is according to the *ASCII* sort order of their key strings, rather than the
    EBCDIC sort order. This order of resources is visible when iterating through
    the table resources and when accessing table resources by index. As a
    benefit, the order is now consistent between platforms.

All resources still take advantage of memory mapping. There is no significant
increase in heap memory usage. (Only some structs get some additional pointers
and scalar values.)

### Compatible changes

(formatVersion 1.3)

*   Within any given bundle, genrb eliminates duplicate string values. Multiple
    string resources may use the same value and offset.
*   Within any given bundle, genrb eliminates duplicate key strings, and shorter
    key strings can share the suffix of a longer one. For example, given two key
    strings "abc" and "bc", only "abc" will be stored (with its terminating NUL)
    and "bc" will use the "abc" offset plus 1. ("Suffix sharing")

These changes are compatible for runtime code, but the string value duplicate
elimination requires a new version of the swapper code (compiled into the icupkg
tool) to avoid swapping the same resource multiple times.

### Incompatible changes

(formatVersion 2)

**Within a bundle**, there is a new, optional array of 16-bit units, for
resource values that can be stored entirely as sequences of 16-bit units. One
benefit is that none of the resources stored there need per-resource padding to
4-byte boundaries.

*   If the 16-bit-units array exists, the first 16-bit unit is zero for empty
    resources.
*   There is a new string resource type. Unicode strings are stored in the
    16-bit-units array, mostly without an explicit length. For short strings,
    u_strlen() is called. (Strings must be NUL-terminated anyway for
    compatibility.) For longer strings, the first 16-bit unit is in the range
    0xDC00..0xDFFF (which is the trail surrogate range). This unit either
    contains the length of the string directly (up to about 1000), or together
    with the following one or two 16-bit units, as indicated by the range of the
    first unit.
    *   Short Unicode string values also use suffix sharing like key strings.
        (This works for short suffix strings because of the omission of the
        explicit length.)
    *   "Short" is a variable threshold that can be changed in genrb without
        affecting compatibility.
*   There is a new table resource type. Tables that only contain strings which
    are stored within the first 64k 16-bit units are also stored in this
    16-bit-units array. They have a 16-bit number of entries, 16-bit key string
    offsets, and 16-bit resource values which are the string offsets.
*   There is a new array resource type. Arrays that only contain strings which
    are stored within the first 64k 16-bit units are also stored in this
    16-bit-units array. They have a 16-bit number of entries and 16-bit resource
    values which are the string offsets.
*   For all resource types except simple integers (that is, for all resource
    types that use an offset to the resource value), a resource offset of 0
    indicates an empty resource. For example, an empty string, an empty table,
    zero-length intvector, etc.

**Across bundles**, a set of resource bundles may optionally use a common
pool.res bundle which may contain some or all of the key strings of that set of
bundles. For each bundle using the pool, the key strings are omitted if they
exist in the pool bundle, and the key string offsets indicate whether they are
for a local key string or a pool key string. At runtime, the pool bundle must be
available when another bundle is loaded that depends on it.

The current prototype uses this feature for the 330 locale data bundles. (Not
for collation, break iterator, miscellaneous bundles, etc.)

As a consequence of the pool bundle being inaccessible during bundle swapping,
the sort order of table items is fixed for all platforms. In sorting and lookup,
key strings must be compared as ASCII strings even if they are not stored as
ASCII. As a side benefit, iterating through a table will return its resources in
the same order on all platforms, and swapping is simplified. (In formatVersion
1, the order is different on EBCDIC platforms and the swapper has to re-sort
tables.)

*   Note: It would be possible to always store key strings in ASCII, but then
    the key strings would have to be swapped and cached at runtime so that the
    API can return them in the local charset family (if that's not ASCII).

## Summary

For a complete summary of the data format changes see the updated data format
description at
<http://bugs.icu-project.org/trac/changeset?new=icu/branches/markus/smallres/source/common/uresdata.h@HEAD&old=icu/branches/markus/smallres/source/common/uresdata.h@26012>
