# General-Purpose Compression

We have been thinking about using general-purpose compression for some time.

Pro:

*   Better compression ratio than custom compact encodings in many cases.
*   Handles arbitrary data, not just strings or integers.
*   Low maintenance when using well-tested open source libraries.

Con:

*   Too much overhead (time and memory) for very small amounts of data, such as
    short strings.
*   ICU4C data loading memory-maps data (strings, binary collation blobs, etc.).
    Decompression would require heap-allocating the uncompressed result,
    normally per process. Up-front decompression would waste time and space for
    unused data. Incremental decompression requires synchronization. See also
    ["Runtime Issues for Compression" on the "Resource Bundle Strings"
    page](http://site.icu-project.org/design/size/strings#TOC-Runtime-Issues-for-Compression).

Requirements:

*   Must be compatible with the ICU license.
*   Must be supported in Java, ideally via built-in
    [java.util.zip](http://java.sun.com/javase/6/docs/api/java/util/zip/package-summary.html):
    zlib, gzip or zip.

Recommendation:

It looks like [zlib](http://zlib.net/) might be the best choice: It is open
source with a notice-style license like ICU's, is free of patents, documented in
RFCs (1950 & 1951) supported in java.util.zip, used in .jar and .png files, used
in Apache, ...

## Granularity

**Compress whole .dat package.** Pro: Might be best compression because it can
optimize across pieces. Con: Need to decompress all of the data before accessing
any of it.

**Compress per data item/file, e.g. per resource bundle, except for the
header.** Pro: Could be used with any data item/file without modification to its
internal format. The general data writing and loading code could handle
compression and decompression. Con: If it is common for only parts of a data
item/file to be accessed, such as in resource bundles, then the decompression
may read and decompress much more data than necessary, and the
per-data-item/file load time increases.

**Compress per piece of data, e.g. per string in a resource bundle.** Pro: Does
not decompress what's not used, faster bundle loading. Con: Probably inefficient
for short strings (which are most common). Need per-string synchronization for
multithreading.
