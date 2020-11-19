# Questions & Decisions

SelectFormat.applyPattern(String pattern)

*   ICU 4.6: Throws an IllegalArgumentException("Duplicate keyword error.") if
    there are duplicate keywords.
*   **Proposal: Don't test for duplicates**, just return the sub-message for the
    first matching keyword.
*   Rationale: Saves time, especially in one-shot use case, and saves creation
    of a HashSet or HashMap.
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

SelectFormat.applyPattern(String pattern)

*   ICU 4.6: Limits keywords to ASCII alphanumeric identifiers:
    \[a-zA-Z\]\[a-zA-Z0-9\]\*
    *   Pro: Simple, stable, might be sufficient.
    *   Con: Seems a little limited.
*   Note: The keyword syntax should be the same for SelectFormat & PluralFormat
    (see below).
*   Note: Argument names are Unicode identifiers (see below). Not sure whether
    they should be consistent with plural/select keywords.
*   Question: Should we support a wider set of keywords?
*   For example: \[^\[\[:Pattern_Syntax:\]\[:Pattern_White_Space:\]\]\]+
    *   Pro: More lenient, still stable, still does not require properties data.
    *   Con: Slightly more expensive to test than just ASCII.
*   Alternative: Support all Unicode identifiers
    \[:ID_Start:\]\[:ID_Continue:\]\*
    *   Pro: Common Unicode identifier syntax.
    *   Con: These properties are not immutable and therefore cannot be
        hardcoded as fixed data. And an older ICU version (with older Unicode
        data) will reject identifiers from a newer Unicode version.
*   **Proposal: Use Unicode "pattern identifiers":**
    \[^\[\[:[Pattern_Syntax](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B:Pattern_Syntax:%5D&abb=on&g=):\]\[:[Pattern_White_Space](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B:Pattern_White_Space:%5D&abb=on&g=):\]\]\]+
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

PluralFormat.applyPattern(String pattern)

*   ICU 4.6: Keywords are delimited by optional \[:Pattern_White_Space:\]\* and
    then '{', but later rejected if they are not in the set of PluralRules
    keywords for the locale.
*   Background: The MessagePattern class will be used for parsing. It is
    locale-independent and cannot verify PluralRules keywords. Testing for
    whether keywords are in a particular set costs a bit of time.
*   Note: While normally only 6 predefined plural keywords are used, the
    PluralRules class technically supports Unicode identifier syntax, plus '_'
    in initial position: \[\[:ID_Start:\]\[_\]\]\[:ID_Continue:\].
    *   Bug in PluralRules.isValidKeyword(String token): It uses charAt() rather
        than codePointAt(), so its identifier syntax is limited to BMP
        characters.
*   **Proposal:**
    *   (a) Make the MessagePattern class test PluralFormat keywords for the
        same syntax as in SelectFormat, see above (ASCII vs. Pattern vs.
        Unicode).
        *   **Proposal: Use Unicode "pattern identifiers":**
            \[^\[\[:Pattern_Syntax:\]\[:Pattern_White_Space:\]\]\]+
    *   (b) **Do not test** whether the keywords are in some PluralRules set.
        (They just won't match if they aren't.)
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

MessageFormat.applyPattern(String pattern)

*   ICU 4.6: Argument names (when not numbers) are Unicode identifiers
    \[:ID_Start:\]\[:ID_Continue:\]\*
*   Question: Should they be consistent with plural/select keywords?
*   **Proposal: Use Unicode "pattern identifiers":**
    \[^\[\[:Pattern_Syntax:\]\[:Pattern_White_Space:\]\]\]+
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

MessageFormat.applyPattern(String pattern)

*   ICU 4.6: In Java, argument numbers must not contain whitespace. That is, "{
    0}" and "{1 ,number}" are rejected (by Integer.parseInt()).
*   However, in C++, Pattern_White_Space anywhere in the argument number field
    is allowed and ignored (by stou()).
*   In both Java and C++, the type and style fields allow leading and trailing
    whitespace which is ignored (unless in the style field if it's not one of
    the predefined keywords like "medium"). So "{2, number , medium }" is valid.
*   **Proposal:** Consistently allow leading and trailing Pattern_White_Space
    only.
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

MessageFormat.applyPattern(String pattern)

*   ICU 4.6: Argument numbers can use any \[:Nd:\] digits, via
    Integer.parseInt().
*   However, if an argument number string differs from the Integer.toString() of
    that number (leading zeros, leading plus sign, non-ASCII digits), then
    format(Object\[\]) fails to match that argument number string to any input
    value.
