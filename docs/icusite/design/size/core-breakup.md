# Core Breakup

Here is a sketch of what we could do:

Refactor core (ICU4J) into 3 pieces. The motivation is to separate out
everything that is involved in formatting (and parsing) into a separate project.
So we'd have:
**Format** - Collate - Translit // higher level modules
**FormatCore**
Core

*   Format contains formatting (and parsing) classes (from the old Core)
*   Core contains all the stuff that doesn't depend on any (substantive)
    language-specific format data. We make a CoreLocale that has all the
    functionality of ULocale other than formatting-dependent stuff. We widen out
    all the input ULocale parameters in ICU to use CoreLocale instead.
*   FormatCore contains stuff from the old Core needed by more than one of
    Format/Collate/Translit, with as little formatting info as we can - it is a
    bit of a hack, but necessary for compatibility. ULocale is the main item
    here, and really only needed for getAvailableLocales.

So the remaining Core has stuff like:

*   UCharacter/UProperty/UScript
*   Math stuff
*   CoreLocale (no display name, no available locales, etc)
*   LocaleData
*   PluralRules
*   UnicodeSet
*   Properties
*   ResourceBundles
*   BreakIterator
*   Calendar
*   BOCU/SCSU/PunyCode/IDNA/StringPrep
*   Bidi/Shaping
*   Charset detection
*   Character iterators
*   DigitList
*   Normalizer
*   Utilties (Freezable, Measure, MeasureUnit, VersionInfo,...)
*   UniversalTimeScale
*   MessageFormatBase

UnicodeSet

*   We also split UnicodeSet into two parts: CoreUnicodeSet and UnicodeSet, with
    a separate UnicodeSetFormat class for format/parsing.
*   UnicodeSet would inherit from CoreUnicodeSet, and have-a UnicodeSetFormat.
*   We'd widen our input parameters where possible to take a CoreUnicodeSet, and
    replace all the Core usages.
*   CoreUnicodeSet has no parsing, and only very simple formatting (lists of
    ranges). CoreUnicodeSet adds a couple of extra methods that use UCharacter
    properties directly, eg addAllIntProperty(int propEnum, int valueEnum). For
    constants like UnicodeSet foo = new UnicodeSet("\[\[:cn:\]\[:co:\]\]"),
    someone (and all our Core code) could use these methods, eg
    *   UnicodeSet foo = new UnicodeSet("\[\[:cn:\]\[:co:\]\]")
        =>
    *   UnicodeSet foo = new UnicodeSet()
        .addAllIntProperty(UProperty.GENERAL_CATEGORY, UCharacter.UNASSIGNED)
        .addAllIntProperty(UProperty.GENERAL_CATEGORY, UCharacter.PRIVATE_USE);

From Doug:

The data chunks I've thought of splitting out are locale display name
data, time zone name data, and currency format data. Formatting
numbers ought to be pretty cheap, but the locale display names are
heavy. Once they're split out, the currency display names are next.
Formatting dates and times ought to be relatively cheap as long as you
don't need long time zone display names. Currently, to do either of
these, you drag along lots of stuff.
I'm not sure formatters even really need core property data.
UnicodeSet drags in a log of it, when really all that's needed is a
quick way to build a character set for some scanners. This is almost
always just a property list that gets turned into a set. These sets
could be generated at build time instead.
Besides this, there are some APIs that sit at a high level and call
down into multiple packages. GlobalizationPreferences and
StringSearch were the initial examples I hit. I put both into
Collation when I did the original refactoring but they should be
pulled into some 'client' module that depends on multiple other
modules.

I did some more work on MessageFormat, and it looks promising. I separated out
an interface, FormatRegistry, and refactored an implementation that just does
the old stuff (SimpleFormatRegistry). Later on it can be a more real version,
where we can actually register new stuff, but for now it is equivalent in
functionality. If this looks good, I'd like to go ahead with it.
In MessageFormat, I have:
static final FormatRegistry formatRegistry = (FormatRegistry)
newInstanceOrNull("...SimpleFormatRegistry", null);

In Utility, I have
/\*\*
\* Utility for refactoring
\* @param className
\* @return A new instance of that class, or null if none available.
\*/
public static Object newInstanceOrNull(String className, Object fallback) {
try {
return (Class.forName(className)).newInstance();
} catch (Exception e) {
return fallback;
}
}
Here is the new interface.
// internal for now
public interface FormatRegistry {
/\*\*
\* Return a default Format for a given type of object.
\*
\* @param obj the input Object. For example, for an object of type Number, a
NumberFormat would be appropriate to return.
\* @param ulocale
\* @return default format, or null if none available for that type of object.
\*/
public abstract Format getFormatForObject(Class classType, ULocale ulocale);
/\*\*
\* Return a key, like "number", or "number,currency", or "number,#0,0#". If
\* that key were passed into getFormat (with the same uLocale), then a
\* format would be generated that would be equal to this one.
\*
\* @param format The format to generate a key for.
\* @param ulocale
\* @return
\*/
public abstract String getKey(Format format, ULocale ulocale);
/\*\*
\* From a key of the form mainType, subType, return a format.
\*
\* @param mainType
\* Guaranteed to be non-empty.
\* @param subType
\* May be empty or not. An empty subtype always works (if the mainType is
valid).
\* @param ulocale
\* @exception IllegalArgumentException
\* thrown if the mainType is not valid, or or the subType
\* invalid for the mainType.
\* @return
\*/
public abstract Format getFormat(String mainType, String subType, ULocale
ulocale);
}
