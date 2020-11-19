# CLDR Data

There are a few ways to reduce the data size by working on the conversion from
CLDR data to ICU.

**File selection:** The most obvious size reduction is by only including a
certain set of data files in an ICU build. **File dependencies** should be
considered, and are partially enforced by icupkg. (For example, for each locale
resource bundle, its parent bundle should be included.) The **res_index.txt**
file should be updated (and res_index.res regenerated) when the set of resource
bundles in a locale tree changes.

**Translation selection:** Even when files for a smaller set of languages are
selected, those still contain translations for all the languages/regions/time
zones etc. for which CLDR has data. Example: If only English (en\*.txt) and
Japanese (ja\*.txt) bundles are used, they still contain display names for the
languages French, Thai, Zulu and hundreds of others. A white list of such
entities could drive both the file selection and the selection of strings inside
the resource bundles. Intra-file selection would have to be done with the
LDML2ICUConverter. Investigate whether it is sufficient to create a config.xml
file for this.

**Shorter keys:** Even with key suffix sharing, ICU 4.2 resource bundles contain
nearly 500kB of key string characters. (Most data is stored as key/value pairs.)
It might help to use shorter keys, where they are arbitrary (that is, keys like
"abbreviation", but not date/time pattern skeletons or transliterator IDs, nor
otherwise data-driven, etc.). Requires changes in both the LDML2ICUConverter and
in the runtime code (which should be able to use both the old and the new keys).
For more about keys, see the [Keys](keys.md) page.

**Smaller values:**

*   Rule strings: Remove **comments and non-essential white space**. (Already
    done for [collation](collation.md) by LDML2ICUConverter and genrb.) No
    runtime change.
*   Rule strings: Use more **compact syntax**. For example, in collation, a
    special sequence-of-primary-differences syntax like in CLDR. Addition to
    builder code.
    *   Note: It is tempting to use fewer characters by replacing a sequence of
        ASCII characters with a single symbol, for example the collation
        tertiary difference "<<<". However, this may not help under string value
        compression which favors ASCII.
*   **Enumerated values:** In some places, we use strings to express enumerated
    values, for example, "FORWARD" and "REVERSE" in the transliterator data.
    Consider using small integer values instead, or compression-friendly short
    ASCII strings. Requires runtime change (should support both old and new
    values) and may reduce readability (which is probably ok). Size reduction
    may not be large because of string duplicate elimination and string
    compression.