*   format(Map<String, Object>) fails to match an argument if the string is not
    the same, even if the Integer.parseInt() results of the argument number and
    the map key would match.
*   JDK MessageFormat documents: "The *ArgumentIndex* value is a non-negative
    integer written using the digits '0' through '9'".
*   **Proposal: Only allow ASCII digits** in argument numbers (i.e., in argument
    name/number strings that start with a digit).
*   Decision: Proposal accepted in modified form (ICU meeting 2011-feb-16):
    *   While parsing an argument name/number, collect a "pattern identifier".
    *   Trim leading/trailing Pattern_White_Space.
    *   If the identifier contains only ASCII digits, then it is an argument
        ==number== and must not have leading zeros (except "0" itself).
    *   Otherwise it is an argument ==name==.
*   *Implemented: Java*

MessageFormat.applyPattern(String pattern)

*   ICU 4.6: Forbids mixing named and numbered arguments, although the
    formatting and other code is written to apply named values to numbered
    pattern arguments.
*   **Proposal: Allow mixing named and numbered arguments**, and just return
    true from usesNamedArguments() in those cases.
*   Decision: Proposal accepted (ICU meeting 2011-feb-16)
*   *Implemented: Java*

SelectFormat.format(String keyword)

*   ICU 4.6: Throws an IllegalArgumentException("Invalid formatting argument.")
    if the keyword is not ASCII alphanumeric. Seems unnecessary.
*   **Proposal: Remove this test**, and just return the "other" sub-message when
    there is no match.
*   Decision: Proposal rejected (ICU meeting 2011-feb-16). Continue to validate
    keywords (but now as "pattern identifiers" to match the parser).
*   *Implemented: Java*

MessageFormat.setFormat(int, Format) and siblings (setFormatByXyz(),
getFormats(), ...)

*   ICU 4.6: After applyPattern(), these methods modify the resulting, internal
    array of Format objects.
    *   For example, if format is a short-style SimpleDateFormat, then
        applyPattern("Level at {3,number,percent}").setFormat(0,
        format).toPattern() becomes "Level at {3,date,short}".
*   These methods are hard to implement, or force inefficient data structures
    and code.
    *   They only really work with an internal data structure equivalent to the
        current one.
    *   There is no good way to implement MessageFormat.toPattern() with these.
        (Just take a look at the code...!)
    *   An efficient, simple implementation (e.g., based on MessagePattern) does
        not need to instantiate any ChoiceFormat/PluralFormat/SelectFormat
        objects for proper message formatting, except artificially to satisfy a
        getFormat() method.
