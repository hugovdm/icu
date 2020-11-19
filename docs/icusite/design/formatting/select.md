# SelectFormat

### New format type "select" in MessageFormat

The MessageFormat class will recognize a new format type "select". Just like for
the format types "choice" and "plural", the format type is interpreted by
creating a SelectFormat object from the subpattern string that's part of the
element format definition.

### New class SelectFormat

The SelectFormat class is very similar to the PluralFormat class, with the
following differences:

*   The pattern string for SelectFormat allows user-defined keywords that match
    the pattern \[a-zA-Z\]\[a-zA-Z0-9_-\]\* as well as the predefined keyword
    "other". The keyword "other" must occur in the pattern string, or the error
    U_DEFAULT_KEYWORD_MISSING is returned. For comparison, the PluralFormat
    pattern allows as keywords only the set of tags defined in UTR 35, Unicode
    Locale Data Markup, section C. 11, Language Plural Rules.
*   The SelectFormat.format method accepts only a string, and compares this
    string directly against the keywords found in the pattern. If the string
    matches one of the keywords, the subpattern following this keyword is used;
    otherwise the subpattern following the keyword "other" is used. For
    comparison, the PluralFormat.format method accepts only numbers, and maps a
    number to one of the tags defined in UTR 35 following locale-specific rules
    before matching the resulting tag against the keywords used in the pattern.

The API of the SelectFormat class is provided below.

### Use cases for select format

The main use case for the select format is gender based inflection. When names
or nouns are inserted into sentences, their gender can affect pronouns, verb
forms, articles, and adjectives. Special care needs to be taken for the case
where the gender cannot be determined. The impact varies between languages:

*   English has three genders, and unknown gender is handled as a special case.
    Names use the gender of the named person (if known), nouns referring to
    people use natural gender, and inanimate objects are usually neutral. The
    gender only affects pronouns: "he", "she", "it", "they".
*   German differs from English in that the gender of nouns is rather arbitrary,
    even for nouns referring to people ("Mädchen", girl, is neutral). The gender
    affects pronouns ("er", "sie", "es"), articles ("der", "die", "das"), and
    adjective forms ("guter Mann", "gute Frau", "gutes Mädchen").
*   French has only two genders; as in German the gender of nouns is rather
    arbitrary – for sun and moon, the genders are the opposite of those in
    German. The gender affects pronouns ("il", "elle"), articles ("le", "la"),
    adjective forms ("bon", "bonne"), and sometimes verb forms ("allé",
    "allée").
*   Polish distinguishes five genders (or noun classes), human masculine,
    animate non-human masculine, inanimate masculine, feminine, and neuter.

Some other languages have noun classes that are not related to gender, but
similar in grammatical use. Some African languages have around 20 noun classes.
To enable localizers to create sentence patterns that take their language's
gender dependencies into consideration, software has to provide information
about the gender associated with a noun or name to MessageFormat. Two main cases
can be distinguished:

*   For people, natural gender information should be maintained for each person.
    The keywords "male", "female", "mixed" (for groups of people) and "unknown"
    are used.
*   For nouns, grammatical gender information should be maintained for each noun
    and per language, e.g., in resource bundles. The keywords "masculine",
    "feminine", and "neuter" are commonly used, but some languages may require
    other keywords.

The resulting keyword is provided to MessageFormat as a parameter separate from
the name or noun it's associated with. For example, to generate a message such
as "Jean went to Paris", three separate arguments would be provided: The name of
the person as argument 0, the gender of the person as argument 1, and the name
of the city as argument 2. The sentence pattern for English, where the gender of
the person has no impact on this simple sentence, would not refer to argument 1
at all:

{0} went to {2}.

The sentence pattern for French, where the gender of the person affects the form
of the participle, uses a select format based on argument 1:

{0} est {1, select, female {allée} other {allé}} à {2}.

Patterns can be nested, so that it's possible to handle interactions of number
and gender where necessary. For example, if the above sentence should allow for
the names of several people to be inserted, the following sentence pattern can
be used (with argument 0 the list of people's names, argument 1 the number of
people, argument 2 their combined gender, and argument 3 the city name):

