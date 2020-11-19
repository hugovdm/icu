# Number format skeleton support

## Status

This feature is implemented in ICU 62. For documentation, see:

<https://github.com/unicode-org/icu/blob/master/docs/userguide/format_parse/numbers/skeletons.md>

## Introduction

Often people want to be able to do with number patterns what they can with dates
- Specify that they want a currency, with min fractions=x and max fractions=y,
etc. But that they want the pattern that is appropriate for the locale.

<http://bugs.icu-project.org/trac/ticket/8610>

## Background

ICU number formatters support different styles, like percent, currency,
scientific or decimal. Those produce locale appropriate results, but are not
flexible.
One can create a formatter using a user specified pattern, with full control of
significant digits, min/max number of fractional digits... Problem with this
approach is that the result is usually not locale appropriate.

## ~~Solution~~

~~Introduce skeleton feature to number formatting, similar to what date formatters have today. The skeleton would allow user to specify:~~

*   ~~min and max number of fraction digits, using .0# notation~~
*   ~~min number of integer digits, using 0. notation~~
*   ~~number of significant digits, using @ character~~
*   ~~type of the formatter - currency or percent if corresponding symbols are
    used, otherwise decimal~~
*   ~~if grouping separator is on or off - user can’t specify actual placement
    of the separator.~~
*   ~~whether the currency symbol, ISO 4217 code, or name (eg “US Dollar”)
    should be used for currency formatting - the actual placement of the
    symbol/ISO/plural is locale specific.~~

~~The skeleton syntax would be similar to current pattern syntax with couple restrictions/changes in meaning:~~

*   ~~Skeleton would be specified with leading !! mark~~
*   ~~There is no support for a separate negative pattern using the ; character.
    The negative format is derived from the locale info.~~
*   ~~If <comma> is present it means use grouping separator, if it’s not present
    then the grouping separator is off. The actual grouping (the character and
    position) is locale dependent.~~

## ~~Questions~~

*   ~~cira: Should we allow 0-9 for rounding, @ for significant digits, \\u2030
    for per mile and E for scientific?~~
    *   ~~mark: @ is the most useful.~~
*   ~~markus: Should there be a separate builder class, like
    DateTimePatternGenerator vs. DateFormat?~~
*   ~~markus: Should we support date and number skeletons in MessageFormat?~~

## ~~Examples~~

~~!!0,0.000#%~~

*   ~~since there is a %, pick up the percent pattern for the locale~~
*   ~~since there is a grouping separator, set grouping separator = true~~
*   ~~set the min/max fractions and min integers to match the skeleton - min 2
    integer digits, min 3 fraction digits, max 4 fraction digits.~~

~~!!¤¤¤0,0.00~~

*   ~~since ¤ occurs, pick up the currency pattern for the locale.~~
*   ~~replace the ¤ by ¤¤¤ to match the request.~~
*   ~~since there is a grouping separator, set grouping separator = true~~
*   ~~set the min integers to match the skeleton. (The min/max fractions are set
    by the currency when formatting)~~
