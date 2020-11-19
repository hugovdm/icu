# ICU 4.6 Changes

[TOC]

The following are changes planned for collation for ICU 4.6. Some of them come
from CLDR, but I wanted to have them in one place.

## Changes in CLDR data

1.  Many locale-specific changes. \[John managing this\].
2.  Basing Pinyin and Radical stroke collations on Unihan data. Draft rules are
    in <http://www.unicode.org/review/pr-175/>, and may be updated during the
    public review period. These include collations for pinyin, stroke,
    radical-stroke. For comparison, pinyin transliteration is also included.
    Some additional data sources are used besides Unihan.
3.  Removing “backwards secondaries” from default French collation. Users will
    still be able to set this option parametrically or via locale keywords (such
    as “fr-u-kb-true”) when using French (or other languages); the only change
    is that this option will no longer be the default for French. It will
    however be the default for fr-CA.
4.  Collation rules will also allow an “import” statement. This allows for size
    reductions in the source strings, better maintenance, and use of the default
    European Ordering Rules for languages of the European Union.
5.  Tailored UCA DUCET. CLDR will use a tailored UCA DUCET (Default Unicode
    Collation Element Table) in the root locale. This will be inherited by all
    other locales by default. However, there will be a separate collation also
    in root, with the keyword “ducet”. Using that keyword, the locale ID
    “und-u-co-ducet” will allow access to the original DUCET. Using the DUCET
    ordering tailored for another language will require building a collator from
    the concatenation of the ducet rules and the language rules. The root locale
    ordering will be modified in the following ways:
    1.  Punctuation will be grouped together, below symbols and above
        whitespace. The relative order of the punctuation matches the DUCET.
        This grouping only matters where a punctuation mark in one string is
        compared to a symbol in another, eg, “I♥NY” vs “I-NY”
    2.  There are two options in the UCA for symbols and punctuation:
        non-ignorable, or shifted. With the shifted option, symbols and
        punctuation are ignored -- except at a fourth level. The default setting
        for CLDR will be modified so that symbols are not affected by the
        shifted option. So shifted only causes controls, spaces, and punctuation
        to be ignored, but not symbols (like ♥). The old behavior can be
        specified with a locale ID such as “fr-u-vt-1D371” to set the Variable
        section to include all of the symbols below it, or be set parametrically
        where implementations allow access. See also:
        *   <http://www.unicode.org/reports/tr35/tr35-16.html#Key_Type_Definitions>
            under “Collation parameters”
        *   <http://www.unicode.org/charts/collation/>
    3.  In the DUCET, almost all currency symbols are grouped together before
        numbers. For the tailored UCA DUCET the two exceptions to this pattern
        are also moved into this group:
        *   U+20A8 ( ₨ ) RUPEE SIGN
        *   U+FDFC ( ﷼ ) RIAL SIGN

## Changes in FractionalUCA, UCA_Rules

1.  **Change the weight allocation to allow for a large gap for CJK.** ([The gap
    had been shrinking because of new
    scripts.](https://docs.google.com/Doc?docid=0AQ3w_MjvUEoRZGRzcnJwajVfMTRkOHB3bXNnZA))
    This will involve changes to character weights:
    1.  Make “infrequent” characters have 3-byte weights.
        1.  For UCA 5.2, this was for old jamo, plus implicit BMP han. The
            current version adds:
        2.  All characters in scripts outside of
            [tr31.html#Table_Recommended_Scripts](http://unicode.org/draft/reports/tr31/tr31.html#Table_Recommended_Scripts),
            also excluding 'Aspirational'
        3.  Non-currency symbols that sort before Latin.
    2.  Make “most frequent” characters have 1-byte weights.
        1.  For UCA 5.2, this was a-z, plus space. The current version adds:
        2.  0-9, period, and comma.
    3.  We can tune these further, if people have recommendations based on the
        data.
2.  **Make the changes corresponding to the *Tailored UCA DUCET*.**
    1.  Group punctuation together.
    2.  Move variable-top below symbols.
    3.  Group currency symbols together.
3.  **Generate the table of “reverse” rules. That is,** produce a tailoring of
    the CLDR_DUCET that is the same as the DUCET. Check these in under ‘ducet’
    in CLDR root.

Changes in ICU code

1.  Scripts and certain other categories of characters (whitespace, currency
    symbols, punctuation, most numbers, other symbols) will be parametrically
    reorderable. For example, the rules for Greek would be able to specify that
    the sorting order is:
    1.  punctuation < Greek letters < numbers < currency symbols < Latin letters
        < other scripts and characters.
2.  Special noncharacter weights.
    1.  The code point U+FFFF will be tailored to have a weight higher than all
        other characters. The weight will be EF FE. This allows reliable
        specification of a range, such as “Sch” ≤ X ≤ “Sch\\uFFFF” within code
        point space.
    2.  The code point U+FFFE will be tailored to have a weight lower than all
        other characters. The weight will be 03 04. This allows for
        [Interleaved_Levels](http://unicode.org/reports/tr10/#Interleaved_Levels)
        within code point space.
    3.  These values will not be tailorable, and nothing can tailor to them.
        That is, neither can occur in a collation rule: for example, & \\uFFFF <
        x is illegal.
3.  All other noncharacters, and all isolated surrogates, get treated as
    unassigned code points; that is, they get computed implicit values.

## Issues

### Variable Top Values

The variable-top attribute defines which characters get "shifted" primary
weights use. The attribute value is a sequence of code points. It requires that
a user look at the current UCA table and find the desired first character below
which everything gets shifted. The behavior will change when that selected
character is moved around in the UCA ordering, or is not the first/last of a
group any more.

Idea: Provide a small set of keywords with stable semantics, with code point
sequences or CEs defined by the UCA table itself:

*   de-u-vt-**none**: "Shifted" does not affect any characters. (CEvt=00)
*   de-u-vt-**spaces**: Shifts only spaces. (CEvt=first punctuation CE)
*   de-u-vt-**punct**: Shifts spaces and punctuation. (CEvt=first symbol CE)
*   de-u-vt-**symbols**: Shifts spaces, punctuation and most symbols.
    (CEvt=first currency symbol CE)
*   de-u-vt-**currency**: Shifts up to and including currency symbols.
    (CEvt=first digit CE)
*   de-u-vt-**digits**: Shifts up to and including digits. (CEvt=first letter
    CE)
*   de-u-vt-**ducet**: Shifts the same set of characters as the DUCET default.
