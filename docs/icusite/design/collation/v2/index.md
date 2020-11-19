# Collation v2

[TOC]

## Status

The "collation v2" reimplementation went into [ICU 53](../../../download/53.md)
(both C++ & Java) (released 2014-apr-02), based on many of the elements of the
design doc below. (See also the [ICU 53 collation v2 work log](icu53log.md).)

After ICU 53, the collation code is again maintained as usual, with further
incremental enhancements and bug fixes.

## Introduction

### 2010-2014

(First ideas noted down here 2010-oct-23.)

What if we could implement ICU collation anew?

For reference

*   [Collation v1 design
    doc](http://source.icu-project.org/repos/icu/icuhtml/trunk/design/collation/ICU_collation_design.htm)
*   [LDML Spec:
    Collation](http://unicode.org/repos/cldr/trunk/specs/ldml/tr35-collation.html)
*   [UTS #10: UCA](http://www.unicode.org/draft/reports/tr10/tr10.html)
    *   Submit bug reports at <http://www.unicode.org/reporting.html>
*   [ISO/IEC
    14651:2007](http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=44872)
    (outdated, [available
    free](http://standards.iso.org/ittf/PubliclyAvailableStandards/index.html))
*   [ISO/IEC
    14651:2011](http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=57976)
    (CHF 146)
    *   The [Unicode collation FAQ "Q. What are the differences between the UCA
        and ISO 14651?"](http://www.unicode.org/faq/collation.html#13) includes
        among features of UCA but not ISO: "irrelevant combining characters
        don't interfere with contractions" (that is, ISO does not mention
        discontiguous contractions)
    *   The ISO Standard Abstract says: "ISO 14651 ... does *not* mandate... A
        context-dependent ordering." (unclear: ISO does support contractions =
        multi-character "collating elements")

Done: Fix documentation. For example, in the [User Guide,
Customization](https://sites.google.com/site/icuprojectuserguide/collation/customization),
there is a "For example, with the rule" but no rule, and a "Given the rules"
with no rules. Review & fix.

*   Sample rules there restored 2012-feb-08 from the [wayback machine's
    2004-aug-07 version of the User Guide, Customization
    chapter](http://web.archive.org/web/20040807074759/http://oss.software.ibm.com/icu/userguide/Collate_Customization.html).

[Ticket #9101](http://bugs.icu-project.org/trac/ticket/9101): collation v2
prototype (branch in [Trac
browser](http://bugs.icu-project.org/trac/browser/icu/branches/markus/collv2),
[web svn](http://source.icu-project.org/repos/icu/icu/branches/markus/collv2/);
[Trac revision
log](http://bugs.icu-project.org/trac/log/icu/branches/markus/collv2))

*   Files:
    *   source/i18n/collation\*
    *   source/i18n/rulebasedcollator\*
    *   source/test/intltest/ucaconf.cpp
    *   source/test/intltest/collationtest.cpp

Open collation tickets:
[ICU](http://bugs.icu-project.org/trac/query?status=accepted&status=design&status=new&component=collation&group=type&max=300&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&col=weeks&order=priority)
([opened by
markus](http://bugs.icu-project.org/trac/query?component=collation&reporter=%5Emarkus&group=resolution&col=id&col=summary&col=owner&col=type&col=status&col=priority&col=milestone&desc=1&order=id)),
[CLDR data-collation &
uca](http://unicode.org/cldr/trac/query?status=accepted&status=assigned&status=new&status=reopened&component=data-collation&component=uca&or&status=accepted&status=assigned&status=new&status=reopened&keywords=~collation&group=priority&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&order=priority),
[CLDR by
summary](http://unicode.org/cldr/trac/query?status=accepted&status=assigned&status=new&status=reopened&group=type&max=300&summary=~collat&col=id&col=summary&col=status&col=owner&col=priority&col=milestone&col=component&col=weeks&order=priority),
[CLDR by
description](http://unicode.org/cldr/trac/query?status=accepted&status=assigned&status=new&status=reopened&group=type&description=~collat&max=300&order=priority&col=id&col=summary&col=status&col=owner&col=priority&col=milestone&col=component&col=weeks)

## Why?

The current code ("v1") is large and complex, and feels fragile. We recently
(ICU 4.6) found several lurking bugs, and we have a large number of bug reports,
suspected issues and feature requests.

*   Easier maintenance.
*   Simpler.
*   Fewer hacks (e.g., hiraganaQuaternary).
*   Hopefully fewer bugs.
*   Maybe faster.
*   Maybe implement UTF-8 string comparison with at least some real
    UTF-8-processing code.
*   Easier to port to other programming languages (e.g., JavaScript or Python)

v1 architectural issues: Some ICU collation bugs (see the open collation tickets
above) are probably small and fixable. Some would require more work. For
example:

### Contiguous-contraction matching

The v1 contraction matching code and data structure assume that for every
contraction string every prefix of that string is also a contraction. The
builder automatically adds all such prefix contractions.

This is incorrect and can lead to unexpected results. While we should have a
prefix contraction for a contraction whose last-character tccc>1 (so that
discontiguous-contraction matching works as expected \[0FB2 0334 0F71 0F72
matching the contraction without 0334\] and yields canonically equivalent
results \[0FB2 0F71 0F72 vs. 0FB2 0F73\]), we should not auto-add them (because
their correct behavior is not necessarily obvious), and we don't need prefix
contractions when the above condition is false.

### Discontiguous-contraction matching

The matching code mixes code unit and code point handling. For discontiguous
contractions, we really need to work with code points and check their lccc &
tccc values (not ccc).

Skipped combining marks are written into the normalization buffer which is
discarded when we move beyond it. Instead, we need to keep track of how far we
move, and be able to return back into it.

Skipped combining marks can start nested and interleaved contractions,
recursively skipping some combining marks again. We need a second buffer to
handle these.

Note: The algorithms for contiguous-contraction matching (no skipping of
characters, could work with code units, continue with characters on the way to a
full match) and discontiguous-contraction matching (skip non-starter code points
that do not extend one full match to another, stop if there is no more
non-starter resulting in a new full match) are really different.

### Canonical equivalence

When normalization is on, we still fail to achieve canonical equivalence in
unusual cases. Tibetan composite vowel signs sometimes need to be decomposed
before reaching the trie lookup, even when the input passes the FCD check.

### Character iteration & trie lookup

All of the character iteration and FCD checking is done in one place, handling
all forms of input together with the trie lookup. This is hard to extend
efficiently to other forms of input (e.g., UTF-8 or UText without wrapping them
into a UCharIterator).

The newer UTrie2 was designed to support a dedicated UTF-8 fastpath. It should
be slightly faster with UTF-8 than using v1's UTrie and fully assembling every
code point before the trie lookup.

### Fast UTF-8 collation

Clean code layering, and also the use of UTrie2 (see previous paragraph), make
it reasonable to provide a dedicated UTF-8 string comparison implementation,
rather than v1's wrapping the UTF-8 input strings into character iterators.

For ICU 50, IBM is looking into duplicating a large chunk of UTF-16-handling
code for an optimized ucol_strcollUTF8(). With v2, much less code would have to
be duplicated.

### Hack to support Japanese

ICU supports level 4 distinctions between Hiragana & Katana for Japanese
collation (JIS X 4061) via a combination of the tailoring making such pairs of
characters equal and a hack in the code. This hack is turned on with the
hiraganaQuaternary attribute (which is [public although it's an implementation
detail of the Japanese
tailoring](http://bugs.icu-project.org/trac/ticket/9406)). The CE-fetching code
tests for a range of Hiragana characters (which is hardcoded and outdated) and
also tests if the combining voice marks are preceded by Hiragana (which does not
work when iterating backwards). The Hiragana-ness flag is not returned as part
of the CE but queried via a separate function (which is [not available via the
CollationElementIterator](http://bugs.icu-project.org/trac/ticket/2485)).

It would be cleaner and simpler to use one or more CE bits for quaternary-level
weights and make the distinction available to tailorings.

### Missing data

Sort key compression is hardcoded to be used for characters that sort between
the Latin and Han scripts. This is sub-optimal. We would need at least a minor
ucadata file format change to include the COMPRESS flags that were recently
added to FractionalUCA.txt.

When we have reliable data for compressible primary lead bytes, we can also [use
more byte values for the second byte of incompressible
primaries](http://bugs.icu-project.org/trac/ticket/7757#comment:7).

AlphabeticIndex has hardcoded data for the first character of each script. We
should have data for the first primary weight of each script to replace that.
(See [ICU ticket #9424](http://bugs.icu-project.org/trac/ticket/9424) comment 4
& [CLDR ticket #5129](http://unicode.org/cldr/trac/ticket/5129).)

TODO: Test if AlphabeticIndex works with script reordering!

Han characters (\[:Unified_Ideograph:\]) by default map to collation elements
with computed primary weights, with characters in the CJK Extension A block
ordered after the other BMP Han characters. The set and order of those
characters is hardcoded, and the code must be updated when new Han characters
are assigned in Unicode. This must be synchronized between the FractionalUCA.txt
generator code and ICU's runtime code. It would be much better to make this
fully data-driven. (See OFFSET_TAG in the prototype, and [CLDR ticket
#5142](http://unicode.org/cldr/trac/ticket/5142).)

### Easy & reliable Unicode version updates

Hacks that involve hardcoded data (set of Chinese and Hiragana characters,
compressible primary lead bytes, etc.) are bad for Unicode version updates.
Either the hardcoded data is not updated, or not updated correctly in all
variations of the code (e.g., C++ & Java), which can lead to poor results
(incomplete Hiragana set, wrong primary weights for numeric collation, missing
first-character-of-new-script in AlphabeticIndex) or performance (e.g.,
compressing the wrong lead bytes).

When the hardcoded set of Chinese characters is out of sync between the
FractionalUCA.txt generator and ICU4C/ICU4J, then the default order of some of
Chinese is just plain wrong; this requires updating circular-dependent Java code
during version updates.

Adding proper data and data structures greatly simplifies version updates and
avoids all of these problems.

### Smaller data

There is some low-hanging fruit for reducing the size of the CLDR root collation
data. For example, mappings for several thousand characters can be compressed by
encoding them as ranges that sort in code point order, and the "inverse UCA"
data necessary for building tailorings could easily be reduced by over 50%. This
might make it reasonable to include this data file in low-memory environments
and enable runtime building of tailorings.

Some changes to the root collation data would benefit both v1 & v2, in
particular changing artificial secondary collation elements to secondary weights
on the preceding primary collation elements.

It should be feasible to provide a shared file with just the "unihan" mappings,
and use it in CJK tailorings that include those mappings. This might make it
reasonable to include those tailorings in low-memory environments.

Longer-term, and with more effort, shared mappings for "search" collators and
European Ordering Rules ("eor") or similar might be feasible as well. Shared
"search" data should help reduce the data size for most languages, and would be
useful for all users of string search (e.g., browser in-page ctrl-F search).

### Porting to other programming languages

The v1 collation code is C-like (by design at the time), large and convoluted.
It would be nice to have smaller, better-structured code for porting to other
programming languages.

## How?

### Good software engineering

Encapsulation. Layering.

Object-oriented, real C++, no more naked arrays with duplicated overflow
handling, etc. More vtable and maybe function pointer dispatching, maybe fewer
ifs and switches. Try to work with fewer side buffers to switch input text
iteration into and out of. Easier to test: More isolated functional units with
their own tests.

A CollationIterator class for iterating through the input text, looking up
32-bit CE values and either turning them into full CEs or calling out to handle
specials. The base class should be optimized for assumed-to-be-FCD UTF-16 text.
Subclasses would handle non-trivial input text (iterative FCD
checking/normalization, UTF-8, UCharIterator, etc.) and only override text
access and maybe 32-bit-CE lookups as necessary.

A const CollationData class for all other data lookup and handling specials,
using an iterator for all text access and to hold variables.

### 64-bit CEs

Once lookup values are decoded to 64-bit CEs: Fixed-bit-field format. No
continuations. No special values or tags.

v1 uses 32-bit CEs. Each primary/secondary/tertiary weight can be 32 bits long,
but if it does not fit the standard 16:8:8 format we use continuation CEs which
complicate the code. Instead, we should use 64-bit CEs (32:16:16) in most of the
code. We should not need more than 16 bits for secondary weights and 12 bits for
tertiary weights, thus we should not need any continuations. Lookup values for
code points and strings (contractions)/prefix matches would still be 32-bit
values but they would more clearly be compact forms of full CEs, still with high
values encoding special forms. Expansions would be sequences of 64-bit CEs; we
should not copy them into the collation iterator but just set a pointer to them.

Note: In principle, CODAN needs continuations for long digit sequences. Without
continuations, we have to generate independent CEs, each with the '0' weight
byte in the top, so that script reordering works. This would break
French-reverse-secondary handling because we don't get enough information to
know the start and end of the CE sequence to keep them in order, but all of the
CODAN-generated secondaries are "common" so reversing them is harmless.

**Secondary weights:** We should use a smaller sort key compression range so
that there is more space for UCA and tailoring weights. v1's use of 05..85 for
compression seems excessive. We could use 05..45 which would match the tertiary
compression range. If we needed even more tailoring space for secondary weights,
we could use sequences of real CEs where non-initial ones would be
primary-ignorable. They would not need any special processing but have the small
cost of not being tertiary-ignorable, slightly lengthening sort keys, but the
common tertiary weight would be compressible. (Or maybe we could just construct
continuation CEs that are in fact ignorable except for their secondary weights?)

**Case-level weights:** Of the lower 16 bits, 2 are used for lower/mixed/upper
case flags as in v1. We would not need one of the 4 values for continuation. We
could in principle stretch each of the three case types from a quarter of the
16-bit range to a third, but we should continue to use the extra quarter as the
common-weight compression range.

The case bits do not store complete weights. In particular, we cannot
distinguish between 0=ignorable and 0=uncased=lowercase. We need to ensure that
the case level weight is 0 if the tertiary weight is 00 or 02. After fetching
CEs, we construct 0=ignorable for tertiary=0, special low weights for
end-of-string and for the minimal tertiary=02 for U+FFFE, and higher weights
when tertiary>02, for uncased=lowercase/mixed/uppercase.

For more details on case level weights see the [LDML
spec](http://unicode.org/repos/cldr/trunk/docs/web/tr35.html#Collation_Elements),
section 5.14.13 Case Parameters (new in LDML 22, fall of 2012) and
<http://bugs.icu-project.org/trac/ticket/9355>

**Tertiary weights:** Of the lower 16 bits, we use one or two 6-bit fields for
the tertiary weight. This makes for an easy transition and fits existing API
(CollationElementIterator) because 1 64-bit v2 CE encodes exactly 1 or 2 32-bit
v1 CEs.

TODO: Consider changing the case weight computation (spread contraction case
bits across an expansion), consider removing "mixed" case, and then expanding
the tertiary-weight lead byte to 7 bits. Or use 84 tertiary lead bytes 3..56,
57..AA, AB..FE.

*   This would be disruptive to the CollationElementIterator. Deprecate the API
    that returns a 32-bit integer (where the caller needs to know how to use the
    bit fields), and replace it with one that returns partially-processed
    weights. (See <http://bugs.icu-project.org/trac/ticket/2485>)

**Quaternary weights:** In v1, quaternary weights are purely computed, with low
weights for shifted primaries (when alternate-handling=shifted), and a high
weight for everything else. An intermediate weight is inserted for Hiragana
characters and their voice marks when the hiraganaQuaternary attribute is on.
This hiraganaQuaternary behavior is a hack that adds some complications; in
particular, the Hiragana-ness must be looked up by code point, inherited from
the preceding character (which is especially tricky in backward iteration), and
returned outside the collation element value.

Instead, v2 will use CE bits 7..6 for a "true" quaternary weight. It will be 0
everywhere except for ==Kata==kana characters in the Japanese tailoring.
(Easiest would be to introduce <<<< tailoring syntax.)

Bits 7..6 are "available" because in v1 they correspond to the two-bit
"continuation" marker which is not needed in v2. Using these bits for the
quaternary weight limits the tertiary weight to 12 bits, matching v1 for the
transition and for the CollationElementIterator.

Like the case level, quaternary bits do not store complete weights. We cannot
distinguish 0=ignorable from 0=common low weight. We need to ensure that the
quaternary level weight is 0 if the tertiary weight is 00 or 02. After fetching
CEs, we construct 0=ignorable for tertiary=0, special low weights for
end-of-string and for the minimal tertiary=02 for U+FFFE, and higher weights
when tertiary>02.

For tailorings, we assume/require that the base table only contains 0=common
quaternary weights.

Note: For tailoring after the common weight (<<<<), the quaternary sort key
compression needs to work much like secondary & tertiary compression, with a
split compression range for multiple common weights followed by either a low
weight (a shifted primary) or a high weight (a tailored quaternary). In v1,
since the common weight was the highest value, the compression range did not
need to be split. This is a minor disimprovement of the compression for a more
natural tailoring behavior. (We want to tailor &Hira<<<<Kata rather than
&\[before 4\]Kata<<<<Hira.)

TODO: Is a "true", tailorable quaternary level useful for languages other than
Japanese?

*   Maybe Arabic? See <http://bugs.icu-project.org/trac/ticket/8050> &
    <http://unicode.org/cldr/trac/ticket/4207#comment:4>

#### Rejected ideas

1. Should we use one or two bits for whether ccc=0 and/or whether there is a
decomposition, lccc!=0, tccc!=0?

*   probably not useful and/or too hard & wasteful to snag another couple of
    bits
*   lccc is only tested in discontiguous contractions, and at that point we do
    not look up any CE value

Note: There are characters where the decomposition changes the combining class
(especially ccc!=lccc, especially where ccc=0 as with U+0F73), which is
problematic for when we do not have normalization (FCD check) on. We should be
able to handle those in discontiguous-contraction matching by checking their
lccc values.

2. With 64-bit CEs, the second tertiary byte could use the full range 03..FF.
Effectively, 14 bits would be fully tailorable for tertiary weights.

*   Compatibility with existing tailoring code (at least during a
    dual-implementation transition phase) and existing API
    (CollationElementIterator) is easier if we keep the second tertiary byte to
    6 bits so that two bits remain for continuation bits in an old-style, 32-bit
    continuation CE.

### Code points not code units

v1 runtime code works with UTF-16 code units almost everywhere. Tracking of the
current position is complicated, and contractions matching is difficult because
the code iterates over code units for matching but has to check for code point
combining classes for correct handling of discontiguous contractions. We should
only optimize for BMP units in fastpath code, for example reading forward
through the inner UTF-16 buffer, and finding FCD boundaries (when normalization
is on). Otherwise we should iterate over code points to keep the code simple.
Note: Unlike normalization and case mapping where property-less surrogates can
be skipped/copied, collation always delivers a CE for a code point, either the
lookup value or the implicit value for the code *point*. Working with code
points would also help if we wanted to provide UTF-8-native collation.

### UTrie2

### Share word-trie code

We could consider using common [trie code](../../struct/tries/index.md) for
contractions and prefixes that is shared with other parts of ICU.

### Complications with context

#### Contractions

Using common trie code for contractions means that we cannot have a
collation-specialized trie data structure. v1 stores a **maxCC** (maximum
combining class of all characters at this level) and an **allSame** flag
(minCC==maxCC) at each contraction level. It seems like the main purpose is to
avoid launching into discontiguous-contraction matching when that cannot yield
any valid match.

No discontiguous contraction is allowed if all contraction characters have
cc==0; or if the intervening, non-matching character has a higher cc than any of
the contraction characters (there can't be lower-cc chars following in FCD
input); or if the intervening cc==maxCC && allSame (all contraction chars would
be normalization-blocked by the intervening one).

*   We should be able to achieve most of this optimization by encoding some of
    the data for the first contraction level in the contraction-special-CE32.
    **Ideas:**
    1.  One bit for "all contraction characters have cc==0".
    2.  9 bits for all of maxCC+allSame. (Leaves fairly few bits for the
        contraction-table offset.)
    3.  5 bits for a table-lookup of maxCC values (rounded up). We should find
        out whether the allSame flag is usually set to TRUE or FALSE. If it's
        usually FALSE, then we could omit it and only sometimes under-optimize.
*   Alternative: Much simpler, just prepend the contraction trie with one 16-bit
    unit that contains maxCC and allSame. Then we could also use another bit
    there for whether the character has a 32-bit CE by itself, and if so, store
    that in the following two 16-bit units. (Or we could always store the 32-bit
    CE, even if it's copied from the UCA, rather than falling back to the UCA.
    \[v1 seems to do this.\] If it's an implicit-special we could then compute
    its CE without fallback too.) Storing the default CE32 before the
    contraction trie is likely more efficient than storing it inside.
*   Builder calculating maxCC and allSame: In each suffix string, if it begins
    with cc!=0, iterate to the end or to the first cc==0. Collect all non-zero
    cc values, get max & allSame. No cc!=0 at all: maxCC=0, allSame=true. Any
    cc!=0 but also cc==0: allSame=false.

#### No contraction closure

v1 contraction data always stores results for every prefix of a contraction
string, even if that prefix only yields the concatenation of CEs from its code
points. This seems unnecessary. It seems like it should be easy to only store
real contraction results and to just remember the skipped combining marks, and
how many code points were read after the last match (for going that far
backwards).

The closure is probably also a bug: If we have contractions abcd & bc and a
string abce, storing a closure result for ab would prevent us from seeing
a+bc+e.

It turns out that discontiguous contractions only work as intuitively expected
if for every contraction abc with n>2 code points where the last code point c
has ccc(c)!=0 the prefix of length (n-1) is also a contraction. Missing prefix
contractions are not a problem with the functioning of the algorithm but with a
mismatch between human expectations and actual behavior. See UTC doc [L2/12-131R
UCA: Simplify contraction
processing](https://docs.google.com/document/pub?id=1_-0fMgYkbKCm-N-lrnMWhIN1abnpmPYPmqsHLPBVA6w)
(which adds a requirement for such prefix contractions) and [L2/12-108 UCA:
Problems with discontiguous
contractions](https://docs.google.com/document/pub?id=1nAC7dQMU5Sej3ZYkn_d7cIFmAYYk0hWxSuqytmHTow8)
(which explains the problem a bit more).

For discontiguous-contraction matching to work as expected, it is only really
necessary to have a prefix contraction when the last character has ==*ccc>1 not
ccc>0*==.

*   Decision: Not important to exclude trailing ccc=1 from the requirement to
    add prefix contractions.

#### Discontiguous contractions

Once we have a discontiguous contraction and are skipping intervening/skipped
combining marks, it gets messy. Consider input text ...fgAmnBpCxy... where ABC
are a contraction and mn & p are skipped combining marks. If those skipped
combining marks themselves are prefix and/or contraction specials, then we need
to start a contraction match from inside the skipped-marks buffer, possibly
extending into the following normal text, for example for a contraction of m+y,
and sometimes backing up again back into the skipped-marks buffer when an
attempted contraction did not yield a match.

The UCA Main Algorithm handles this by rewriting the input string, removing the
combining marks (e.g., B and C in the example above) that match.

v1 saves the skipped combining marks in a buffer (mnp) and then switches the
iterator to it. When moving beyond the buffer, the fact that the text spanned by
the contraction was effectively reordered, is lost. What happens when m has a
prefix of "gA"? What if "np", "nBp", "px" or "pCx" are contractions? v1 probably
yields somewhat unexpected results because it has to move beyond the
skipped-marks buffer and fails to backtrack back into it.

v2 uses two buffers. If the first one is non-empty, then it contains skipped
combining marks from an outer discontiguous contraction. We track how far we
read beyond that into the regular text. We collect newly skipped combining marks
in the second buffer. When the outer contraction is done, we replace its
contents, up to the last matched character, with the contents of the second
buffer and empty that. Loop until the first buffer is empty. (This replaces
recursion.)

Insight: The complexity is somewhat limited by the UCA Main Algorithm
effectively having two different contraction matching algorithms of which
neither has all possible complexities.

*   Contiguous contractions need to handle missing prefix contractions, via
    partial matches when walking down a dictionary, but they need not handle
    combining marks. ("S2.1 Find the longest initial substring S at each point
    that has a match in the table.")
*   Discontiguous contractions need to check ccc values but skip both mismatches
    *and partial matches*. Only combining marks that extend a contraction string
    from one real match to another real match are taken. ("find if S + C has a
    match in the table") This also means that when a partial contiguous match
    fails, we need to back up to the last real match before attempting a
    discontiguous-contraction match.

Note: There is a small number of contractions starting with non-zero combining
marks in the DUCET (Tibetan) and in CLDR (Myanmar). See UTC doc L2/12-108. The
analysis and recommendations there predate the above insight and the writing of
contraction matching that is both fast and conforms to the existing UCA Main
Algorithm.

#### Prefixes first

When a character has both a prefix table and a contraction table, it seems
arbitrary which is the primary type (the special 32-bit CE from the trie lookup)
with the other one being a result of the first. It seems better to always make
the primary type a prefix-special because prefix matching does not consume text
and returns the iterator to the original character. (As a result, a prefix match
can return a contraction table but not the other way around.) By contrast,
contraction matching consumes following matching code points, and if the result
of a contraction were a prefix table, then we would need to go back to the
original character, match the prefix, and go forward again to after the
contraction.

Example for characters with both prefixes and contractions: Japanese has rules
like ヴ|ヾ/゙ = <30f4 | 30fe / 3099> which decomposes to <30a6 3099 | 30fd 3099 /
3099>.

TODO: We (CLDR) should require that prefixes start with a starter (A in "Amn|X")
and that their original character (X in "Amn|X") be a starter. Then we never
need to do a prefix match from a skipped combining mark in a discontiguous
contraction. And if a prefix must start with a starter (A) then contiguous
prefix matching automatically adheres to canonical equivalence because there
cannot be a discontiguous match (all combining marks between A and X would be
included in the prefix).

Better behavior: Require that A and X have NFC.hasBoundaryBefore() (they are not
NFC_QC=Maybe either).

CLDR-DUCET and CLDR tailorings have no [prefixes](prefixes.md) where either the
first character or the original character are non-starters. (That is, the A and
X in "Amn|X".)

TODO: Make sure that we perform canonical closure on prefixes.

#### Contraction overlaps

TODO: Detect if two contractions overlap, for example a+b & b+c. Print warnings.

#### Contractions vs. normalization

When we have a contraction overlap partially with a canonical decomposition
mapping, then v1 collation processing does not yield canonically equivalent
results. For example, Danish has the contraction a+a which is found in <a, a,
umlaut> but not in <a, a-umlaut>. The ICU User Guide lists this as a known
limitation. The only existing workaround is to feed ICU NFD input strings. See
ICU ticket [#9319](http://bugs.icu-project.org/trac/ticket/9319).

We should try to fix this in v2 if not in v1. Canonical equivalence should be
preserved for a normal tailoring and not-very-contrived text.

See the ideas in the ticket. Further notes:

Detect overlaps between contractions & decomposition mappings: (class
CanonicalOverlaps?)

*   Build a dm_trie with all decomposition mappings. Build another rev_dm_trie
    with reversed decomposition mappings.
    *   Code point reversal or code unit reversal?
    *   Lookup values could be all 0, or could be the composite-character code
        points.
*   Go forward through each trailing contraction substring, look in dm_trie, if
    non-final match then iterate through all further strings and append them to
    the input contraction to form additional contractions.
*   Go backward through each leading contraction substring, look in rev_dm_trie,
    if non-final match then iterate through all further strings and prepend them
    to the input contraction to form additional contractions.
*   Look up CEs for additional contractions, add those mappings.
*   We need to detect and prevent endless loops. See if it works to check if an
    overlap substring has been seen in an earlier phase (using two sets of
    leading & trailing overlap substrings). If it does not work, then limit the
    number of phases where we find new contractions.

TODO: Think about how prefixes affect this. Maybe we need to look at
prefix+c+suffix as the string for which we look for overlaps? Is it possible
that an overlap substring crosses to and beyond c?

Early decomposition of some set of characters could be a generalization of the
early Hangul decomposition. Use an additional variable with the lowest code
point in the set for quick checks. Maybe have a second set with the union of
early-decomposition and non-zero-FCD characters. Concern: Early decomposition
slows down all processing, probably as much as the FCD check.

If we did have a set of characters that we always decompose, then we could
remove all of their data (remove all prefixes/contractions that contain any of
them) and set them to fallback or implicit-tag values as appropriate.

Yet another idea: Always do a trie lookup in nextCodePoint()/previousCodePoint()
etc., and use a specials tag for characters that need decomposition before trie
lookup and for prefix/contraction matching. Core code would maintain a small
buffer for decomposition mappings. Pro: Characters that need not be decomposed
are not slowed down. Con: Slower prefix/contraction matching, and more complex
core code and iterator subclassing interface. Also every trie lookup is preceded
by at least a check for whether we are in a decomposition buffer, which could
noticeably affect performance.

TODO: Consider 0F71+0F71+0F72==0F71+0F73==0F73+0F71 (does canonical closure find
both?!). Needs (or should have) prefix contraction 0F71+0F71. Needs all
0F71+0F71+0F72+y, and equivalent, where y∈{0F72, 0F74, 0F80}, because of
overlaps of 0F73+0F71 with decomposition mappings. Worse, Richard Wordingham
claims that the DUCET contractions alone require an infinite set of overlap
contractions. See 2012-may unicore thread "Compliant Tailoring of Normalisation
for the Unicode Collation Algorithm".

TODO:

*   Check all prefix/contraction strings for FCD. Require them to be in FCD, or
    make them. (Which is better?)
*   Always add the NFD forms even if we don't apply canonical closure?
*   Make sure that canonical closure only adds FCD prefixes/contractions.

We only need to add canonically-equivalent contraction strings that also pass
the FCD check. Anything not FCD won't match FCD input text.

#### Contractions with 0F72, 0F74, 0F80

These characters are second in the decomposition mappings of 0F73, 0F75, 0F81
respectively. They have a higher combining class (130/132) than the first
character 0F71 (129) in those decomposition mappings. Therefore, for canonical
equivalence when processing FCD text, if there is a contraction x+0F72, then we
should find a discontiguous contraction in FCD text x+0F73. This is not a
problem if there is also a contraction for x+0F73. (See 2012-may unicore
discussion "Compliant Tailoring of Normalisation for the Unicode Collation
Algorithm".)

TODO: Check & require that if there is a contraction x+0F72, then there must
also be a contraction x+0F73 (and the canonically equivalent x+0F71+0F72). Same
for the other two pairs.

Note: When adding x+0F71+0F72 it is strongly recommended to also add the prefix
contraction x+0F71 for expected discontiguous-contraction matching (see section
"No contraction closure" and UTC doc L2/12-131R). This then causes overlap
issues with the other decomposition mappings of 0F71+y (see section
"Contractions vs. normalization") so we might also need to add x+0F71+0F74 and
x+0F71+0F80, etc.

Note: Another option might be to add an explicit test for 0F73, 0F75, 0F81 into
the incremental-FCD check and always decompose those before they reach the core
collation code. We would then also have to modify the bulk FCD normalization
used in sort key generation, or add another pass over the string, or always
normalize to NFD. See <http://bugs.icu-project.org/trac/ticket/9324> "FCD
normalization: consider always decomposing Tibetan composite vowels"

TODO: My reply on 2012-may-17 on unicore thread "Compliant Tailoring of
Normalisation for the Unicode Collation Algorithm":

> The DUCET has the contraction 0F71+0F72, and we should find a discontiguous
> match on <0F71, 0F71, 0F71, 0F72> skipping the two middle 0F71. That string is
> equivalent to the FCD-passing string <0F71, 0F71, 0F73> but there is no 0F72
> in sight there to complete the match if we don't modify the string.

> If we cannot find a way to handle this with a finite (actually, small) amount
> of data, then we either have to decompose those three Tibetan composite vowels
> before they reach the core collation code, or, frankly, we just document a
> limitation for ICU and point to the fact that [the use of these three
> characters is "discouraged"](http://unicode.org/charts/PDF/U0F00.pdf) and they
> don't occur in any normalized text (e.g., NFC).

> The more I think about these the more I believe I could live with such a
> limitation. If we could get our code to support all of UCA, provide a dozen
> runtime attributes, compare strings and return two kinds of sort keys, be
> fast, and deliver correct results on all FCD input except if these three
> characters are involved, I would be quite happy.

> Maybe we could lobby to change these characters to be "strongly discouraged"
> or "deprecated" or "too hard to implement"...

Proposal:

*   Do best effort with canonical closure, requiring/adding prefix contractions
    (adding with warning?), adding overlap contractions (with warning?).
*   When normalization mode is off, document that input text should pass the FCD
    test ==*and*== should not contain 0F73, 0F75, 0F81. (Although some text
    containing them will sort as expected.) Mention that they are "discouraged"
    and do not occur in any normalized text (e.g., NFC).
*   When normalization mode is on:
    *   Incremental FCD check decompose (NFD) any segment surrounding 0F73,
        0F75, 0F81.
    *   getSortKey() always use NFD.

#### Hangul

v1 implementation behavior for HANGUL_SYLLABLE_TAG: Text iterator delivers
Hangul syllable, lookup yields a CE with the Hangul syllable tag, specials
handling decomposes it. if(!jamoSpecial) then fetch the L/V/T CEs and create a
simple expansion, otherwise decompose into the normalization buffer and return
an ignorable CE (0) to later switch into that buffer.

The v1 design doc is slightly different: It fills the side buffer and then
*recursively* calls the CE-fetching code and returns the first resulting CE. It
then says "Note that because of this special processing of Hangul Syllables, we
do not allow contractions between Hangul Syllables and other characters." (It
should then also forbid Jamos in prefixes, and even contractions of Jamos are
probably not supported.)

v2: In general, decomposing Hangul syllables before looking up their CE32 values
looks cleanest because it presents a consistent character stream.

Consider the alternative: Read a syllable, fetch its CE32, detect its special
Hangul tag, only then decompose and switch to a Jamo side buffer. This means
that nextCodePoint() & previousCodePoint() deliver Hangul syllables when they
move outside that side buffer.

*   Prefix rule T | a & text (LVT)a: Prefix matching sees only the syllable
    without fetching and acting on its CE32.
*   Contraction LVTa & text (LV)Tb: Contraction matching will move to b, find a
    mismatch, backtrack, but probably the Jamo side buffer has already been
    abandoned because we moved beyond it, and backtracking does not look up
    CE32s.
*   Contraction aL & text a(LV): Contraction matching fetches (LV) but does not
    look up its CE32 and thus will not find its L.

The other way to avoid these issues (like the v1 design doc says) would be to
add constraints on how Jamos can be tailored, forbidding anything that causes
trouble with the decompose-after-lookup processing model.

Decision: At least initially, always do late decomposition and lookup of
no-context CE32s for L/V/T in the HANGUL_TAG specials handling. If the tailoring
says that early decomposition is required, then handle that in CollationIterator
subclasses, so that the core code will never see the HANGUL_TAG. (Revisit if
this causes undue performance degradation.)

Builder: Hangul syllables need to be decomposed early when any Jamo is part of a
prefix or part of a contraction. In that case, set a flag that will be carried
into the runtime iterator.

Hangul syllables need specials tags, for on-the-fly decomposition when there is
no special handling.

Note: With this processing model, we might not need canonical closure to include
Hangul syllables.

TODO: On the other hand, early decomposition of Hangul slows down all
processing, probably similar to the incremental FCD check. We would need less
runtime code, and could process faster, if we either kept and enforced tailoring
limitations or added contractions to bridge tailored context and Hangul
decompositions like we might do anyway for contraction-decomposition overlaps
anyway.

TODO: We could handle Jamo expansions by nested appending CEs for V & T like we
do for skipped marks from discontiguous contractions.

TODO: Initially, forbid expansions/contractions/prefixes for conjoining Jamo.
Get it to work, then see if there is a CLDR tailoring or other demand that
requires more complexity. Also see
<http://www.unicode.org/reports/tr10/#Hangul_Collation> for which Hangul sorting
method needs what collation engine complexity.

#### Contractions in backward iteration

These are probably the most complicated part of a collation implementation. We
need to either have reverse-contraction tries indexed by the last contraction
characters (I believe v1 does this) or skip backwards across "unsafe" characters
and then collect CEs again going forward.

*   Combined with prefixes: Prefix matches would have to be results of backward
    contractions (opposite of forward iteration) and a non-matching prefix would
    have us retract the contraction match. A backward prefix trie would have to
    be different from the forward version: The lookup result would be a normal
    CE, never a contraction.
*   Combined with discontiguous contraction: Might be even more "interesting"
    than when going forward.
*   Overlapping backward contractions: Going forward, we do a simple, greedy,
    longest-match. We cannot do that going backward or else we get different
    results compared to going forward.
    *   Given contractions: abc & bcd -- when iterating backwards over abcd, we
        need to get CE(abc) + CE(d), not CE(a) + CE(bcd). \[Does this work in
        v1? If so, how?\]
    *   Contractions ab & ba -- strings ababa vs. baba
    *   Contraction bb -- strings bbbb vs. bbb
    *   Simplest would be to skip backwards over any sequence of characters that
        occur after the first character in any contraction
        (non-initial-contraction-characters), and go forward from there.
        *   Vladimir says 20120124 that v1 does this.
        *   We might skip too far when that sequence of "unsafe backward"
            characters does not match any sequence of contractions.
    *   Using a trie, we could store a special-tag for "another contraction ends
        here" which needs an optional result CE if we also get a
        back-contraction result at the same time. (It could be the same
        special-tag as the one that gets us into the back-contraction to begin
        with.) Whenever we get another contraction terminator, we could record
        it like in the UnicodeSet string span, and loop with backward matching
        until we get to a state with only a single string location; then we
        iterate forward from there up to where we started.
    *   If we need to deliver "tightly fitted" indexes for CEs (for string
        search?), then the forward iteration could record the index mapping in
        some form; pseudo-CE64s or parallel structure with either string index
        deltas for the next n CEs, or with start and limit string indexes.
    *   When there is no overlap, we can get a simple, normal CE result from a
        backward contraction trie and just stop there. This can happen when the
        contraction contains no other contraction terminators, or when none of
        them lead to a match.
    *   If the original back-contraction itself does not lead to any match, then
        we need to deliver the non-contraction result of the original terminator
        character.

It would really help if we could restrict contractions to avoid most of these
complications.

TODO: How tightly does the character iterator need to move when iterating
backwards? How tight does it need to be for string search? Do we still support
Boyer-Moore search? -> Vladimir says 20120124 that we don't (need to) deliver
indexes.

Q: Can we forbid prefixes and contractions to contain decimal digits? Otherwise
we get complicated interactions with CODAN. Or, CODAN simply overrides anything
else.

TODO: How do tailoring and CODAN combine?

*   Do we reapply the digit-tag to tailored decimal digits (&a=1)?
    *   Simple: Apply the digit-tag to any tailored Nd.
*   Do characters get the digit-tag if they are tailored to sort like digits
    (&1=a)?
    *   Simple: See above, non-Nd do not get the digit-tag.
*   What about digits in expansions? '('<<<（<<<⑴/**1**')'<<<⑽/**10**')' -- no
    CODAN because ⑴ and ⑽ are not Nd? (Otherwise we might need specials in
    64-bit CEs for conditional CODAN handling.)

#### Switching iteration direction

Do we need to support this, or is it sufficient to start iterating in either
direction from a fresh start somewhere in the input text? If we do, then
nextCE() and previousCE() need to count how many code points they consumed for
their set of CEs (1 except for contractions and unsafe-backward handling). When
we switch direction, we need to move by that many code points in the opposite
direction.

*   [ICU4C
    CollationElementIterator](http://icu-project.org/apiref/icu4c/classCollationElementIterator.html)
    documents: "The Collation Element Iterator **moves only in one direction**
    between calls to CollationElementIterator::reset. That is,
    CollationElementIterator::next() and CollationElementIterator::previous can
    not be inter-used. \[...\] **If a change of direction is done** without a
    CollationElementIterator::reset(), **the result is undefined**."
    *   Same text in C ucoleitr.h file documentation.
    *   Presumably, the direction can also change after calling setOffset().
*   [ICU4J
    CollationElementIterator](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/CollationElementIterator.html)
    documents: "... when you change direction while iterating (i.e., call next()
    and then call previous(), or call previous() and then call next()), you'll
    get back the same element twice."
*   TODO: Resolve the conflict! ->
    <http://bugs.icu-project.org/trac/ticket/9104>

#### Possible lower-level backward-iteration support

Rather than the ideas above, since backward iteration is rare, we might just do
something simpler: When we are set up to go backwards (or the first time a
"previousCE()" API is called), we iterate forward from the start and record in a
compact but simple way enough information to find string indexes where we can
start iterating without context. For example, record in a byte vector how many
code points are inside vs. outside contractions. When we go backward through a
contraction-free region, we just pick up CEs from each code point, until the end
of that region (this would include the skipped combining marks from
discontiguous contraction).

When we go backwards and hit a contraction region, we skip further back to its
beginning, iterate forward, collect the CEs, and store them for backward
delivery. We might need to record other regions as well, for example regions
that are modified in normalization.

#### "Unsafe-backward" set

(for identical-prefix-comparison backup, and backward iteration after finding a
contraction terminator)

UCA: USerializedSet, unserialize & freeze on load. Include ccc!=0. For prefix
data like Amn|Bpq, all but A need to be in the unsafe-backward set.

*   TODO: Include Nd for CODAN (probably not worth two sets, standard vs.
    CODAN).

Tailoring: Only store a set if "worse" than in UCA. If worse <=U+0FFF, store bit
set including ccc!=0. If worse >U+0FFF (or anywhere), store USerializedSet with
delta over UCA.

Simpler would be to always clone the UCA set and add the delta and freeze, but
that would cost some space & time, and we always need the unsafe-set for string
comparisons.

TODO: Check CJK tailoring contractions. Would they need unsafe-set deltas? Above
U+0FFF? Bit set for 3000..33FF??

\[Probably start simple, and optimize the serialized data structure if
necessary.\]

Note: A simple implementation without backward iteration and without
identical-prefix optimization would not need the unsafe-backward set.

#### Contraction terminator set

(for backward iteration from a known-good string position)

Always USerializedSet, use directly. Could unserialize & freeze for UCA.
Tailoring set for just the delta.

#### No "unsafe-backward" set?

We might be able to use an optimized version of the
contraction/normalization-region-recording technique for the
identical-characters-prefix string comparison as well, instead of the "unsafe
backward" set which might then not be necessary at all. \[Unlikely:
Identical-prefix comparison does not do any lookups, so we won't know whether we
saw the start of a contraction.\]

### Case handling

Keep case handling the same as in v1 (minus bugs), and document it better.

When the caseLevel is on, we *==move==* the case bits from the tertiary level
into their own level. We never have case bits in both the case and tertiary
levels.

When both caseLevel and caseFirst are off, then we remove and ignore the case
bits. (This is the default.)

When caseLevel is off but caseFirst=lowerFirst or caseFirst=upperFirst, then the
case bits are kept in the tertiary weights and trump other tertiary differences.

We use two bits with three values for case: 0=uncased/lower vs. 1=mixed vs.
2=upper. We treat these bits almost like a separate level, but we do not have
the usual 0 weight for "ignorable". Instead, we treat the case bits as ignorable
when the non-case tertiary bits are 0=ignorable. Rationale:

*   When the caseLevel is turned on, and the remaining tertiary weight is 0,
    then we anyway have to ignore (or must not have) case bits to keep the CE
    well-formed. (No 0 weight on any level if the previous level is non-zero.)
*   When the caseLevel is off, then on tertiary level we also consider only the
    non-case bits for testing "ignorable". This means that if we tailor
    &\\u0001=b=B=c then b==B even if caseFirst is not off; in other words, both
    b and B are treated as tertiary ignorable even with caseFirst=lowerFirst or
    caseFirst=upperFirst. This is desirable because we can't distinguish
    ignorable-case from lowercase, and thus b gets an all-zero,
    completely-ignorable CE. In particular, this prevents bBc==bcB.
*   Therefore, we *always* ignore case bits if the non-case tertiary weight is
    0. We should construct the table this way and rely on it in the code.

TODO: Make the CollationDataBuilder verify that case bits are 0 if non-case
tertiary bits are 0.

TODO: The builder must set case=upper for tertiary CEs. With caseLevel=off &&
caseFirst=upperFirst the runtime tertiary-level code must keep case=uppercase
(don't invert) for tertiary CEs to keep them well-formed. (0.0.t -> 0.0.4t see
LDML 5.14.13 Case Parameters).

### Fewer specials

Several of v1's special CE tags are now unused, e.g., for Thai prevowel
processing. We should discard them and use fewer primary lead bytes for the
specials range. (Specials for lead surrogates with and without supplementary
data may or may not make sense.) We could use multiple compact forms for
different kinds of "normal" CEs so that we don't need specials for them
(long-primary & expansions); for example, leading 00 byte for long-secondary CEs
(0:16:8) and trailing 00 byte for long-primary CEs (24:0:0).

Support Unified Ideographs via an "offset" special that can be used for other
code-point-order ranges as well, see below. Since we do not encode long-primary
CEs as a special, we do not need 24 bits for the specials payload, and therefore
we can reduce the lead bytes for specials from 16 to 1.

#### No special Han implicit weights

We could instead use an OFFSET_TAG special for Unified_Ideograph blocks as well
as other large code-point-order blocks (e.g., UCAS, Yi, Egyptian Hieroglyphs)
which reduces their size too while sharing code.

Idea: We could save a little bit of base table space by storing a specials value
for Han compatibility characters. They are mostly allocated in dense ranges and
are probably not used very often. At runtime, we could get their decomposition
and get their trie value via a second lookup. Pro: Save some 4kB for the 1002
compatibility ideographs. Con: More core engine code.

#### No hiraganaQuaternary attribute & special

Replace the hiraganaQuaternary attribute & special CE tag with a "true"
quaternary weight in the 64-bit CEs. See the section near the top about 64-bit
CEs.

### More default collators

CLDR has been adding several collation types that combine with smaller
language-specific tailorings. As a result, we have more and larger tailoring
tables than before.

*   Note: We might not care as much about unihan collators if they are excluded
    from stock ICU.

Decision: At first, do not add anything clever. Get UCA/DUCET to work, get
normal tailorings+import to work, turn off old implementation, then think about
the problem of multiple bases.

*Idea: Multi-level fallback. tailoring -> base -> UCA*

Easy at runtime.

Challenge: How to build an intermediate base from a rule string with gaps for
further tailoring.

Try to write a third builder type (UCA vs. tailoring vs. intermediate base).

Variant: Try to build one base, but with optionally-used mappings for
search/unihan/eor characters.

Idea for combining multiple base tables into one FractionalUCA.txt:

*   Add mappings for each base variant (ducet, search, eor, etc.).
*   Reserve their weights in the "inverse" table but do not store them in the
    CLDR root base table.
*   Use syntax to distinguish them; either something with the name of the base,
    or a noncharacter per base (e.g., FDE0, FDE1, ...)
*   For each base variant, store a delta table with its overriding mappings.
    Probably UTrie2 etc.? Share expansions etc. with main table?
*   CE lookup: Fall back to the variant then to the shared base.

Idea for compact Unihan table:

*   Store ranges (start & limit) of \[:Unified_Ideograph:\] to convert them into
    linear indexes. First=1 for tailoring before it.
*   Store a table indexed linearly by that, with 20 bits per character. Unicode
    6.1: 74,617 characters \* 2.5 bytes = 186542.5 bytes.
*   Use only the upper 18 bits. Leave the lower 2 as 0. (Looking up 20 bits per
    character is easier than looking up 18.)
*   Split the 18 bits into 7:8:3, add 0x7d to the first byte for a total range
    of primary lead bytes 7D..FC. No need for regular OFFSET_TAG for Hani
    because all of them are remapped using this table, so we can go up to just
    before FD for unassigned implicits.
*   Store second bytes directly; avoid bytes 00 & 01.
*   For third bytes, add an offset of 2, for 02, 22, 42, ..., C2, E2. (With
    smaller gaps at byte boundaries than elsewhere.)
*   Allows for 128 \* 254 \* 8 = 260,096 three-byte weights with gaps of 29 or
    31.

*Alternative: Multiple bases, either plain UCA or "search" or "eor" or "unihan"*

Challenge: The base must provide for tailoring gaps.

Idea: Multiple bases, *each built from a rule string* (support \[import\]), none
(not even UCA) from special data like FractionalUCA.txt. Build a base with gaps.
(Building a base might be a fairly small change in the from-rule builder: Allow
resets only to \[first ...\] or similar, or to previously-seen tokens; allocate
weights in order with gaps rather than putting weights between anchors.) In
tailoring rules, select a base via \[import ==*und*==-co-xyz\] or via new \[base
eor\] or similar.

Opportunity: By using the rule string rather than FractionalUCA.txt, a
reimplementation has more freedom, hopefully does not require any changes to the
FractionalUCA/UCARules builder tool. (UCARules is also
version-control-friendlier than FractionalUCA...) On the other hand, it means
completing substantial builder code early, and fixing or hooking up the rule
parser, rather than front-loading most runtime work.

TODO: Do the UCARules represent all necessary information?

*   (Except for easy things like using single-byte primaries for \[ 0-9a-z\],
    and canonical closure.)
*   Add syntax & data for extra weights & gaps at reordering-group boundaries,
    and for which scripts lump together.
    *   ... < \[first digit\] < 0 < ...
    *   ... < \[first Grek compress\] < alpha < ...
    *   ... < \[first Cyrl Glag compress?\] < cyrl-a < ...
*   Hardcode which scripts are "unusual" and are ok with 3-byte primaries?
*   Might be able to compute which scripts are compressible: Latn, and ones that
    fit into a single lead byte.
*   Rather than "& \[last primary ignorable\] << ..." the rules should have "&
    \[first primary ignorable\] << ..." with \[first primary ignorable\] = \[0,
    05, 05\] -- UCARules.txt probably uses \[last\] so that the rules can be
    applied on top of FractionalUCA.txt without blowing up the first gap. We can
    probably work around this by initially setting \[last\]==\[first\] when
    building a base.
*   Do the particular tertiary weights matter if there is a secondary one? Do
    the particular secondary weights matter if there is a primary one?

Challenge: Data for a base is large, especially together with the invuca table.
(Unicode 6.1: ucadata.icu 186228 bytes, invuca.icu 307242 bytes)

*Idea: Partial intermediate table between tailoring & UCA.*

*   Less duplication.
*   Might not use a trie at all.
*   Might make most sense for unihan because it orders Han characters in the top
    range while tailorings could order other characters elsewhere.
*   In principle, we could store 20-bit primary weights per Unified_Ideograph.
    Index by dense order of ideographs in DUCET order (Unihan & compat first,
    then all others).
*   Problem: If there are also multiple bases, then which is the first lead byte
    for the top range?
*   The tailoring on top of an intermediate table could provide data for access
    into the intermediate table. (Constructed to be trie-compaction-friendly.)
*   The intermediate table might not be usable by itself.

TODO: Understand the invuca data structure (see
[ucol_imp.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/i18n/ucol_imp.h)
around line 160) and the process of tailoring one or more items before/after a
reset point. Think about how to make it more compact.

*   Do we need to actually map to code points/strings?
*   Do we need to store expansions?
*   For exansions, do we need to store all CEs or all primary CEs (to avoid
    overlaps with tertiary tailoring) or only the initial CEs?
*   Do we need to store the artificial secondary CEs? (Otherwise tailoring could
    generate them for real characters; do we care?)

Assume/enforce that primary weights in the base use at most 3 bytes, secondary &
tertiary ones at most 2 bytes.

If possible, store single CE32's, with an escape (e.g., FFtiiii1) for an index
to a 64-bit CE. If this works, it would reduce the invuca size by two thirds.

**class CollationAnchors** data structure: (it is not really an inverse; maybe
CollationLimits? CollationBoundaries?)

*   principles
    *   binary searchable array of 32-bit integers
    *   no need for an additional index table
    *   do not store unassigned implicits (handle them in code)
    *   store single-CE primaries directly (with single-byte tertiary weights)
    *   store code-point-order ranges as ranges (pairs of integers)
    *   store non-common secondary & tertiary weights as deltas
    *   store the common secondary & tertiary weight bytes in the header (store
        both, just in case we want them to be different)
*   constraints
    *   explicit primary weights use at most 3 bytes
    *   no case weights (the tailoring builder creates them from scratch)
    *   no quaternary weights
    *   all secondary & tertiary weights >= common
*   array structure
    *   last unit: limit sentinel 0xff000000, looks like primary-weight unit but
        larger than any actual primary
    *   **bit 7**: type
        *   0 primary
            *   bits 31..8: primary weight
            *   bits 6..0:
                *   0 for single CE or range start
                *   otherwise this is a 7-bit range step value, and then the
                    weight is the last primary in the range
                    *   a range end always follows immediately after a preceding
                        primary which itself can be a range end
                    *   Note: Storing the range end rather than the range length
                        makes it easier to detect if an input primary is at the
                        end, to look for the next primary. Also easier to find a
                        primary before an input primary when a range precedes
                        the input (look backward until type=primary).
            *   support ranges of 2-byte and 3-byte primaries; these ranges are
                not necessarily aligned with offset-tag mapping ranges
        *   1 sec/ter delta (modifies last primary from type 0)
            *   bits 31..16: secondary
            *   bits 15..0: tertiary (00 case bits, 6 bits t1, 1 bit type, 1 bit
                unused/0, 6 bits t2)
*   runtime
    *   after(ce, strength)
    *   before(ce, strength)
    *   return input ce if an error occurs (e.g., ill-formed input ce, or ce not
        found)
    *   clear case bits
    *   binary search for a primary
        *   from a mid-point, linear search forward until we have a primary,
            return
        *   if none before the limit, then linear search backward until we have
            primary, return
        *   if none after the start, then stop the search, return the start
            *   if anchor\[start\].type==range end, then handle the range; else
                linear search forward through the deltas to find the matching CE
            *   if the input ce is in fact in the range, then we can just
                add/subtract the step to get the next/previous one; we can
                double-check the ce against the range start/end
    *   construct secondary upper limit: use 0xFFFF; the weight allocator might
        recognize this and make FFFF itself available?
    *   construct tertiary upper limit: use 0x3FFF; weight allocator should be
        able to make single byte 3F available
        *   or maybe use 0x10000 & 0x4000 if the sec/ter allocator does not
            left-align its weights
    *   secondary/tertiary lower limits before the common weights could use lead
        bytes 01 so that the weight allocator can start at 02

Older idea:

*   More compact: Some binary searchable CEs; in between: sequences of 16-bit
    deltas
    *   top 2 bits=00: rest is tertiary weight, primary/secondary same as
        previous
    *   top 2 bits=01: rest is secondary weight>>2, tertiary=common, primary
        same as before
    *   top bit=1: secondary/tertiary=common, increment p1 by 1 or 2, set p2,
        set p3>>2
    *   use some forbidden combinations to indicate that a number of CEs differ
        by a constant delta in the second or third primary byte, with
        secondary/tertiary=common
    *   escape: if a weight does not fit such a delta encoding, then just store
        the full CE as another binary searchable one
    *   insert full CEs at least every so often to make some use of binary
        search

### Optimize the base data

TODO: The DUCET creates secondary differences by adding secondary CEs, rather
than by changing secondary weights of primary CEs. We should change the
FractionalUCA.txt (and allkeys_CLDR.txt) generator to merge those artificial
secondary CEs into their preceding primary CEs. Exception: The l-with-middle-dot
mappings must remain as expansions so that the prefix syntax works for the
middle dot. (See private email Markus/Mark/Ken in 2012-jul "secondary weights in
DUCET/allkeys.txt".)

TODO: Consider spreading out tertiary weights for the same primary+secondary
combination, for larger tertiary tailoring gaps. For example, if there are very
few tertiary weights, create a gap of 10 or 20 or more after the common weight,
and maybe another larger gap after the DUCET "uppercase" weight, or spread out
the weights evenly after the after-common gap. Check tailorings for lengths of
tertiary-difference chains, and for where we generate two-byte tertiary weights.

Try to use one compressible lead byte for Cyrl+Glag. Two-byte primaries only for
common Cyrillic letters (as many as fit). Pro: Shorter sort keys, saves a lead
byte. Con: Expansions for uppercase Cyrillic letters with three-byte primaries.

*   Decision: Rejected. Use non-compressible two-byte primaries for both
    Cyrillic and Glagolitic.

Try to use one compressible lead byte for Arab+Syrc+Mand.

*   Decision: Rejected. Too many primaries, too many of them with non-common
    secondary/tertiary weights.

We could use 3-byte primary weights for Ethi & Mymr, saving 4 lead bytes. Most
of their CEs could be long-primary CEs, and single lead bytes would allow
compression, so sort keys would hardly be longer. (Ethi will not yield offset
ranges, which is ok for performance, unless we squeeze characters from extended
blocks into gaps between main-block characters, which is not worth it.)

*   Decision: Done.

It would be great if exactly all scripts between Latn & Hani were compressible:
Simple test, no extra data.

*   Decision: Rejected, see the following.

On the other hand, the base data would be smaller if we used 2-byte primaries
where there are secondary or tertiary variants, especially cased letters.

*   3-byte primaries with uppercase require expansions. FractionalUCA 6.2 has
    them for Coptic, Glagolitic and Deseret. This might argue against making
    Cyrillic use 3-byte primaries.
    *   ==Coptic has 60 primary weights. They fit into the Greek lead byte even
        as two-byte weights.==
        *   Done: Coptic changed to two-byte primary weights.
    *   Cyrillic has 172 primary weights. At least 50 or so would have to use
        3-byte weights in order to fit just Cyrillic into one lead byte.
    *   ==Glagolitic is balanced by its lowercase range allowing for a 3-byte
        offset range. However, its 47 primary weights easily fit as 2-byters
        into Cyrillic's second lead byte.==
        *   Done: Coptic changed to two-byte primary weights.
    *   Arabic has 202 primary weights. At least 80 or so would have to use
        3-byte weights in order to fit just Arabic into one lead byte.
    *   Deseret is balanced by its lowercase range allowing for a 3-byte offset
        range continuing into Shavian. Decision: Keep Deseret with three-byte
        primaries.
*   Look for scripts that are small and have secondary or tertiary variants
    (which would do better with 2-byte primaries -- check for single-code point,
    single-CE expansions) vs. scripts that have code-point-order ranges (which
    are supported efficiently with 3-byte primaries).

If necessary, we could lower the common weight bytes to 03 or 04. More room for
normal weights, less room for \[before 2\] and \[before 3\].

*   Decision: Rejected for now, seems unnecessary.

### Simpler data file structure

We could have separate .res file entries for the UTrie2, contractions+prefixes,
expansions (new int64vector type), attribute values, etc. The ures_swap()
function could detect and swap simpler types directly.

Unsure if this is worth it. We would either have to build the genuca code into
genrb, which is poor layering, or generate a number of files with binary data
etc. for each resource item. It might be simpler to keep ucadata.icu &
invuca.icu but redo their data formats from scratch.

## Builder

### Simple low-level builder

We should have a builder class that just takes (input, value) pairs where input
is code point or string and output is a CE or CE sequence. The CE could be given
as a triple of 32-bit primary/secondary/tertiary weights, and the builder could
look up the case information. It could use an interface so that a case
properties provider could be injected (so that we could merge genuca into
genprops if we wanted). Clean, simple, separate from the rule parser and used
from genuca as well. Use simple data structures for contractions & prefixes
until all data is collected, and only then turn them into runtime data
structures. (Rather than rebuilding runtime structures on the fly.)

TODO: Even simpler builder & genuca: Have genuca & the low-level builder not
perform canonical closure. Make the low-level builder not depend on
normalization data, or add a setter for the fcd16_F00\[\] array. Make
preparseucd.py write a header file with the fcd16_F00\[\] array, include into
genuca.cpp. With this, genuca can be run at the same time as genprops, without
rebuilding the ICU library in the middle.

With a simple, separate builder, unit testing should be much simpler. We could
build very simple collators (not full UCA & CLDR tailorings) with known CEs.

Make sure the tailoring builder detects redundant rules (putting a character
after another when they are already in that order according to the base
collator) and optimizes them (re-anchor at the other character, or just merge
token lists, as appropriate).

*   Decision: Only replace the low-level builder. Reuse/refactor the rule parser
    & tokenizer.

Make sure the builder (base or tailoring) detects collisions in canonical
closure, where an auto-added mapping results in different CEs than an explicit
one.

*   Decision: See above. Mimic (or possibly improve) existing canonical-closure
    behavior.

TODO: It is possible that we build unreachable expansions or context tries, by
setting and then overwriting values. We could detect that by using functions for
setting & changing CE32 values that test for such older values and setting a
flag. At the very end we could copy the final data into a new builder. Might be
worth doing only if we don't serialize the collator. (Build time vs. final
size.)

### Prefix canonical closure

We probably need to apply canonical closure to prefix rules like ガ|ー. Does v1 do
this? Apparently not: <http://bugs.icu-project.org/trac/ticket/9444>

We probably need to apply the cross-product of the prefix & contraction
closures.

U+0387 (GREEK ANO TELEIA) canonically decomposes to U+00B7 (MIDDLE DOT).

Japanese has rules like ヴ|ヾ/゙ = <30f4 | 30fe / 3099> which decomposes to <30a6
3099 | 30fd 3099 / 3099>.

### Reset to string

Verify that v1 tailoring treats &ch=x and &c=x/h as equivalent.

*   Done, it does. It passes the reset string into the UCA collator, fetches the
    first complete CE and checks where the iterator stopped. The remaining text
    becomes the deferred expansion.
*   TODO: It should not just remember the remaining text but the whole string
    with that position. Then a reset on l+middle dot would get the correct
    prefix CE for the middle dot.

They probably need to be different if ch is a contraction or if there is a
prefix rule for c|h. Or maybe &c=x/h should then still be treated as equivalent
to &ch=x??

Q: How does this extend to &ch=x<y<z vs. &c=x/h<y/h<z/h?

*   See the User Guide Customization chapter.

TODO: We might need additional syntax to completely represent the UCA ordering.
However, this is only really needed for the UCA if we want to build it from
rules. Otherwise, the DUCET allkeys.txt does not have prefix rules, it already
treats l+middle dot as a contraction like the UCA rules string.

### Tailoring miscellaneous

TODO: Document that we forbid reset relative to an unassigned code point, at
first. Including relative to "first unassigned" and primary-before \[first
trailing\].

### Tailoring Builder

(From rules)

TODO: Experiment with CLDR rules, instrument the collation builder and build ICU
data:

*   How often do we tailor an item (character or string) that has already been
    tailored? (Which removes the old tailoring from its token list?)
*   How often do we reset on an item that has already been tailored? (Which
    inserts the following tailorings into the existing token list.)
*   If these are very rare, then we need not use much book-keeping to find such
    items quickly; we could just use a UnicodeSet of tailored items (and
    probably reset anchors), and if there is a duplicate then we do a linear
    search.

TODO: Does it make sense to refactor the existing from-rules builder, changing
it to using 64-bit CEs and writing to either the old or the new low-level
builder? Or is it better/easier to copy pieces of the old from-rules builder for
writing a new one?

TODO: suppressContractions -> suppress prefixes too? (that would be easier:
don't copy any context)

TODO: \[suppressContractions \[ab\]\] -> &ab<x -> does this treat ab like a
contraction and get the root contraction CE, or is that contraction suppressed?

#### Notes from 2012q2

&reset:

Rule parser immediately assigns CEs to a reset that was not previously tailored
(nor a prefix was tailored).

Performs UCA lookup with standard collation iterator.

Notes expansion starting from where the iterator stopped after first CE.

This handles resets on contractions but could be trouble with discontiguous
contraction.

TODO: Factor out CE storage (uint32_t vs. int64_t),

factor out fetching of first CE & length of reset prefix from which it was
generated,

factor out upper & lower bounds and weight allocation.

Generally, try to keep as much of the existing rule parser as possible.

Find CE lookups (collIterate and ucolIndirectBoundaries\[\])

and invuca lookups (ucol_inv_getPrevCE()) and factor them out.

Create a new from-rules builder using the same parser but

injecting the new lookups and weight allocation.

Probably replace all of ucol_bld.cpp/ucol_assembleTailoringTable() but call
shared code.

TODO: Builder: add(prefix, s, ces\[\], cesLength, UErrorCode &errorCode)

1. Turn list of CEs into a CE32, store unique list of CE32s or CE64s if it/they
do not just fit into the main CE32. Max 31 CEs.

2. Canonically iterate over string/code point/contraction, set
prefix+first+suffix->ce32.

Max 31 prefix NFD length, max 31 string NFD length.

3. Canonically iterate over prefix, set actual values on
prefix|first->suffix->ce32.

TODO: Builder: If we only need to look up CEs from an unfinished collator for
canonical closure, then we could probably remove the BUILDER_CONTEXT_TAG and do
what the v1 builder does: build a collator from the pre-closure data and use it
in the closure process.

#### Notes from 2010/2011

CollationRuleParser parses a rule string (and \[import\]) into token lists.

CollationTokens stores token lists (resets vs. orderings) and applies canonical
closure.

CollationBaseBuilder turns tokens into base-collation data. (This might live in
genrb??)

CollationTailoringBuilder turns tokens+base into tailoring data.

CollationDataBuilder presets with IMPLICIT_TAG and Han OFFSET_TAG; encodes and
stores CEs for characters, contractions & prefixes, applies DIGIT_TAG for
\[:Nd:\].

TODO: Design iterator+data+builder so that it's easy to get CEs for a string
(from start index) from non-final builder data, without building final data and
without duplicating all of the lookup code to a version dedicated to the builder
data.

*   CollationData: Use array of 16 function pointers for specials dispatch
*   Use some specials tags only for in the builder, for non-final data. Builder
    provides subclass which sets function pointers for those specials, for
    lookups in non-final data.
*   Builder might only need non-runtime code for context handling (prefixes &
    contractions).
*   Consider one single specials tag & data structure for combined
    prefix-and/or-suffix data. Linear search through list of ConditionalCE for
    the code point. Might get away with not supporting discontiguous
    contractions.

/\*\*

\* Low-level CollationData builder.

\* Takes (character, CE) pairs and builds them into runtime data structures.

\* Supports characters with context prefixes and contraction suffixes.

\* TODO: Consider handling canonical closure here rather than on the token lists
level.

\*/

class CollationDataBuilder : public UObject {

// store relatively simple values in or close to final form

UTrie2 -- for values that can be encoded as a ce32

array of pointer to ConditionalCE { // indexed while adding data from prefix
and/or contraction ce32; one entry with empty strings for code point alone, if
it by itself has a value

UnicodeString prefix;

UnicodeString suffix; // for contraction

uint32_t ce32;

int32_t next; // index of next of same kind; Java would probably use Maps from
context strings to ce32 values instead (would that work for context-sensitive
lookups??)

};

UVector32 -- only final vector for ce32 that are displaced by specials tags,
e.g., DIGIT_TAG

UVector64 -- for expansions; probably one for while adding & overriding data,
and one for final expansions array

get/lookup CEs for string -> UVector64

setCEs(

const UnicodeString &prefix,

const UnicodeString &s, // code point with optional contraction suffix
const int64_t &ces, int32_t length, // no expansion string here
UErrorCode &);

};

**CollationTokens**:

Store a UnicodeSet of all reset anchors and tailored code points & strings.

Store a list of reset anchors, each with its tailoring data.

*   TODO: Can we instead store a single list of tokens, with the resets
    intermingled with other relations? Keep it simple...

Each reset anchor is stored as an object that contains:

*   TODO: Can we do this on the fly, and just store the reset position token in
    the list?
*   The reset token (code point or string), with an indication for whether it is
    a reset-before or reset-at.
*   A UVector/ArrayList of tokens tailored to the reset anchor, terminated with
    a sentinel value.
*   A UnicodeSet of those code points and strings. No expansion strings. If
    there is a prefix, store prefix+\\uFFFF+contraction. TODO: Include the reset
    anchor itself in the set?
*   The CE or CEs for the reset anchor.
    *   TODO: This assumes that we lookup a reset anchor immediately. Otherwise
        we will have to be able to store special reset tokens like \[last
        tertiary ignorable\]
    *   TODO: Count how many resets occur in CLDR tailorings: Max & average. If
        there are many, then we might want to minimize reset position storage,
        for example by storing only one CE, with a special value (high byte FF)
        for length & index into a larger array.
    *   TODO: This is not an issue if we store a special token rather than
        CEs...

Token: One int32_t each:

*   28 bits (31..4) code point or string index
    *   code point 0..0x10ffff
    *   if we need special values, start those at 0x110000
    *   string index is stored as a negative value (~index)
        *   index into a shared UnicodeString/StringBuilder
        *   first unit: high byte=prefix length, low byte=contraction length
        *   followed by prefix+contraction
        *   if there is an expansion string (see bit 3), it follows after the
            contraction, with another first unit with its lengths
*   1 bit (3) set if there is an expansion string after the contraction. Future
    syntax might result in the expansion string having a prefix as well.
*   3 bits (2..0) relation -- 0=, 6=reset-before, 7=reset-at, 1=primary, ...,
    4=quaternary, 5=identical
    *   0=NO_RELATION=sentinel
    *   1=reset-before
        *   The parser checks that the before-strength matches the first
            relation after the reset.
        *   TODO: Make the before-strength optional in the syntax.
        *   \[before 4\] (quaternary) is not allowed in the v2 data structure
    *   2=reset-at
        *   The resets have lower values than the real relations so that when we
            "postpone insertion" we can always stop before a token with a
            relation less-than-or-equal the new one.
    *   3=primary
    *   4=secondary
    *   5=tertiary
    *   6=quaternary
    *   7=identical

No range token type because it complicates finding tokens and merging token
lists. -> Expand <\*a-e into <a<b<c<d<e.

Issue: It should be possible to reset-before and reset-at the same position. We
should support disjoint token lists for both such resets.

TODO: If a and b are primary-adjacent, then we should merge the token lists for
&a<p<q and &\[before\]b<x<y.

TODO: We should merge the token lists for &\[before\]a<p and &\[before\]p<<s
etc.

TODO: Maybe instead of such complicated merging of token lists we could remember
the ranges of root element weights and levels that we tailor between, and check
for overlaps?

Maybe: Parser: For each reset position, parse it and its tailoring list. Then
merge the new data into the existing list of resets. If the reset anchor itself
was already tailored, then merge the new list into the old one. Otherwise, if a
newly tailored character was already tailored before, then remove it from the
old list; and add the new reset anchor to the reset list.

Parse up to the end of the rules string, or the next reset, or the next
\[import\]. For \[import\], try to switch to the imported rules rather than
modifying the rules string?

TODO: Have builder optionally take an object that supplies character properties
(case, digit value, etc.) so that genuca could be merged into genprops and use
data without rebuilding.

TODO: Cache some more FCD values in UCA data file? \[try to measure performance
first\] e.g., for U+0000..U+07FF or ..U+0FFF -- the advantage over storing in
the .nrm file is that ucadata is already large, and we will by design build this
only for NFC data, and FCD is more commonly used with collation than elsewhere.
If we have multiple compact-ish base collators, better to store a linearfcd
"string" on the top level of the coll/root.res (if it does contain any
collators).

TODO: Data for characters with ccc!=lccc, tccc? In .nrm or in ucadata?

## getTailoredSet

Collator has getTailoredSet(), and UCollator has ucol_getTailoredSet(),
ucol_getContractions() and ucol_getContractionsAndExpansions().

*   They all required the presence of the rule string, and parse that and
    re-engineer what is tailored.
*   TODO: This is bad... <http://bugs.icu-project.org/trac/ticket/9594>

ucol_getContractionsAndExpansions() has `@param addPrefixes add the prefix
contextual elements to contractions`.

*   TODO: Find out how prefixes are added, how they are distinguished from
    contractions, and how input is represented that has both a prefix and a
    contraction.

TODO: C++ Collator does not have any API to find out expansions (not sure that's
useful) or prefixes (needed to find all tailorings).

TODO: There should be API to return all inputs, and it should have have an
option for whether to include the inputs from the root collator.

## Testing

Write a data-driven test so that new test cases are easy to set up and
understand.

Done: Do not use "ducet" to refer to the CLDR root/standard collator. Use "und"
or "root".

TODO: Look at the 2012-may unicore list thread "Sterner Collation Tests".

TODO: Design and publish as part of UCA new test data that tests only Main
Algorithm Step 2 "Produce Array", like in the v2 tests.

## More notes

TODO: integrate above

export LD_LIBRARY_PATH=../../lib:../../stubdata:../../tools/ctestfw

make && ./intltest collate/CollationTest collate/UCAConformanceTest

./intltest collate/CollationAPITest/TestRuleBasedColl

(apicoll.cpp:296)

Find out where canonical closure happens.

1. ucol_elm.cpp uprv_uca_addAnElement()

- uses the CanonicalIterator for contractions

+ which misses singleton decompositions

- adds all canonical equivalences that are FCD

2. ucol_res.cpp ucol_getTailoredSet()

// The idea is to tokenize the rule set. For each non-reset token,

// we add all the canonicaly equivalent FCD sequences

- rather than iterating over the actual data structures

3. ucol_elm.cpp uprv_uca_canonicalClosure()

- called from near the end of ucol_bld.cpp ucol_assembleTailoringTable(),

just before uprv_uca_assembleTable()

- calls u_enumCharTypes(_enumCategoryRangeClosureCategory, &context);

with a clone of the input partially-built collator

+ takes every character with decomposition,

if c & decomp sort differently adds c->CEs(decomp) via uprv_uca_addAnElement()

(maps to intermediate prefix special CE if the decomp yields that)

- calls uprv_uca_addTailCanonicalClosures() for each item from a UColTokenParser

- uprv_uca_addTailCanonicalClosures() takes a base character and a combining
mark

+ each only a UChar! (and checks pairwise composition via len==1)

+ appears to find all composites with base + lower-cc combining mark

+ appears to add closure for a+n so that a+m+n (and maybe a+n+p?) gets
appropriate weights

+ TODO: try to understand what this does and how

TODO: test canonical closure with supplementary base characters & combining
marks,

make it work with v2

TODO: do we really need 3 algorithms for canonical closure?

(CanonicalIterator in addAnElement,

iterating over composites with _enumCategoryRangeClosureCategory,

iterating over base+combining marks)

-- 2012jun --

Can we forbid secondary ignorables?

-Then we don't need a gap, can use more values in DUCET, tailor up to 3F.

-simplifies tailoring code slightly

-all levels after primary have same length

-would not need to check if secondary is 0 in STBuffer

Even now need not check if tertiary is 0 - if we stop storing completely ign. in
STBuffer, no shifted either in v2

-note: Oracle uses secondary ignorables for punctuation & spaces, rather than
shifting them to quaternary:
http://docs.oracle.com/cd/E11882_01/server.112/e10729/ch5lingsort.htm#i1006286
"Oracle® Database Globalization Support Guide

11g Release 2 (11.2)

Part Number E10729-07" - "5 Linguistic Sorting and String Searching"

TODO: Richard Wordingham pointed out in July 2012 that our constructing case
weights (and prepending them in a case level, or prepending to the tertiary
weights) violates the tertiary-weight well-formedness condition. Did he submit a
CLDR ticket for that? I submitted <http://bugs.icu-project.org/trac/ticket/9439>
"fix case weights for tertiary CEs".

TODO: See if any CLDR tailoring yields tertiary CEs. It would have to tailor a
tertiary difference from a completely ignorable character.
<http://unicode.org/cldr/trac/ticket/5093> "consider tertiary CEs for some
characters"

Secondary weights in Korean

*   Korean tailors Han characters as chains of secondary differences from Hangul
    syllables.
*   The longest chains are longer than 100.
*   Most chains appear to be shorter than 32. We should be able to handle them
    with single-byte secondaries.

Use two-byte sec/ter gap boundaries to avoid lead byte waste

HiraganaQ sort key compression

- maybe fixed range for compression, eg 2c..94..fc,

and if primary weight too high (2b & higher) then in sort key

prepend a fixed lead byte (2b) just below the common-compression range

Done: Note if any variable CEs in primary code

Question: If we have &v<<w and &V<<W, what is the relationship of w and W? Will
they be only tertiary different? Actually, they should probably go into the same
tailoring chain and become secondary different.

Case level why not compress like secondary?

-nibble wise so one uppercase does not cost a whole byte

-lowerFirst 1..7..D lower E mixed F upper

-upperFirst 1 upper 2 mixed 3..F lower

-Markus Scherer F6 F6

-added to http://bugs.icu-project.org/trac/ticket/9355 "collation: primary+case
ignores primary ignorables"

Case bits:

*   &x=y/z -- v1 sets the case weight of the first CE to the combined case value
    of y and copies the CEs of z verbatim, including their case bits. Does not
    reflect well how y should sort.
*   We cannot just copy the case bits of the reset anchor (x) because we have
    tailorings like &V<<w<<<W where w would get V's uppercase bits.

Case bits: We should use the DUCET decomposition mappings to distribute
uppercase bits over the CE sequence. Issue: What if we do not have the usual 1:1
relationship of decomposition characters : CEs?

*   Too few characters: Set remaining CEs to be uncased.
*   Too many characters: For N characters and M CEs with N>M, set the case bits
    for the first M-1 CEs 1:1. Combine case values of characters M..N into one
    value, using mixed if there are both lowercase and uppercase characters, set
    the combined value on the last CE.
*   Maybe try to only set real case values on primary CEs. Or mixed/upper only
    on primary CEs?
*   Usually primary ignorables are caseless. (Except U+0345 is Lowercase.)
*   Probably skip tertiary CEs when distributing case weights, since we do not
    support case distinctions on tertiary CEs.

Case weight=mixed: We would rarely use it if we distribute case bits over
expansions. If we removed it, we would not be able to make as many primary+case
distinctions for true contractions like ch vs. Ch vs. CH.

How many tertiary values do we need?

*   Current DUCET tertiaries fit into single 6-bit bytes.
*   See if tailorings generate two-byte tertiaries.
*   Do tailorings have tertiary differences from root CEs (a<<<x) which might
    have non-common tertiaries, or mostly from newly constructed CEs (a<b<<<x or
    a<<b<<<x) which have the whole tertiary range available?
*   Look at how long chains of tertiary differences are. (How many items do we
    tailor between two CEs, especially between two root CEs?)
*   We might use tertiary CEs for some characters (see
    <http://unicode.org/cldr/trac/ticket/5093>), which would need higher
    tertiary weights. Are two-byte weights acceptable for these, or should we
    use single-byte tertiary weights for some of them?

If we get enough two-byte tertiaries to worry about:

*   We could try to use more tertiary lead byte bits. Not promising. See the
    constraints below.
*   Probably simpler: Find out how many tertiary weights we use per
    primary+secondary combination. Spread them out over the available range.
    Consider extra-large gaps after tertiary weights where we tend to add
    tailorings, that is, after the common weight and maybe after the uppercase
    weight.
    *   If there are too many for all single-byters, overflow rarely occurring
        ones to two bytes.

Constraints on case bits:

*   Case bits need to use higher bits than the tertiary weight so that
    caseFirst!=off works right with a mask.
*   Case bits + tertiary lead byte together should fit into 8 bits so that they
    fit into one sort key byte for caseFirst!=off.
*   We could remove "mixed" and use 7 tertiary lead byte bits, but would lose
    the extra 64 byte values for tertiary compression and would have to reserve
    space inside the 7-bit range for that.
*   We could use up to 84 tertiary lead byte values, but that would complicate
    processing (div/mod or tables, rather than masking) and we would also have
    to reserve space inside the larger range for compression.

CollationElementIterator: ICU4C & JDK do not document the bit fields; ICU4J
does.

Note: implicitly support 4-level CEs in collationtest.txt -- just treat them as
tertiary bits in the syntax, like case bits

Add API to set variableTop to space, punct, symbols:
http://bugs.icu-project.org/trac/ticket/8032

Consider returning extended set of coll result values when new attribute is set;
makes testing more precise. Values easy to turn into simple ones, internal code
always returns extended values. May not be useful except for testing. Workaround
is to inspect in which level difference occurs in sort keys.

-> http://bugs.icu-project.org/trac/ticket/2737

TODO: CODAN: Support at most 254 digits, or support thousands? millions? See the
v1 ticket that limited it to 254: <http://bugs.icu-project.org/trac/ticket/4121>

Consider API for "equals" that compares CE bits in one pass. May not be useful
outside string search.

fcd16_F00:

*   It might suffice to have the fastpath test
    singleLeadMightHaveNonZeroFCD16(c), maybe preceded by "if(c <= 0x7f)". This
    should be good for Latin & CJK. We might not need to optimize further, and
    could drop fcd16_F00. Measure!
*   If we keep fcd16_F00, it would probably be cleaner to move it out of
    CollationData. We could even hardcode it in the .cpp file that implements
    the incremental FCD check. Maybe have preparseucd.py generate a .txt file
    with initializers.
*   Also try to move nfcImpl out of CollationData, into Collator. Pass into
    FCD-checking-iterator constructor.

Performance:

- Measure with normalization off!

+ Temporarily turning normalization off (and disabling ordering checking) in
ucaconf.cpp shows:

TestTableNonIgnorable {

} OK: TestTableNonIgnorable (66ms)

TestTableShifted {

} OK: TestTableShifted (104ms)

TestTable2NonIgnorable {

} OK: TestTable2NonIgnorable (67ms)

TestTable2Shifted {

} OK: TestTable2Shifted (85ms)

-> this is still with the 32-bit sec/ter buffer and separate quaternary
iteration

TestTableNonIgnorable {

} OK: TestTableNonIgnorable (66ms)

TestTableShifted {

} OK: TestTableShifted (103ms)

TestTable2NonIgnorable {

} OK: TestTable2NonIgnorable (66ms)

TestTable2Shifted {

} OK: TestTable2Shifted (80ms)

-> this is with the 64-bit CEBuffer and single iteration

- Do long-secondary CEs hurt in the fast path?

- Are long-primary CEs better as a special??

- Using different code paths inside compare() for variableTop==0 vs. !=0, like
in v1,

seems to not speed up the UCA conformance test.

Turning the Latin-1 fastpath off in v1 does make that some 20% slower.

Latin-1 special compare():

- really worth it?

+ v1 restrictions (ucol.cpp ucol_updateInternalState()):

if(coll->caseLevel == UCOL_OFF && coll->strength <= UCOL_TERTIARY &&
coll->numericCollation == UCOL_OFF

&& coll->alternateHandling == UCOL_NON_IGNORABLE && !coll->latinOneFailed)

+ "shifted" should be easy with <=tertiary: just set sort key bytes to zero

- try less memory; 32 bits rather than 96?

+ extend to U+017F? still half the memory

+ single CEs have at most two primary weight bytes

+ most expansions of at most two CEs

+ few exceptions (vulgar fractions & letter ae), bail out for those

+ actually...: 00DF; \[4B, 05, 89\]\[, DB A9, 89\]\[4B, 05, BD\] # ... \* LATIN
SMALL LETTER SHARP S

-> expansion specials?

+ primary weight lead byte <= 5A, letters >= 26

+ however, reordering might yield higher lead byte values

~ we could restrict reordering to at most Latin, at most <=5A or <=7F

+ ASCII and <=U+009F limited to single CEs, max 2-byte primaries, 05
secondaries, 05 or 8F (uppercase) tertiaries;

plus contractions for some languages;

simple 16-bit entries??

+ one common secondary, or one common + one other byte, or just that other byte

+ tertiary could be common or other+common

+ maybe basically use lookup CE32s including Latin expansion specials,

but with larger Latin-expansion-tags range and fewer other specials

+ class CollationLatin?

-- 20120706 --

DUCET size without trying to compress primary-code-point-order ranges:

\*\*\* DUCET part sizes \*\*\*

trie size: 113728

CE32s: 6051 \*4 = 24204

CEs: 1884 \*8 = 15072

contexts: 3473 \*2 = 6946

Jamo CEs: 67 \*8 = 536

fcd16_F00: 3840 \*2 = 7680

compressibleBytes: 256

unsafeBwdSet: 364 \*2 = 728

\*\*\* DUCET size: 169150

missing: headers, options, reordering group data

DUCET size with trying to compress primary-code-point-order ranges:

\* set code point order range U+21d5..U+2202 \[46\] 0d5ca800..0d5e1c00 step 8
computed-next primary 0d5e1c00

\* set code point order range U+22ee..U+2328 \[59\] 0d7af200..0d7cce00 step 8
computed-next primary 0d7cce00

\* set code point order range U+232b..U+23f3 \[201\] 0d7cce00..0d834000 step 8
computed-next primary 0d832400

\* set code point order range U+2503..U+25a0 \[158\] 0d84f600..0d89fc00 step 8
computed-next primary 0d89f000

\* set code point order range U+25a1..U+25cb \[43\] 0d8a0a00..0d8b6000 step 8
computed-next primary 0d8b6400

\* set code point order range U+25cc..U+262f \[100\] 0d8b6800..0d8e9200 step 8
computed-next primary 0d8e8e00

\* set code point order range U+2638..U+266c \[53\] 0d8e8e00..0d904600 step 8
computed-next primary 0d903a00

\* set code point order range U+2690..U+26ff \[112\] 0d912400..0d94ae00 step 8
computed-next primary 0d94aa00

\* set code point order range U+2701..U+2767 \[103\] 0d94aa00..0d97f800 step 8
computed-next primary 0d97e800

\* set code point order range U+2794..U+27c4 \[49\] 0d97e800..0d998c00 step 8
computed-next primary 0d997400

\* set code point order range U+2900..U+2982 \[131\] 0d9b1800..0d9f4000 step 8
computed-next primary 0d9f3800

\* set code point order range U+2999..U+29d7 \[63\] 0d9f3800..0da14400 step 8
computed-next primary 0da13400

\* set code point order range U+2a0d..U+2a73 \[103\] 0da2c200..0da60a00 step 8
computed-next primary 0da60200

\* set code point order range U+2a77..U+2adb \[101\] 0da60200..0da94400 step 8
computed-next primary 0da93000

\* set code point order range U+2ade..U+2b4c \[111\] 0da95400..0dacd600 step 8
computed-next primary 0dacd200

\* set code point order range U+2800..U+28ff \[256\] 0dad6400..0db58c00 step 8
computed-next primary 0db57400

\* set code point order range U+4dc0..U+4dff \[64\] 0db60a00..0db80e00 step 8
computed-next primary 0db80e00

\* set code point order range U+1d300..U+1d356 \[87\] 0db80e00..0dbad200 step 8
computed-next primary 0dbaca00

\* set code point order range U+a490..U+a4c6 \[55\] 0dbaca00..0dbc9600 step 8
computed-next primary 0dbc8600

\* set code point order range U+101d0..U+101fc \[45\] 0dbdd400..0dbf4000 step 8
computed-next primary 0dbf4000

\* set code point order range U+1d000..U+1d0f5 \[246\] 0dbf4000..0dc72000 step 8
computed-next primary 0dc6fe00

\* set code point order range U+1d200..U+1d241 \[66\] 0dcca600..0dceba00 step 8
computed-next primary 0dceba00

\* set code point order range U+1f000..U+1f02b \[44\] 0dceca00..0dd02e00 step 8
computed-next primary 0dd02e00

\* set code point order range U+1f030..U+1f093 \[100\] 0dd02e00..0dd36000 step 8
computed-next primary 0dd35400

\* set code point order range U+1f337..U+1f37c \[70\] 0dd68a00..0dd8be00 step 8
computed-next primary 0dd8be00

\* set code point order range U+1f400..U+1f43e \[63\] 0ddb5000..0ddd4c00 step 8
computed-next primary 0ddd4c00

\* set code point order range U+1f442..U+1f4f7 \[182\] 0ddd5c00..0de32800 step 8
computed-next primary 0de31800

\* set code point order range U+1f500..U+1f53d \[62\] 0de35000..0de54400 step 8
computed-next primary 0de54400

\* set code point order range U+1f5fb..U+1f640 \[70\] 0de63200..0de86600 step 8
computed-next primary 0de86600

\* set code point order range U+1f680..U+1f6c5 \[70\] 0de8c600..0deafa00 step 8
computed-next primary 0deafa00

\* set code point order range U+1f700..U+1f773 \[116\] 0deafa00..0deeb200 step 8
computed-next primary 0deea200

\* set code point order range U+a882..U+a8c4 \[67\] 71a74600..71a96200 step 8
computed-next primary 71a96200

\* set code point order range U+11183..U+111c3 \[65\] 71ab1e00..71ad2a00 step 8
computed-next primary 71ad2a00

\* set code point order range U+11005..U+11033 \[47\] 71b06c00..71b1e200 step 8
computed-next primary 71b1e600

\* set code point order range U+11103..U+1112e \[44\] 763f0400..76406200 step 8
computed-next primary 76406600

\* set code point order range U+aa00..U+aa28 \[41\] 779bd200..779d1e00 step 8
computed-next primary 779d1e00

\* set code point order range U+13a0..U+13f4 \[85\] 77a78200..77aa3400 step 8
computed-next primary 77aa3000

\* set code point order range U+1401..U+157b \[379\] 77aa3000..77b65400 step 8
computed-next primary 77b62000

\* set code point order range U+15a7..U+166c \[198\] 77b7fa00..77be4c00 step 8
computed-next primary 77be3800

\* set code point order range U+18b0..U+18f5 \[70\] 77be9c00..77c0d000 step 8
computed-next primary 77c0d000

\* set code point order range U+a588..U+a5b4 \[45\] 77c8b000..77ca1c00 step 8
computed-next primary 77ca1c00

\* set code point order range U+a5d9..U+a60c \[52\] 77cb4a00..77cce800 step 8
computed-next primary 77ccec00

\* set code point order range U+a6a0..U+a6ef \[80\] 77ccec00..77cf7a00 step 8
computed-next primary 77cf7200

\* set code point order range U+16800..U+16a38 \[569\] 77cf7200..77e1ae00 step 8
computed-next primary 77e15e00

\* set code point order range U+d7cb..U+d7fb \[49\] 7898f200..789a7e00 step 8
computed-next primary 789a7e00

\* set code point order range U+a000..U+a48c \[1165\] 7a700400..7a954200 step 8
computed-next primary 7a94b400

\* set code point order range U+16f50..U+16f7e \[47\] 7a98d400..7a9a5000 step 8
computed-next primary 7a9a5000

\* set code point order range U+102a0..U+102d0 \[49\] 7a9bce00..7a9d5a00 step 8
computed-next primary 7a9d5a00

\* set code point order range U+10450..U+1049d \[78\] 7aa15a00..7aa3ce00 step 8
computed-next primary 7aa3ce00

\* set code point order range U+10080..U+100fa \[123\] 7aa77600..7aab5e00 step 8
computed-next primary 7aab5600

\* set code point order range U+1080a..U+10835 \[44\] 7aab9e00..7aacfc00 step 8
computed-next primary 7aad0200

\* set code point order range U+10b00..U+10b2d \[46\] 7aae1800..7aaf8600 step 8
computed-next primary 7aaf8a00

\* set code point order range U+12000..U+1236e \[879\] 7ab42c00..7ad04400 step 8
computed-next primary 7acfda00

\* set code point order range U+13000..U+1342e \[1071\] 7acfda00..7af28800 step
8 computed-next primary 7af19600

\*\* set 54 ranges with 8322 code points

\*\*\* DUCET part sizes \*\*\*

trie size: 91520

CE32s: 6051 \*4 = 24204

CEs: 1938 \*8 = 15504

contexts: 3473 \*2 = 6946

Jamo CEs: 67 \*8 = 536

fcd16_F00: 3840 \*2 = 7680

compressibleBytes: 256

unsafeBwdSet: 364 \*2 = 728

\*\*\* DUCET size: 147374

missing: headers, options, reordering group data

-> "195 test failures"

\* set code point order range U+1113..U+1160 \[78\] 78300400..78336600 step 8
computed-last primary 78327000

\* set code point order range U+1176..U+11a7 \[50\] 785e0400..785f9200 step 8
computed-last primary 785f8e00

\* set code point order range U+11c3..U+11ff \[61\] 78970400..7898ea00 step 8
computed-last primary 7898e600

\* set code point order range U+13a0..U+13f4 \[85\] 77a78200..77aa3400 step 8
computed-last primary 77aa2800

\* set code point order range U+1400..U+157c \[381\] 064a0400..77b7f200 step 8
computed-last primary 0655fa00

\* set code point order range U+157d..U+166c \[240\] 77b65c00..77be4c00 step 8
computed-last primary 77bde200

\* set code point order range U+18b0..U+18f5 \[70\] 77be9c00..77c0d000 step 8
computed-last primary 77c0c800

\* set code point order range U+1980..U+19ab \[44\] 77972200..77988000 step 8
computed-last primary 77987c00

\* set code point order range U+19e0..U+1a1b \[60\] 0d53c000..74d28400 step 8
computed-last primary 0d559c00

\* set code point order range U+1a1e..U+1a52 \[53\] 0a6d0400..779b3a00 step 8
computed-last primary 0a6ea600

\* set code point order range U+1bfc..U+1c23 \[40\] 0c7fac00..74cb4a00 step 8
computed-last primary 0c80e600

\* set code point order range U+21d0..U+2203 \[52\] 0d5c8000..0d5e2400 step 8
computed-last primary 0d5e1c00

\* set code point order range U+22b0..U+22df \[48\] 0d793400..0d7aba00 step 8
computed-last primary 0d7aae00

\* set code point order range U+22ee..U+2328 \[59\] 0d7af200..0d7cce00 step 8
computed-last primary 0d7cc600

\* set code point order range U+232b..U+23f3 \[201\] 0d7cd600..0d834000 step 8
computed-last primary 0d832400

\* set code point order range U+2500..U+2637 \[312\] 0d84de00..0db5fc00 step 8
computed-last primary 0d8eaa00

\* set code point order range U+2638..U+266f \[56\] 0d8e9a00..0dc87600 step 8
computed-last primary 0d905600

\* set code point order range U+2690..U+26ff \[112\] 0d912400..0d94ae00 step 8
computed-last primary 0d94a200

\* set code point order range U+2701..U+2767 \[103\] 0d94b600..0d97f800 step 8
computed-last primary 0d97ec00

\* set code point order range U+2794..U+27c4 \[49\] 0d980600..0d998c00 step 8
computed-last primary 0d998800

\* set code point order range U+27f0..U+28ff \[272\] 0d9a9200..0db58c00 step 8
computed-last primary 0da31c00

\* set code point order range U+2900..U+2982 \[131\] 0d9b1800..0d9f4000 step 8
computed-last primary 0d9f3000

\* set code point order range U+2999..U+29d7 \[63\] 0d9f4800..0da14400 step 8
computed-last primary 0da13c00

\* set code point order range U+2a0d..U+2a73 \[103\] 0da2c200..0da60a00 step 8
computed-last primary 0da5f800

\* set code point order range U+2a77..U+2adb \[101\] 0da61200..0da94400 step 8
computed-last primary 0da93800

\* set code point order range U+2add..U+2b4c \[112\] 0da94c00..0dacd600 step 8
computed-last primary 0dacca00

\* set code point order range U+2c30..U+2c5e \[47\] 5d6b0400..5d6c7a00 step 8
computed-last primary 5d6c7600

\* set code point order range U+2d30..U+2d65 \[54\] 636f0a00..6370c800 step 8
computed-last primary 6370b400

\* set code point order range U+4dc0..U+4dff \[64\] 0db60a00..0db80e00 step 8
computed-last primary 0db80600

\* set code point order range U+a000..U+a48c \[1165\] 7a700400..7a954200 step 8
computed-last primary 7a94ac00

\* set code point order range U+a490..U+a4c6 \[55\] 0dbada00..0dbc9600 step 8
computed-last primary 0dbc8e00

\* set code point order range U+a4fe..U+a60c \[271\] 08510c00..77cce800 step 8
computed-last primary 08598c00

\* set code point order range U+a6a0..U+a6ef \[80\] 77ccf000..77cf7a00 step 8
computed-last primary 77cf6e00

\* set code point order range U+a882..U+a8c4 \[67\] 71a74600..71a96200 step 8
computed-last primary 71a95a00

\* set code point order range U+a984..U+a9ab \[40\] 77a04800..77a18e00 step 8
computed-last primary 77a18200

\* set code point order range U+aa00..U+aa32 \[51\] 779bd200..779d8e00 step 8
computed-last primary 779d6600

\* set code point order range U+aa80..U+aab4 \[53\] 736a0400..736baa00 step 8
computed-last primary 736ba600

\* set code point order range U+d7cb..U+d7fb \[49\] 7898f200..789a7e00 step 8
computed-last primary 789a7600

\* set code point order range U+10080..U+100fa \[123\] 7aa77600..7aab5e00 step 8
computed-last primary 7aab4e00

\* set code point order range U+101d0..U+101fc \[45\] 0dbdd400..0dbf4000 step 8
computed-last primary 0dbf3800

\* set code point order range U+102a0..U+102d0 \[49\] 7a9bce00..7a9d5a00 step 8
computed-last primary 7a9d5200

\* set code point order range U+10428..U+1049d \[118\] 7aa01400..7aa3ce00 step 8
computed-last primary 7aa3c200

\* set code point order range U+1080a..U+10835 \[44\] 7aab9e00..7aacfc00 step 8
computed-last primary 7aacf800

\* set code point order range U+10b00..U+10b2d \[46\] 7aae1800..7aaf8600 step 8
computed-last primary 7aaf8200

\* set code point order range U+11005..U+11046 \[66\] 71b06c00..71b29000 step 8
computed-last primary 71b27800

\* set code point order range U+11103..U+11130 \[46\] 763f0400..76407200 step 8
computed-last primary 76406e00

\* set code point order range U+11183..U+111c3 \[65\] 71ab1e00..71ad2a00 step 8
computed-last primary 71ad2200

\* set code point order range U+12000..U+1236e \[879\] 7ab42c00..7ad04400 step 8
computed-last primary 7acfd200

\* set code point order range U+13000..U+1342e \[1071\] 7ad04c00..7af28800 step
8 computed-last primary 7af1fe00

\* set code point order range U+16800..U+16a38 \[569\] 77cf8200..77e1ae00 step 8
computed-last primary 77e16600

\* set code point order range U+16f50..U+16f7e \[47\] 7a98d400..7a9a5000 step 8
computed-last primary 7a9a4800

\* set code point order range U+1d000..U+1d0f5 \[246\] 0dbf4800..0dc72000 step 8
computed-last primary 0dc6fe00

\* set code point order range U+1d12a..U+1d15d \[52\] 0dc87e00..0dca2a00 step 8
computed-last primary 0dca1a00

\* set code point order range U+1d200..U+1d241 \[66\] 0dcca600..0dceba00 step 8
computed-last primary 0dceb200

\* set code point order range U+1d300..U+1d356 \[87\] 0db81600..0dbad200 step 8
computed-last primary 0dbaca00

\* set code point order range U+1f000..U+1f02b \[44\] 0dceca00..0dd02e00 step 8
computed-last primary 0dd02600

\* set code point order range U+1f030..U+1f093 \[100\] 0dd03600..0dd36000 step 8
computed-last primary 0dd35400

\* set code point order range U+1f337..U+1f37c \[70\] 0dd68a00..0dd8be00 step 8
computed-last primary 0dd8b600

\* set code point order range U+1f400..U+1f43e \[63\] 0ddb5000..0ddd4c00 step 8
computed-last primary 0ddd4400

\* set code point order range U+1f442..U+1f4f7 \[182\] 0ddd5c00..0de32800 step 8
computed-last primary 0de31000

\* set code point order range U+1f500..U+1f53d \[62\] 0de35000..0de54400 step 8
computed-last primary 0de53c00

\* set code point order range U+1f5fb..U+1f640 \[70\] 0de63200..0de86600 step 8
computed-last primary 0de85e00

\* set code point order range U+1f680..U+1f6c5 \[70\] 0de8c600..0deafa00 step 8
computed-last primary 0deaf200

\* set code point order range U+1f700..U+1f773 \[116\] 0deb0800..0deeb200 step 8
computed-last primary 0deea600

\*\* set 64 ranges with 9328 code points

\*\*\* DUCET part sizes \*\*\*

trie size: 87408

CE32s: 6051 \*4 = 24204

CEs: 1948 \*8 = 15584

contexts: 3473 \*2 = 6946

Jamo CEs: 67 \*8 = 536

fcd16_F00: 3840 \*2 = 7680

compressibleBytes: 256

unsafeBwdSet: 364 \*2 = 728

\*\*\* DUCET size: 143342

missing: headers, options, reordering group data

-> "5605 test failures"

\* collation demo

custom tailoring: print tailoring CEs

sorting output: print CEs, show which part of string for which CEs

make && ./intltest collate/UCAConformanceTest/TestTable2NonIgnorable

make && ./intltest collate/CollationTest
collate/UCAConformanceTest/TestTable2NonIgnorable