*   These methods have broken semantics and implementation.
    *   It seems really strange to construct a MessageFormat from a pattern and
        then to override some of the parsed, internal data structure.
    *   It is impossible to localize/translate a sub-format that is provided in
        runtime code, and it is impossible to localize a sub-format (e.g.,
        ChoiceFormat) because its text snippets lack the necessary context. But
        localization is the main purpose for MessageFormat.
    *   The old (JDK 1.1) setFormat() etc. take an index that refers to the
        parsing order. With a localized string, the arguments are often
        rearranged, and the parsing order differs. (The ones "ByArgumentIndex"
        are only @since 1.4 and the ones "ByArgumentName" are ICU-specific and
        only exist to be parallel with "ByArgumentIndex".)
    *   Format objects returned by getFormats() etc. are not cloned and can be
        modified, changing the behavior of the MessageFormat object that
        returned them.
    *   All of these methods only work on top-level arguments. Arguments in
        nested MessageFormat objects are not visible.
        *   In the current implementation, such nested MessageFormat objects do
            not even exist except as temporary objects while formatting the
            result of a choice/plural/select argument.
        *   For example, applyPattern("ab {3} {2,select,other{cd{3}ef}}
            yz").setFormatByArgumentIndex(3, format) only affects the top-level
            {3}, not the nested {3}.
    *   ICU4J MessageFormat.toPattern() reconstitutes the message pattern via
        RTTI, testing for a small set of com.ibm.icu.text.xyz classes and their
        standard options. It does not test for equivalent JDK java.text.xyz
        classes, and it has no chance of doing anything useful with an unknown
        subclass of java.text.Format.
*   These are hardly used even inside ICU.
    *   "Even" where people might understand these APIs...
    *   There is one call to setFormat(int, Format) and one class with four
        calls to setFormatByArgumentIndex(int, Format). (Aside from test code.)
*   The usage example in the API docs is broken.
    *   ChoiceFormat does not work for plural formatting. (That's why we added
        PluralFormat.)
*   There are trivial workarounds for formatting.
    *   The canonical example of setting a ChoiceFormat can be expressed much
        simpler via a MessageFormat pattern string that contains the
        ChoiceFormat pattern.
    *   In general, simply use the sub-formatter object to format to a string,
        and then pass that string value into MessageFormat.
*   However, workarounds for parsing are not trivial. (Doable, but involve more
    thought and code.)
*   **Proposal: Deprecate** all forms of MessageFormat.setFormat() and
    getFormats(), and change them to just throw new
    UnsupportedOperationException().
*   Note: If we later discover that someone has an important use case for some
    of these, then we can un-deprecate and work on some implementation (and
    concentrate on those methods and use cases that are needed). Rather than
    spending a lot of time fully supporting questionable API.
*   Alternative: Implement most but not all of setFormat() etc.: For example,
    getFormat() return null for choice/plural/select arguments, and toPattern()
    always return the original pattern string.
*   Decision: Support setFormat() etc. ==but==: (ICU meeting 2011-feb-16)
    *   toPattern() will throw a
        [java.lang.IllegalStateException](http://download.oracle.com/javase/6/docs/api/java/lang/IllegalStateException.html)
        when custom formats have been set (it already could not write a usable
        pattern string in all cases)
    *   getFormats() etc. will return null for choice/plural/select arguments
        that have not been overridden (because the implementation should not
        have to construct those Format objects)
        *   Otherwise we would have to construct these objects in getFormats()
            etc., and for maximum compatibility we would ==*also have to set*==
            them at that time so that when the caller modifies the objects, the
            modifications are used in subsequent format()/parse() etc. calls.
*   *Implemented: Java*

MessageFormat implements Serializable

*   ICU 4.6: Supports serialization via standard readObject()/writeObject(),
    apparently backward-compatible at least a few versions. (Maybe back to when
    UFormat was inserted into the class hierarchy?)
*   It looks like this might be the hardest part of the "API" to be supported
    with a very different implementation. The MessageFormat class does not just
    read and write the message pattern string, it reads and writes whatever
    non-transient fields it has; trouble is, the old data structure would take a
    fair bit of convoluted code to turn into the new one.
*   At the same time, there seems to be no need to serialize these \*Format
    classes using Java Serializable, and there seems to be consensus that the
    Serializable mechanism was somewhat ill-conceived and applied to too many
    classes.
*   **Proposal**: Implement readObject() and writeObject() and just have them
    throw
    [java.io.**NotSerializable**Exception](http://download.oracle.com/javase/6/docs/api/java/io/NotSerializableException.html).
    (Yes, there is a nice, standard, JDK 1.1 Exception class for "when an
    instance is required to have a Serializable interface" but we don't actually
    want to implement it.)
*   If this is not ok:
    *   Is it ok to break serialization compatibility in ICU 4.8? (That is, ICU
        4.6/4.8 MessageFormat would not be able to deserialize each other.)
    *   Is it ok to not support serialization if setFormat() or one of its
        siblings has been called?
*   Yoshito writes 2011-feb-16:
    *   Breaking serialization backward compatibility in stable API class is not
        desired, but I personally can accept such breakage if this is well
        informed.
    *   I disagree to throw NotSerializableException always. I request -
        *   Change the value of serialVersionUID.
        *   readObject to check the serialVersionUID - if old one, throw an
            exception.
        *   Implement Serializable properly for ICU4J 4.8 and maintain
            serialization compatibility for 4.8 and beyond.
*   Decision: Agreed in icu-design email 2011-feb-17
*   *Implemented: Java* (serialization works as requested by Yoshito: new data
    format & new ID; custom Format objects are supported; incompatible
    before/after ICU 4.8)

MessageFormat.applyPattern(String pattern) & MessagePattern.parse(String
pattern)

*   The MessagePattern implementation imposes new limitations.
    *   Pattern string length 0..**0xfffff** (max. **1M** chars/UChars).
    *   Argument numbers limited to 0..**127**.
    *   Argument name lengths limited to 1..**127**.
    *   Argument type name lengths limited to 1..**127**.
    *   Choice/plural selector lengths limited to 1..**127**.
    *   Maximum number of numeric choice/plural values: **128**
    *   **Maximum depth of nested MessageFormat patterns: **128****
*   **Proposal: Accept** these limitations.
*   Decision: Proposal tentatively accepted (ICU meeting 2011-feb-16).
*   *Implemented: Java*
*   **Modified limitations: (Java branch r29636 2011-mar-11)**
    *   Pattern string length **Integer.MAX_VALUE**.
    *   Argument numbers limited to 0..**32767**.
    *   Argument name lengths limited to 1..**65535**.
    *   Argument type name lengths limited to 1..**65535**.
    *   **Argument style text lengths limited to 0..65535.**
    *   Choice/plural selector lengths limited to 1..**65535**.
    *   Maximum number of numeric choice/plural values: **32768**
    *   Maximum depth of nested MessageFormat patterns: **32768**

Stand-alone Choice/Plural/SelectFormat.applyPattern(String pattern)

*   ICU 4.6: The ChoiceFormat, PluralFormat, and SelectFormat classes have not
    been enforcing that the snippets they choose/select follow full
    MessageFormat syntax. For example, it was possible to create a stand-alone
    PluralFormat object with "odd{The number {#} is odd.}".
    *   This would not work as a nested pattern inside a MessageFormat because
        the resulting snippet "The number {#} is odd." would have failed being
        parsed for a temporary MessageFormat object.
*   My current work-in-progress code rejects such a pattern even in a
    stand-alone Choice/Plural/SelectFormat because it always requires that
    chosen/selected snippets are MessageFormat patterns. For example, while
    "odd{The number {#} is odd.}" is rejected, "odd{The number {1,number,#} is
    odd.}" works.
*   I intend to change this for better compatibility, that is, adding code to
    the new implementation to suppress full MessageFormat-syntax parsing in
    stand-alone Choice/Plural/SelectFormat objects. As far as I can tell, these
    just handle quoting/escaping and enforce that curly braces {} are used in
    matched pairs. I will take a closer look at the JDK ChoiceFormat behavior in
    particular.
*   Decision: Implement strict MessageFormat-syntax parsing (ICU meeting
    2011-mar-02)
*   *Implemented: Java*

MessageFormat.formatToCharacterIterator() vs. # plural number

*   Background:
    *   Java's Format.formatToCharacterIterator() writes a string with attribute
        meta data, such as which part of the result is a number fraction or, for
        a MessageFormat, which part of the result comes from which argument.
    *   For MessageFormat, the argument's result substring also has the
        attributes from the argument's formatter object.
*   In the MessageFormat reimplementation, we mostly recreate current behavior.
    However, one question came up:
    *   In a plural sub-message, a # is replaced with the formatted plural
        number. In ICU 4.6, neither MessageFormat nor PluralFormat wrote any
        attributes here. In the new code, the # will get at least a
        NumberFormat.Field attribute.
*   **Question**: Should we also write a new kind of attribute for the #
    replacement to make it look more like a simple {2} which gets both the
    MessageFormat.Field.ARGUMENT and the NumberFormat.Field(s)?
    *   If so, on which class should we declare the Field, what should it be
        called, and what attribute value should it come with?
    *   Right now, (on the development branch) we have a new MessageFormat.Field
        value of
    *   public static final Field PLURAL_NUMBER = new Field("plural number
        field");
    *   Value: We currently set this same Object as the attribute value.
        *   This is customary for Format.Field attributes, although
            MessageFormat.Field.ARGUMENT has the argument ID \[e.g., Integer(0)
            or "first_name"\] as the attribute value.
        *   In MessageFormat, we could set the plural argument ID, but if we
            wanted to write the attribute in PluralFormat as well, we would not
            have that information there.
*   **Proposal:** We could go either way, but, given that this API seems rarely
    used, it might be ok to not write an additional attribute at all, and *I
    think we should remove this from the reimplementation (on the development
    branch).*
    *   We should be able to add it later if needed.
*   Decision: Do not write such a special attribute for the # plural number.
    (ICU meeting 2011-mar-02)
*   *Implemented: Java (that is, withdrawn from dev. branch, r29514, 20110302)*

C++ ChoiceFormat internal-array getters; getLimits(), getClosures(),
getFormats() \[there is no ICU4J ChoiceFormat class\]

*   **Proposal:** Deprecate the ICU C++ ChoiceFormat getters which return the
    internal arrays, and set count=0/returning NULL.
*   Background: With the new MessageFormat/MessagePattern work, these getters
    which expose the old internal data structures are hard to implement. They
    are const and return arrays that are owned by the ChoiceFormat object.
*   Assertion: I don't see any reasonable use case for these getters. I expect
    most users to use MessageFormat pattern strings with embedded choice
    arguments, except that they should probably use plural or select arguments
    at this point. The old class API docs showed creating a separate
    ChoiceFormat and setting it into a MessageFormat object, although that would
    be translation-hostile, but even then I don't see why one would access the
    pieces of the just-built ChoiceFormat. Assuming that users build formatters
    and use them for formatting and parsing, any code and time to recreate the
    old internal arrays when no one will call the getters would be wasted.
*   Decision: Deprecate & disable. (ICU meeting 2011-apr-13)
*   *Implemented: C++*