{0} {1, plural, one {est {2, select, female {allée} other {allé}}} other {sont
{2, select, female {allées} other {allés}}}} à {3}.

The introduction of select formats and plural formats, and the possibly nested
use of these formats, may result in rather complex message patterns. We have
discussed this with localization vendors. They understand that the alternative,
breaking down all the cases into separate individual strings, is not scalable.
They believe that they can handle the syntax, although they were concerned about
the ability of non-professional translators to handle it, and suggested
validation
in localization tools. They believe that developer's notes for the strings
explaining variables and possible values of them will address most of the issues
they're having.
SelectFormat can also be used to handle article [elision in
French](http://en.wikipedia.org/wiki/Elision_in_the_French_language) and a few
other languages, where an article is contracted if the (immediately following)
noun starts with a vowel or silent "h": "l’ami", "l’amie", "l’hache". To handle
elision in addition to gender, the set of keywords to categorize nouns can be
extended to "feminine", "masculine", "feminine-vowel", "masculine-vowel". A
sample pattern would be:

{1, select, feminine {La } masculine {Le } other {L’}}{0} est {1, select,
feminine {petite} feminine-vowel {petite} other {petit}}.

### Open Design Issues

Should the keyword for the default case be "other" or "default"? Engineers are
more used to "default", but PluralFormat set a different precedent by using
"other".
Should there be an API to return the set of keywords supported by a SelectFormat
instance? This might help in the construction of a tool to collect and vet the
information used by the SelectFormat. Parsing the pattern directly can be
difficult because of pattern nesting and the painful quote rules for some
formats. However, we'd have a better chance of getting the API supporting such a
tool right if somebody actually worked on it.

### SelectFormat C++ API

`SelectFormat (const UnicodeString &pattern, UErrorCode &status)`
Creates a new SelectFormat for a given pattern string.
`SelectFormat (const SelectFormat &other)`
Copy constructor.
`virtual ~ SelectFormat ()`
Destructor.
`void applyPattern (const UnicodeString &pattern, UErrorCode &status)`
Sets the pattern used by this select format.
`UnicodeString &toPattern (UnicodeString &appendTo)`
Returns the pattern from applyPattern() or constructor().
`UnicodeString &format (const UnicodeString &keyword, UnicodeString &appendTo,
FieldPosition &pos, UErrorCode &status) const`
Formats a select message for a given keyword.
`UnicodeString &format (const Formattable &obj, UnicodeString &appendTo,
FieldPosition &pos, UErrorCode &status) const`
Redeclared Format method.
`virtual void parseObject (const UnicodeString &source, Formattable &result,
ParsePosition &parse_pos) const`
This method is not supported by SelectFormat.
`SelectFormat &operator= (const SelectFormat &other)`
Assignment operator.
`virtual UBool operator== (const Format &other) const`
Return true if another object is semantically equal to this one.
`virtual UBool operator!= (const Format &other) const`
Return true if another object is semantically unequal to this one.
`virtual Format *clone (void) const`
Clones this Format object polymorphically.
`virtual UClassID getDynamicClassID () const`
ICU "poor man's RTTI", returns a UClassID for the actual class.
`static UClassID getStaticClassID (void)`
ICU "poor man's RTTI", returns a UClassID for this class.

### SelectFormat Java API

`SelectFormat(String pattern)`
Creates a new SelectFormat for a given pattern string.
`void applyPattern(String pattern)`
Sets the pattern used by this select format.
`String toPattern() `
Returns the pattern for this SelectFormat.
`StringBuffer format(Object keyword, StringBuffer toAppendTo, FieldPosition pos)
`
Selects the phrase for the given keyword.
`String format(String keyword) `
Selects the phrase for the given keyword.
`Object parseObject(String source, ParsePosition pos) `
This method is not supported by SelectFormat.
`boolean equals(Object obj)`
Indicates whether some other object is "equal to" this one.
`int hashCode()`
Returns a hash code value for the object.
`String toString() `
Returns a string representation of the object.
