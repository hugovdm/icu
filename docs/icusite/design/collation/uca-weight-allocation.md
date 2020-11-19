# UCA Weight Allocation

[TOC]

We have been reviewing the primary weight allocation that we use for the UCA.
This is sparked by two issues: (1) we need to do some work for Script Reordering
anyway, and (2) the allocation of weights has not kept up with the growth of
Unicode, causing a degradation in performance and sortkey size.

**Background**

*   Primary weights can be 1 to 4 bytes in length. (If a character has a 3-byte
    CE primary weight, we'll call it a 3-byter, for example)
*   They have the restriction that no weight can be a proper initial sequence of
    another. Eg, if X has weight 85 42, then no three or four byte weight can
    start with 85 42.
*   It is important that the most frequent characters have short weights. So,
    for example, in U5.2 we have {space, A-Z} being 1-byters.
*   The first bytes are important, and are allocated to special ranges: see
    [Special Byte Values](bytes.md)
*   If scripts have different first bytes, they can be reordered with respect to
    one another. We don't have enough room in the first-byte space to do this
    for all scripts, however.
*   All of the characters that share the same first primary can use a special
    primary compression for shorter sort keys. That means that if a script is
    split across different first bytes, it doesn't benefit from compression, and
    can even get a bit longer.
*   When we generate the base table, we put in gaps to allow tailored characters
    to go in. We try to make the gaps larger where we anticipate there being
    more need.

The particular problems we are are:

*   We only have 17 available first bytes for stuffing in up to 21K tailored CJK
    weights for Chinese, which causes over half of them to be 3 byters. In order
    to fix this, we need to have about 84 first bytes vacant (67 more).
*   The primary-key compression is using old (hard-coded) ranges, causing it to
    be suboptimal *(including making keys longer than if the compression weren't
    there).*

See also:

*   [ICU Fractional UCA
    Crisis](https://docs.google.com/Doc?docid=0AQ3w_MjvUEoRZGRzcnJwajVfMTRkOHB3bXNnZA&hl=en)
*   [Special Byte Values](bytes.md) (mostly replaces the outdated [Magic
    Bytes](http://icu-project.org/repos/icu/icuhtml/trunk/design/collation/ICU_collation_design.htm#Magic_Bytes))
*   [Script-Reordering-Chart](https://spreadsheets.google.com/ccc?key=0AqRLrRqNEKv-dG0taVI1UlR4Y2VaNU1oTDhYa2lzcmc&hl=en#gid=0)

Here is a rough plan for what to do.

## Primary Weights

*   allow 03 as real primary 1st byte or 3rd/4th bytes
*   allow FF as real 3rd/4th

Emit data for first bytes that shouldn’t be compressed. That is, instead of the
primary compression ranges being hard-coded, they will be read out of data
incorporated into the Fractional UCA table.

For script reordering and tailoring to work together, when a character is
tailored before the first character in a script, or after the last character in
a script, the first byte for that character's CE *must* still be in the script.

*   The way we are thinking about doing this is having a fake primary weight,
    not assigned to any character, at the beginning of each script's primary
    lead byte. This weight needs to be listed in the invuca.icu file. The effect
    is that tailoring primary-before the first character of a script results in
    weights with the script's lead byte. This is necessary only for a script
    that starts at the beginning of a lead byte's weight range, not for
    subsequent lead bytes with more weights in the same script, nor for
    subsequent scripts in the same lead byte. The fake low weight can be 03 in
    the second byte. We don't need a fake high weight, because the fake low
    weight for the next higher byte works as an upper bound.

### Variables

*   Remove the 1-byte tailoring gap below TAB (start at 03 20 (or something),
    allowing a gap in 2-byte space)
*   Put space at 04
*   Use 3 byte weights for all/most symbols, maybe some punct too
*   Should result ~10 byte improvement.

### Non-Variables

Assign first bytes as follows:

*   gap
*   gorp symbols
*   sc
*   nd
*   gap
*   latn
*   ...

This is mostly for script reordering - it might cost a byte or two.

For the remaining scripts (see
[Script-Reordering-Chart](https://spreadsheets.google.com/ccc?key=0AqRLrRqNEKv-dG0taVI1UlR4Y2VaNU1oTDhYa2lzcmc&hl=en#gid=0))

*   Non-Recommended scripts
    *   Make all of these 3 bytes.
    *   They are separated from the recommended scripts, but adjacent
        Non-Recommended scripts have the same first byte.
    *   Expected savings: ~45 first bytes
    *   We could merge into the end of the previous byte, saving another ~5
        bytes, but not allowing them to be reordered different from that
        previous byte.
*   For Recommended Scripts
    *   Cyrillic, Arabic, Hangul\* get special treatment. Common characters are
        2 bytes, unusual characters are 3.
        *   "Common" for Hangul is any Jamo that can be in the decomposition of
            a Hangul Syllable
        *   "Common" for Cyrillic and Arabic is any character in the main
            exemplars of some CLDR locale
    *   Other Recommended that are more than 1 byte get 3 byte forms: Ethiopic,
        Myanmar, Mongolian, Cans, Vai, Yii
    *   A test in Fractional UCA will alert if any other script grows beyond 1
        first byte

\* Any sequence of characters normal has a gap between each, in 2-byte space. So
if we have <C U C>, that turns into <C g U g U g C> (where C = Common, U =
uncommon, g = gap). When the uncommon characters are turned into 3-byter, then
we don't need a two-byter gap. So we will get instead <C g UU C>. There will be
a 3byte gap between the U's, and between the last U and the first C. This
assumes that we rarely insert characters before others, because such a character
would turn into a three byter.

### Implicits & Specials

Break

*   han
*   non-han
*   Check that all non-Han are Cn
*   maybe 1 byte cost to breaking.

Trail range

*   We have never used this.
*   Make it shorter, 1 byte. Saving 11 first bytes.

Specials

*   Markus has ideas for compressing these, saving >10 first bytes.

### Details (from Markus)

ICU collation uses the last 32 lead bytes as fixed values:

*   E0..E4 implicit (algorithmically constructed) primary weights for Han
    characters and unassigned code points
*   E5..EF trail weights, unused in UCA; these were intended for tailoring Jamo
    V and Jamo T but such an ordering was never used
*   F0..FF special lead bytes for long-primary CEs, expansions, contractions,
    Hangul syllables, implicit-weight code points, etc.

We could easily squeeze this down to fewer lead bytes. For example, from 32 to
8:

*   F8..FC implicit weights; keep the algorithm, just change the offset from E0
    to F8
    *   If we want to have disjoint lead bytes for Han characters vs. unassigned
        code points, then we need to review the transition point. We might need
        to add a lead byte in that case.
*   FD trail weights
*   FE long-primary weights (need 24 bits of data for 3-byte primary weights)
*   FF specials
    *   Bits 23..4 are the 32-bit-unit offsets of data for expansions,
        contractions, and precontext
    *   Bits 3..0 indicate the type of special value

The following specials are currently defined but are unused and need not be
encoded at all: CJK_IMPLICIT_TAG, CHARSET_TAG, THAI_TAG.

For the LEAD_SURROGATE_TAG we currently need 10 bits of data, but if we change
to using UTrie2 at the same time \[or earlier\], then we don't need data for
that any more.

### Generation

The generator for FractionalUCA (WriteCollationData) currently has a dumb
algorithm for allocation. That is, given 20 characters in a script, it just
increments the weights by a fixed amount, leaving a big gap at the end. If we
wanted to, we could change the algorithm to spread the gap more evenly. Probably
a low priority.

### Testing

We need to test the tailoring of characters before the first of each
reorder-type (script, Nd, IMPLICIT, TRAIL-WEIGHT,...) and after the last of each
reorder-type, to make sure that they stay in the same reorder-type.
