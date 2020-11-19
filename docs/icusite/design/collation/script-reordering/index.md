# Script Reordering

We would like to add script reordering as a new collation setting. This will
allow, for example, sorting Greek before Latin, and digits after all letters,
without listing all affected characters in the rules. This can be implemented
via a permutation table for primary collation weights.

See the original (somewhat outdated) ICU collation design doc for reference:
[Script_Order](http://source.icu-project.org/repos/icu/icuhtml/trunk/design/collation/ICU_collation_design.htm#Script_Order).
See also [CLDR ticket
2267](http://unicode.org/cldr/trac/ticket/2267)/[3081](http://unicode.org/cldr/trac/ticket/3081)
and the [CLDR design doc for proposed collation
additions](https://sites.google.com/site/cldr/development/development-process/design-proposals/collation-additions).

For building the permutation table, we will need to map from a script code to a
range of lead bytes of collation elements, and reorder whole ranges. For many
scripts, there is only one lead byte. For Latn, there is a range (to optimize
Latin collation performance). For Hani, we should probably consider two ranges:
The "top" range after the \[last non-ignorable\] character, which is normally
used for CJK tailorings to provide 2-byte primary weights for thousands of
characters, as well as the immediately following range of CJK implicit primary
weights.

Below are the current UCA values when translated into a Fractional UCA table,
listed by lead bytes. For reference, also look at the [UCA
Charts](http://unicode.org/charts/collation/).

## Status

Fully implemented in ICU 4.8 with public API.

Was already mostly implemented in ICU 4.6 with incomplete "@internal" API and a
couple of implementation issues.

## Issues

1.  We need to have rule syntax and locale code syntax for the reordering, so
    that we can put them into the rules as defaults for a language.
2.  We have to make sure that when you tailor, the script doesn't change. That
    is putting something before alpha needs to still be in the Greek block, and
    putting something after omega still needs to be in the greek block.
    **Done:** FractionalUCA.txt 6.0 (ICU 4.6) adds special weights that appear
    only in invuca.icu not ucadata.icu and serve as weight allocation
    boundaries.
3.  Variable-top can occur in the middle of a lead byte. But we can't shift half
    a byte. For comparison, the current variables are:
    [chart_Variable](http://unicode.org/charts/collation/chart_Variable.html).
    And do we reorder before or after we do the variable detection? **([ICU
    ticket #8065](http://bugs.icu-project.org/trac/ticket/8065))**
4.  We need to make sure that tailored characters get treated consistently. That
    is, we have to decide the "script" of the bytes that can be filled by
    tailored characters: 04, 28, 29, 2B, 2C, CF-DF below. We have two choices.
    1.  Fixed. The script code assigned to those bytes is invariant. For
        example, we could assign CF-DF to be Hani. (I'm leaning towards this.)
        **Done**
    2.  Dynamic. The script code depends on the tailoring. That means that we
        would recompute the code and store it for each set of rules.
5.  I think we want to consider reordering other categories, not just scripts.
    In particular, people seem to want to do numbers (Nd) and currency marks
    (Sc) separately. These are currently in byte 2A, together with some
    non-variable gorp (see
    [chart_Common](http://unicode.org/charts/collation/chart_Common.html)). Byte
    05 (controlish whitespace) and 06 (spaces) also may want to be separately
    reorderable.
    1.  In Fractional UCA, we may want to separate byte 2A into three lead
        bytes: gorp, then currency symbols (Sc), then numbers (Nd/some Nl). That
        would allow them to be reordered separately.
    2.  **Done:** FractionalUCA.txt 6.0 (ICU 4.6) separates
        space/punct/symbol/currency/digit groups into separate lead bytes and
        the script reordering supports them.
    3.  **TODO:** We should split off and support unassigned-implicit weights as
        a group with separate lead bytes, separate from Han implicits. **([ICU
        ticket #8067](http://bugs.icu-project.org/trac/ticket/8067))**
    4.  Trailing weights are not reordered, just like separators (bytes 00..02)
        and specials (F0..FF).
6.  The UCA has some peculiarities exposed by script reordering. We may want to
    have a tailoring in CLDR for this and/or take it to the UTC.
    1.  There are two Sc characters that are oddly sorted with other blocks
        (4F{Latn}=1 6A{Arab}=1). This would be easy to fix with a tailoring.
        **Done**
    2.  Ogham space mark is not sorted with the same primary as the other
        spaces. We make Space a single byte for space and speed (06). If we
        tailored the Ogham space mark to be the same as Space, reordering 06
        would reorder all spaces. On the other hand, who cares much about Ogham
        space mark sorting... **Not done**
    3.  It appears that people generally want to treat punctuation marks
        different than symbols. It is more common for punctuation to be ignored,
        while not symbols. So ideally, all the punctuation marks would come
        before all symbols. Unfortunately, they are interleaved with symbols
        between 07 and 27 (see
        [chart_Variable](http://unicode.org/charts/collation/chart_Variable.html)).
        While it would be a pain to tailor them all so as to have separate lead
        bytes, it could be done. **Done**
    4.  There are some oddities that, while not an issue for reordering, we
        might also call attention to:
        1.  Po has 95{Talu}=2 **Not done**
        2.  There are a bunch of So's that are all over the map. **Done**

## Fractional UCA

(UCA 5.2 data)

There are two sheets: lead-byte => script/category, and the reverse.

#### Script Reordering

<iframe
src="https://spreadsheets.google.com/spreadsheet/lv?chrome=false&key=0AqRLrRqNEKv-dG0taVI1UlR4Y2VaNU1oTDhYa2lzcmc&output=html&pubredirect=true&widget=true"
width="100%" height="1200" allow="fullscreen" />
