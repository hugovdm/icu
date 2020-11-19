# Number Parsing

[TOC]

Author Mark Davis Date 2009-06-19
Status Proposal
In ICU, numbers are parsed according to a pattern and number symbols that are
appropriate for a locale. We need to have some leniency in parsing numbers, but
not so much that we interpret what are likely errors, such as "1.1" being
interpreted as 11 in French (as in the latest, out-of-the-box version of ICU).
Here are some thoughts on how to do that.

## Lenient Parse

We allow for some equivalency between characters used in the number element of
the pattern. The separators fall into 4 classes, where we can allow for
equivalents:

*   private final UnicodeSet dotEquivalents =(UnicodeSet) new UnicodeSet("\[.．․﹒
    。｡︒۔٬\]").freeze();
*   private final UnicodeSet commaEquivalents = (UnicodeSet) new
    UnicodeSet("\[,，﹐ ، ٫ 、﹑､،\]").freeze();
*   private final UnicodeSet apostropheEquivalents = (UnicodeSet) new
    UnicodeSet("\[︐︑ '＇ ‘ ’ \]").freeze();
*   private final UnicodeSet spaceEquivalents = (UnicodeSet) new
    UnicodeSet("\[:whitespace:\]").freeze();

The first two can be either decimal or grouping signs; the others can be only
grouping signs. So for a particular locale, we can include exactly one of the
first two as the decimal sign (based on locale data), and all of the others as
grouping sign. However, we need to have some restrictions on these:

1.  There can either zero or one decimal sign, but not more. We do allow a final
    decimal sign (eg "2.0", "2", or "2." all parse the same)
2.  The grouping sign can be completely missing (eg "12345")
3.  If there are multiple grouping signs, they must must be exactly the same
    character (eg neither "1 234,567" nor "1﹒ 234 。567")
4.  If there is a grouping sign, the intervals must be one of the following, and
    no others (eg not "1,234,56", nor "12,34" nor "1,2")
    1.  every three, or
    2.  every four, or
    3.  hindi-style (twos and then three).
5.  Grouping signs are not allowed after the decimal sign (eg not
    "1.234,567,890")

Other items:

1.  Whitespace is allowed as leading, trailing, or between elements (eg between
    currency and number, or "(" and number, etc.
2.  In addition to an explicit negative format, such as in "0.0; (0.0)", we
    should also always allow a minus sign before or after the positive format
    (or between the number and a currency sign, if there is one) as a negation.
    Thus for the above example, we'd parse "(12)" or "-12" or "12-".
3.  The parse is case-insensitive (mostly an issue for currencies).
4.  Currencies allow for the currency symbol, international symbol, or name(s).
5.  Digits are allowed to be from any decimal set
    (http://unicode.org/cldr/utility/list-unicodeset.jsp?a=\\p{nd}) but must all
    be from the same set.
6.  In the API methods without a ParsePosition (or it is null in others), then
    any trailing characters (other than whitespace) cause an error.
7.  Note: I think we already test patterns, to make sure that they are sane, eg
    no "###-hi mom-##0".
8.  We also use equivalents for the other common elements:
    *   private final UnicodeSet minusEquivalents =(UnicodeSet) new
        UnicodeSet("\[:dash:\]").freeze();
    *   private final UnicodeSet openEquivalents =(UnicodeSet) new
        UnicodeSet("\[ ( ❨﹙...\]").freeze();
    *   ...

We probably should request the equivalency data be added to CLDR, but in the
supplemental data; we don't want to have it vary by locale.

## Strict Parse

A strict parse would expect the characters to match exactly (no equivalents),
but allow the above flexibility in whitespace except within numbers, and in
grouping (subject to the above constraints). Digits can be from either ASCII or
the native set(s) for the locale, but no others; they must be all from the same
set.
Issue: should we make the parsing be
canonical/compatibility-equivalent-insensitive? My initial thought is that we
can adjust the equivalency sets to get most of the benefit, and perhaps do
something with the Currencies, but not aim for full
canonical/compatibility-equivalent-insensitive.
