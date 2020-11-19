# Alphabetic Index

## Status

Available as public ICU API

*   Java:
    [com/ibm/icu/text/AlphabeticIndex.html](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/AlphabeticIndex.html)
*   C++: (i18n library)
    [AlphabeticIndex.html#details](http://www.icu-project.org/apiref/icu4c/classicu_1_1AlphabeticIndex.html#details)

See also CLDR:

*   [LDML spec: Character
    Elements](http://unicode.org/repos/cldr/trunk/specs/ldml/tr35.html#Character_Elements)
    (search there for "index")
*   [LDML spec: Collation
    Indexes](http://unicode.org/repos/cldr/trunk/specs/ldml/tr35-collation.html#Collation_Indexes)

## Original design

What we need from collation to make the indexing work without hardcoded data are
the following:

**1. Given a collation and a string, get the least character in a different
script that is greater than that string.**

I'm implementing this as a hack currently, just doing a binary search in the
following list:

"a", "α", "ⲁ", "а", "ⰰ", "ა", "ա", "א", "&#x10900;", "ࠀ", "ء", "ܐ", "ހ", "ߊ",
"ⴰ", "ሀ", "ॐ", "অ", "ੴ", "ૐ", "ଅ", "ௐ",

"అ", "ಅ", "അ", "අ", "ꯀ", "ꠀ", "ꢂ", "&#x11083;", "ᮃ", "&#x10a00;", "ก", "ກ", "ꪀ",
"ཀ", "ᰀ", "ꡀ", "ᤀ", "ᜀ", "ᜠ", "ᝀ", "ᝠ",

"ᨀ", "ꤰ", "ꤊ", "က", "ក", "ᥐ", "ᦀ", "ᨠ", "ꨀ", "ᬅ", "ꦄ", "ᢀ", "ᱚ", "Ꭰ", "ᐁ", "ᚁ",
"ᚠ", "&#x10c00;", "ꔀ", "ꚠ", "ᄀ",

"ぁ", "ァ", "ㄅ", "ꀀ", "ꓸ", "&#x10280;", "&#x102a0;", "&#x10920;", "&#x10300;",
"&#x10330;", "&#x10428;", "&#x10450;", "&#x10480;", "&#x10000;", "&#x10800;",
"&#x10a60;", "&#x10b00;", "&#x10840;",

"&#x10b40;", "&#x10b60;", "&#x10380;", "&#x103a0;", "&#x12000;", "&#x13000;",
"一"

Since we will have this information internally for the script blocks as part of
the reordering, that should be fairly straightforward to implement, but will
take some more data structure.

**2. Han Collation**

This collation is special, especially for Chinese. The ordering is based on
mapping characters to some data, then ordering by that mapping data. The main
ones are:

1.  Pinyin: compare according to the pinyin for each character. If the pinyin is
    the same, compare by stroke order.
2.  Stroke: compare according to the total strokes for each character. If the
    total strokes are the same, compare by radical-stroke order.
3.  Radical-Stroke: compare according to the radical-stroke for each character.
    If these are the same, compare by code point order.

That means that the index needs to be precisely aligned with the sorting method,
and the index buckets need to be chosen to start on mapping data boundaries
appropriate for each of the above. *If they are out-of-sync, strange results
occur.* The labels also need to be based on the mapping data:

1.  Pinyin: A B C D ...
2.  Stroke: 1 2 3 ...
3.  Radical-Stroke: radicals

In actual display, the labels may be winnowed down, or presented differently (eg
with Stroke the word for "strokes"). For pinyin, notice that both Latin and
Chinese characters are in the same buckets. Ideally Latin is last; for that
we'll wait until script-reordering is in.

This requires not only that the mapping data be available at collation-rule
construction time, but that it also be available at runtime. Luckily, not all of
the data needs to be available, just what the first character of each of the
above buckets is. In an ideal world, that would be another method on the
collator. The situation is complicated by the need to have alternative versions
of the collators to cut down on size, so we can't depend on the first character
of each group being stable for *all* the pinyin collations.

Rather than have some special syntax, the plan is to introduce special
private-use codes, one for each normal index boundary. These will be added to
the rules for each of the above collations.

... X < \\U000F0000 < Y ...

The ranges for these will be hard-coded starting at \\U000F0000.

*   512 for the radicals (the extra bit is for the simplified vs traditional),
    then
*   26 for A-Z for pinyin, then
*   100 for stroke counts.

This has a very minor effect on the size of the collations, but also allows
alternative tailorings and sizes of the tables, once we define the use of these
codes.

**3. Which Pinyin?**

To generate the pinyin for each character, we are creating an ordered list from
the following. The first item in the list is the one used for the collation and
transliteration, so the ordering of the list is important. An item is not added
to the list if it is already in the list.

1.  kHanyuPinlu
2.  kXHC1983
3.  kHanyuPinyin
4.  kMandarin

*This was the priority order recommended by Richard Cook and John Jenkins.*

Some additional pinyin data is added, and the data is closed under:

1.  radical<->Unified mapping (to give the radicals a sort order)
2.  kTraditionalVariant
3.  kSimplifiedVariant

As before, these are only added to the end of the list, so make no effective
difference unless there was no pinyin already.

Any mappings from characters that are canonical equivalents are removed, since
UCA already handles that.
