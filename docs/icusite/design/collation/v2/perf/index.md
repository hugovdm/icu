# Collation v2 Performance

## 2014 ICU 53

C/C++, ICU 52 (maint-52 with a copy of the collperf2 code) vs. ICU 53
(release-53-1), both at svn r35657.

Note: Earlier results from January 2014 had some bogus numbers due to
[bugs](http://bugs.icu-project.org/trac/ticket/10867) in the performance test
code. These bugs were fixed before getting the April 2014 results.

### Linux x86-64, Clang 3.1

See the attached file with "Linux_x64_clang31_20140427". The comments below are
about performance comparisons for sorting and binary search.

Collator::compareUTF8(StringPiece)

*   4.7x as fast (+370%) for binary search of English with Latin text
*   2.1x as fast (+110%) for sorting of English/Latin
*   30%..3800% faster for other function/locale/text combinations
*   The old code wrapped the UTF-8 strings into UCharIterator and compared
    those.

ucol_strcollUTF8(char \*)

*   2%..5% faster for English with Latin text
*   1%..1200% faster for other function/locale/text combinations
*   Note: ICU4C 50 (released 2012-nov-05) already added optimizations to this
    function (identical prefix comparison, Latin-1 fastpath).

Collator::compare(UnicodeString)

*   1% slower..7.5% faster for English with Latin text
*   6% slower..140% faster for other function/locale/text combinations (mostly
    somewhat slower)

ucol_getSortKey(UChar \*)

*   Computing sort keys has gotten slower. Sort key ==performance== was not a
    focus for the new code. (Sort key ==length== should be about the same as
    before, using about the same compression techniques.)
*   The old code has duplicate code for computing sort keys with default
    collation settings, and for "preflighting". The new code avoids duplication
    where possible.

#### UTF-8 vs. UTF-16

Binary search of Latin strings is about equally fast for either encoding form.
Sorting of Latin strings is about 40% faster when using UTF-16.

For strings in other languages, binary search is about 20%..40% faster in
UTF-16, and sorting is about 15%..60% faster in UTF-16.

The code has equivalent optimizations for all of the common code paths (Latin
fastpath, fetching characters, looking up collation values) for UTF-8 vs.
UTF-16. All non-character-fetching code is shared. Only when normalization is
necessary (only in some cases when normalization is turned on, which is not the
default) are small pieces of text converted to UTF-16.
