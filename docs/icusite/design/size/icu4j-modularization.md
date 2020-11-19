# ICU4J Modularization

Misc Notes:

*   concerns about making coding inside icu4j more complicated since you can't
    depend on another part of icu4j
    *   don't slice and dice too fine

code (1.8 MB (i18n) + 1.3MB (common)):

*   code components:
    *   i18n: coll, format, regex, translit
    *   common: conv, prop (hardcoded), str, breakiter, set
*   separate library targets for separate functionality
*   top level target which depends on all other targets
*   restructure code to remove dependencies: break away builder from runtime
    code
    *   e.g. code for collator includes build and runtime
    *   e.g. use reflection instead of builder / dependencies
    *   same applies to unicodeset, breakiterator
*   split not just functionality, but also by frequency of use. (thus builders,
    and other code that isn't used very often)

data (13 MB):

*   monolithic 11mb data file -> break up into pieces
    *   format, conv, coll, tz

*   locale dependent separation / data saving
*   mark: might be big win to break out data google never uses (some east asian)
*   but priority 4...last priority

1. separate data

2. separate code

3. split builders apart

4. split by locale
