# Timezone debloating

Here are some thoughts on reducing timezone bloat. This is a rough draft, so
please add comments, other options, etc.
The problem is that when we open a formatter, we read in all of the timezone
data that is relevant for formatting, and create a large dictionary of all the
possible output strings. That is then used to parse. A few key issues:

1.  We create this large array even if we are just formatting (the 90+% case).
2.  The array is expensive to build, and sticks around in cache.

So let's back up a bit. The timezones are formatted according to

*   http://unicode.org/reports/tr35/#Date_Format_Patterns
*   http://unicode.org/reports/tr35/#Time_Zone_Fallback
*   http://unicode.org/reports/tr35/#Timezone_Names
*   http://unicode.org/reports/tr35/#Supplemental_Timezone_Data
*   (wish this was all in one place!)

These correspond to the z, zzzz, Z, ZZZZ, v, vvvv, V, and VVVV patterns (whereby
z's and V vary by daylight vs standard). The data in support of this involves:

1.  patterns to glue stuff together, eg {0} ({1})
2.  a few special strings, eg GMT
3.  specific zone names
4.  specific metazone names
5.  country names

Here's what I propose we do.

1.  Create a TimezoneFormat, inheriting from UFormat / Format (Java vs C++)
    1.  For formatting, it formats a zone. The format object has getters/setters
        for parameters to set the above 9 forms (but with orthogonal
        parameters).
    2.  For parsing, it takes a string from a given point, and returns a zone
        (if possible) and how far it got.
2.  Refactor DateFormat and Timezone, so that each has-a TimezoneFormat, and
    instanciates only on need.
3.  Now change the implementation of TimezoneFormatter.
    1.  For formatting
        1.  Just use the data from the resource bundles directly; don't copy
            them into dictionaries or lists.
    2.  For parsing:
        1.  Instead of putting together a big dictionary, which is a
            cross-product of a bunch of smaller ones, have a few small
            dictionaries that map the specific names. Those dictionaries can be
            loaded quickly, and are smaller.
        2.  When parsing, try multiple times with the different possible glue
            patterns and parsing the subcomponents. If there are multiple
            matches take the longest one.

Example: when parsing Pacific Time (Canada)

*   Check for Z format (eg -0800) - no match, try next
*   Parse for GMT{0} - no match, try next
*   Parse for specific timezone - no match, try next
*   Parse for metazone standard - no match, try next
*   Parse for metazone generic - succeeds, so remember America/Los_Angeles, and
    how far we got; try next
*   Parse for metazone generic + country/city - succeeds, now longest so
    remember, America/Vancouver and how far we got; try next
*   Parse for fallbackFormat, with country/city & generic metazone - no match,
    try next
*   Return longest (America/Vancouver) and how far we got.

The above can be optimized, but we may not need much, since the vast majority of
failures will fail early (and thus not be expensive). This will make it slightly
slower to parse, but that operation is not typically time-critical. In return,
it lowers the initialization time, and reduces the current memory needs
substantially.
