# Canonically Equivalent Shortest Form

[TOC]

It is possible to do fast searching that takes into account linguistic
equivalents, using a modified Boyer-Moore algorithm. For more information on
such matching, see [Searching and Matching in UAX#10: Unicode Collation
Algorithym](http://unicode.org/reports/tr10/#Searching).
In order to do this, it is important to determine a canonically equivalent
shortest form (CESF). This is usually, but not always, the [NFC
form](http://www.unicode.org/reports/tr15/). ICU has a
[CanonicalIterator](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/CanonicalIterator.html)
which can be used to get all the canonically equivalent variants. One could use
it to iterate through and select the shortest form. However, it was not designed
for this particular task -- more efficient code can be designed that uses a
modified NFC algorithm. This document presents some thoughts on how to do this.
In an annex, it also presents a [Quick&Dirty
Approach](normalizing-to-shortest-form.md) using the CanonicalIterator.
Note that here when we say "decomposition" or "equivalence", we always mean
*canonical*. Counts and other information are as of Unicode 5.1.

---

### Background

We want uniqueness for the CESF, and also for it to be the same as NFC where
possible. Note that CESF does not have the same stability constraints as Unicode
Normalization Forms - it is intended purely for internal processing.
We start by looking at the two kinds of cases where NFC is not shortest, as
discussed on [nfc-faq](http://www.macchiato.com/unicode/nfc-faq).

*   **Breakage**. The character breaks apart, as in [breakage
    list](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B:toNfc=/../:%5D).
    There are 85 such characters.
    *   There is only one case where the first character is not a starter:
        `[U+0344](http://unicode.org/cldr/utility/character.jsp?a=0344)` ( ̈́ )
        COMBINING GREEK DIALYTIKA TONOS =>
        `[U+0308](http://unicode.org/cldr/utility/character.jsp?a=0308)` ( ̈ )
        COMBINING DIAERESIS +
        `[U+0301](http://unicode.org/cldr/utility/character.jsp?a=0301)` ( ́ )
        COMBINING ACUTE ACCENT
    *   The non-first characters in the NFC form of other breakage characters
        are called *trail-breakers*. There are [24 such
        characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5Cu0338%5Cu05B4%5Cu05B7-%5Cu05B9%5Cu05BC%5Cu05BF%5Cu05C1%5Cu05C2%5Cu093C%5Cu09BC%5Cu0A3C%5Cu0B3C%5Cu0F72%5Cu0F74%5Cu0F80%5Cu0FB5%5Cu0FB7%5CU0001D165%5CU0001D16E-%5CU0001D172%5D).
*   **Interference**. Some characters interfere with forming the shortest form.
    These are called *irritations*. There are [45 such
    characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5Cu0104%5Cu0105%5Cu0118%5Cu0119%5Cu013B%5Cu013C%5Cu0156%5Cu0157%5Cu015E%5Cu015F%5Cu0172%5Cu0173%5Cu01A0%5Cu01A1%5Cu01AF%5Cu01B0%5Cu01EA%5Cu01EB%5Cu0218%5Cu0219%5Cu0228%5Cu0229%5Cu1E00%5Cu1E01%5Cu1E18-%5Cu1E1B%5Cu1E62%5Cu1E63%5Cu1E72-%5Cu1E77%5Cu1EA0%5Cu1EA1%5Cu1EB8%5Cu1EB9%5Cu1ECC%5Cu1ECD%5Cu1EE4%5Cu1EE5%5CuFB49%5D).

Luckily, all of these cases are rather rare in practice. The most common ones
are the Indic nuktated forms.

#### Example

An example of interference is the sequence
`[U+1EA5](http://unicode.org/cldr/utility/character.jsp?a=1EA5)` ( ấ ) LATIN
SMALL LETTER A WITH CIRCUMFLEX AND ACUTE +
`[U+0328](http://unicode.org/cldr/utility/character.jsp?a=0328)` ( ̨ ) COMBINING
OGONEK. What happens is that the
`[U+0105](http://unicode.org/cldr/utility/character.jsp?a=0105)` ( ą ) LATIN
SMALL LETTER A WITH OGONEK forms first, and prevents the formation of the
shorter composite. (See below for a detailed example.)

### Specification

Use the basic NFC algorithm, with the following changes.

1.  Do not exclude the composition exclusions. That will prevent all but one of
    the breakages above.
2.  Traverse through the segment, as in NFC.
    1.  However, where each composition with the starter is possible,
        recursively process both the result with the composition, and the result
        without.
    2.  If the non-starter is a lead-trail, see if it can be combined with a
        following character. If so, recursively process both the result with the
        composition, and the result without. (This is to prevent the other
        breakage mentioned above.)
    3.  The recursion is always depth-first, with composition being the first
        branch and uncomposed choice being the second.
    4.  Among the shortest final results, pick the one that is NFC if there is
        one. Otherwise, pick the first one (in branch order).

#### Example

In the following case, each row represents looking at that character in the
sequence. The highlighted cells are where a new composition is produced.

( a )
( ̨ ) ogonek
( ̂ ) circumflex
( ́ ) acute ( ą ) a-ogonek
( ̂ ) circumflex
( ́ ) acute
***// NFC Form***
( a )
( ̨ ) ogonek
( ̂ ) circumflex
( ́ ) acute ( ą ) a-ogonek
( ̂ ) circumflex
( ́ ) acute
( â ) a-circumflex
( ̨ ) ogonek
( ́ ) acute ( a )
( ̨ ) ogonek
( ̂ ) circumflex
( ́ ) acute ( ą ) a-ogonek
( ̂ ) circumflex
( ́ ) acute ( ấ ) a-circumflex-acute
( ̨ ) ogonek
***// Shortest form***
( â ) a-circumflex
( ̨ ) ogonek
( ́ ) acute ( a )
( ̨ ) ogonek
( ̂ ) circumflex
( ́ ) acute

There is only one shortest form (the final answer in column 2), so it is the
result. If the result in column #2 had not been present, then column #1 would
have been the first shortest value (and also NFC), so it would have been chosen
as the result.

### Implementation

The above lends itself to many optimizations. Although generating a tree of
results *seems* scary, in practice there can only be a few branches. Moreover,
one can easily prune the results along the way. One only needs to recurse, for
example, if there is a possibility that there is an interfering character. So a
small list of such characters can be tested for to avoid excess processing.
Where data size is an issue, the majority of the data and can be shared with the
NFC implementation; only the breakage cases need to have a special flag.
Implementation can also be done by postprocessing a string in NFC:

Make a "problem list" of characters that can occur in a case of breakage or
interference. This list will need to be generated for each version of Unicode,
but can be done at build time. It includes all of the characters in the
[breakage
list](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B:toNfc=/../:%5D),
like a nukta character, plus all the characters that are formed in the case of
interference (like
`[U+0105](http://unicode.org/cldr/utility/character.jsp?a=0105)` ( ą ) LATIN
SMALL LETTER A WITH OGONEK plus circumflex plus grave, from the above case).
After the string is in NFC, take another pass through, looking for any starter
sequence where the starter and at least one non-starter are in the problem list.
Because the non-starters are quite rare in practice (see
[nfc-faq](http://www.macchiato.com/unicode/nfc-faq)) a quick scan can be done
for them, backing up to check the preceding starter if one is found. In
addition, in each sequence of non-starters, check for the sequence:

*   `[U+0308](http://unicode.org/cldr/utility/character.jsp?a=0308)` ( ̈ )
    COMBINING DIAERESIS +
    `[U+0301](http://unicode.org/cldr/utility/character.jsp?a=0301)` ( ́ )
    COMBINING ACUTE ACCENT

All of these problem starter sequences should be relatively rare in practice,
allowing for a very quick scan. If we find a candidate, find its enclosing
segment (see below) and do the above processing.

See also the [Quick&Dirty Approach](normalizing-to-shortest-form.md). It won't
give precisely the same results, but should be sufficient.

### Annex: Types of Characters

For background information, here are some relevant types of characters. Because
Hangul is unproblematic, we can exclude that from the counts below for
simplicity.

*   A character is *decomposable* when it has a decomposition. There are [2,043
    such
    characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5B:nfdqc=n:%5D-%5B:hangulsyllabletype=LVT:%5D-%5B:hangulsyllabletype=LV:%5D%5D).
*   A character is *lead-only* when it is in the multi-character decomposition
    of some character, but never except as the first character. There are [295
    such
    characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3C-%3EA-PR-Za-pr-z%5Cu00A8%5Cu00C6%5Cu00D8%5Cu00E6%5Cu00F8%5Cu017F%5Cu01B7%5Cu0292%5Cu0391%5Cu0395%5Cu0397%5Cu0399%5Cu039F%5Cu03A1%5Cu03A5%5Cu03A9%5Cu03B1%5Cu03B5%5Cu03B7%5Cu03B9%5Cu03BF%5Cu03C1%5Cu03C5%5Cu03C9%5Cu03D2%5Cu0406%5Cu0410%5Cu0413%5Cu0415-%5Cu0418%5Cu041A%5Cu041E%5Cu0423%5Cu0427%5Cu042B%5Cu042D%5Cu0430%5Cu0433%5Cu0435-%5Cu0438%5Cu043A%5Cu043E%5Cu0443%5Cu0447%5Cu044B%5Cu044D%5Cu0456%5Cu0474%5Cu0475%5Cu04D8%5Cu04D9%5Cu04E8%5Cu04E9%5Cu05D0-%5Cu05D6%5Cu05D8-%5Cu05DC%5Cu05DE%5Cu05E0%5Cu05E1%5Cu05E3%5Cu05E4%5Cu05E6-%5Cu05EA%5Cu05F2%5Cu0627%5Cu0648%5Cu064A%5Cu06C1%5Cu06D2%5Cu06D5%5Cu0915-%5Cu0917%5Cu091C%5Cu0921%5Cu0922%5Cu0928%5Cu092B%5Cu092F%5Cu0930%5Cu0933%5Cu09A1%5Cu09A2%5Cu09AF%5Cu09C7%5Cu0A16%5Cu0A17%5Cu0A1C%5Cu0A2B%5Cu0A32%5Cu0A38%5Cu0B21%5Cu0B22%5Cu0B47%5Cu0B92%5Cu0BC6%5Cu0BC7%5Cu0C46%5Cu0CBF%5Cu0CC6%5Cu0D46%5Cu0D47%5Cu0DD9%5Cu0F40%5Cu0F42%5Cu0F4C%5Cu0F51%5Cu0F56%5Cu0F5B%5Cu0F71%5Cu0F90%5Cu0F92%5Cu0F9C%5Cu0FA1%5Cu0FA6%5Cu0FAB%5Cu0FB2%5Cu0FB3%5Cu1025%5Cu1B05%5Cu1B07%5Cu1B09%5Cu1B0B%5Cu1B0D%5Cu1B11%5Cu1B3A%5Cu1B3C%5Cu1B3E%5Cu1B3F%5Cu1B42%5Cu1FBF%5Cu1FFE%5Cu2190%5Cu2192%5Cu2194%5Cu21D0%5Cu21D2%5Cu21D4%5Cu2203%5Cu2208%5Cu220B%5Cu2223%5Cu2225%5Cu223C%5Cu2243%5Cu2245%5Cu2248%5Cu224D%5Cu2261%5Cu2264%5Cu2265%5Cu2272%5Cu2273%5Cu2276%5Cu2277%5Cu227A-%5Cu227D%5Cu2282%5Cu2283%5Cu2286%5Cu2287%5Cu2291%5Cu2292%5Cu22A2%5Cu22A8%5Cu22A9%5Cu22AB%5Cu22B2-%5Cu22B5%5Cu2ADD%5Cu3046%5Cu304B%5Cu304D%5Cu304F%5Cu3051%5Cu3053%5Cu3055%5Cu3057%5Cu3059%5Cu305B%5Cu305D%5Cu305F%5Cu3061%5Cu3064%5Cu3066%5Cu3068%5Cu306F%5Cu3072%5Cu3075%5Cu3078%5Cu307B%5Cu309D%5Cu30A6%5Cu30AB%5Cu30AD%5Cu30AF%5Cu30B1%5Cu30B3%5Cu30B5%5Cu30B7%5Cu30B9%5Cu30BB%5Cu30BD%5Cu30BF%5Cu30C1%5Cu30C4%5Cu30C6%5Cu30C8%5Cu30CF%5Cu30D2%5Cu30D5%5Cu30D8%5Cu30DB%5Cu30EF-%5Cu30F2%5Cu30FD%5CU0001D157%5CU0001D158%5CU0001D1B9%5CU0001D1BA%5D).
    *   Exactly one of these is a non-starter:
        `[U+0F71](http://unicode.org/cldr/utility/character.jsp?a=0F71)` ( ཱ )
        TIBETAN VOWEL SIGN AA
*   A character is *trail-only* when it is in the multi-character decomposition
    of some character, but never as first. There are [75 such
    characters](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5Cu0300-%5Cu0304%5Cu0306%5Cu0307%5Cu0309-%5Cu030C%5Cu030F%5Cu0311%5Cu0313%5Cu0314%5Cu031B%5Cu0323-%5Cu0328%5Cu032D%5Cu032E%5Cu0330%5Cu0331%5Cu0338%5Cu0342%5Cu0345%5Cu05B4%5Cu05B7-%5Cu05B9%5Cu05BC%5Cu05BF%5Cu05C1%5Cu05C2%5Cu0653-%5Cu0655%5Cu093C%5Cu09BC%5Cu09BE%5Cu09D7%5Cu0A3C%5Cu0B3C%5Cu0B3E%5Cu0B56%5Cu0B57%5Cu0BBE%5Cu0BD7%5Cu0C56%5Cu0CC2%5Cu0CD5%5Cu0CD6%5Cu0D3E%5Cu0D57%5Cu0DCA%5Cu0DCF%5Cu0DDF%5Cu0F72%5Cu0F74%5Cu0F80%5Cu0FB5%5Cu0FB7%5Cu102E%5Cu1B35%5Cu3099%5Cu309A%5CU0001D165%5CU0001D16E-%5CU0001D172%5D).
    *   Eighteen of these are starters.
*   A character is *lead-trail* when it can be both leading and trailing. There
    is only [one such
    character](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%5Cu0308%5D),
    a non-starter.
    *   `[U+0308](http://unicode.org/cldr/utility/character.jsp?a=0308)` ( ̈ )
        COMBINING DIAERESIS
    *   (It is the lead in the decomposition of
        `[U+0344](http://unicode.org/cldr/utility/character.jsp?a=0344)` ( ̈́ )
        COMBINING GREEK DIALYTIKA TONOS)
*   A character is *reorderable* if it is not a starter, and none of the above.
    There are 444 such characters. These characters don't combine, but they do
    reorder.
*   A character is *stable* when it is a starter, and is none of the above.
    These are the rest of the million-odd code points in Unicode (excluding
    Hangul).

Logically speaking, we'll assume that the string is already transformed into
NFD.
We can logically break up any NFD string into a set of segments, where each
segment starts with a lead-only starter or stable character, and has zero or
more others characters afterward. There is also the first segment, which may be
"headless". Each segment does not interact with any characters before it or
after it, in terms of canonical equivalence. Thus if each segment is mapped to
the CESF, then the string as a whole will be in CESF. So then the problem
devolves to mapping the segments.
Luckily, we can do this with the same mechanism as in NFC, looking at sequences
that are starter + non-starter. \[TBD: show why this is sufficient.\]

#### Quick&Dirty Approach

Here is a relatively easy way to get a shortest form that would probably suffice
for collation in terms of function and performance: Function, because it appears
that we don't actually need uniqueness, just the length of the shortest form;
Performance, just because the bad cases are so rare.

1.  Convert to NFC.
2.  Traverse the NFC string by starter-sequence.
    1.  If the segment looks problematic, then use the CanonicalIterator to get
        a shortest form, and substitute.
    2.  Otherwise continue.

The problematic segments are those of more than one character that meet any of
the following conditions:

1.  Contains an irritation
2.  Contains a trail-breaker
3.  Contains `[U+0308](http://unicode.org/cldr/utility/character.jsp?a=0308)` (
    ̈ ) COMBINING DIAERESIS +
    `[U+0301](http://unicode.org/cldr/utility/character.jsp?a=0301)` ( ́ )
    COMBINING ACUTE ACCENT
