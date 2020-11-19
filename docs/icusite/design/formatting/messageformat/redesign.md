# Message Formatting Redesign

## Goals

*   Modularization: We want to be able to offer formatting without static
    dependencies on the majority of ICU.
    *   We should have simple formatting support in the ICU4C common library,
        with just string substitution and "select" formatting, as a basic
        formatting API and for Locale Display Names and similar which use CLDR
        patterns like "{0} {1}" for the ordering of items.
    *   We should have formatting with numeric support, adding support for
        "number", "choice" and "plural" argument types.
    *   Finally, full support.
*   Threading: Significant objects and methods should be thread-safe, so that
    objects can be cached and used concurrently.
*   Performance: It would be nice if it was fairly fast to format a string with
    arguments without remembering context. It should be possible to improve
    speed with caching, but one-shot usage should be ok.
    *   For example, applying choice/plural/select formats should not require
        re-parsing their selected message fragments.
*   Extensibility: Currently, the interaction between MessageFormat and
    Choice/Plural/SelectFormat is hardcoded and relies on RTTI. It should be
    possible to add another such argument formatter without modifying the core
    formatting class.

## Ideas

### Message class

***==Superseded by==* icu-design 2011-02-08 00:59: [ICU4J API \*pre\*-proposal:
MessagePattern
class](http://sourceforge.net/mailarchive/message.php?msg_id=27024895)**

A parser of the [basic MessageFormat
syntax](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/MessageFormat.html),
and container for the parsed message representation. It would parse the whole
message string, remove the syntax it recognizes (quotes and braces, some
whitespace) and build a simple data structure for efficient usage.

*   setAutoQuoteApostrophe(boolean mode)
*   parse(CharSequence msg\[, UErrorCode\])
*   is-a Freezable -- cache-friendly and thread-safe once frozen
*   String getParsedString() -- syntax removed, usable only with indexes
*   int countIndexes()
*   int getIndex(int i)
*   (possibly helpers like int findEndOfArgument(int startIndex))

Design for indexes: (designed for easy processing)

*   The indexes would be bit fields, with bits 24..0 for the actual index into
    the parsed string. (This "limits" parsed message strings to a length of
    0x1ffffff.) The indexes are stored in ascending order.
*   Bits 26..25:
    *   0: Start of argument name/number
    *   1: Start of argument type
    *   2: Start of argument style (pattern)
    *   3: End of argument
    *   **Issue:** We need to allow nested {fragments} that do not use argument
        syntax, and distinguish them from arguments.
        How does the parser distinguish a {fragment} like in plural and select
        formats from an {argument}, without the parser knowing details of the
        subformat style syntax? Would the parser have to be limited to just
        strip '{} and note where the {} were with nesting levels, but without
        preparsing argument numbers/names/types?
*   Bits 30..27: Nesting level 0..15, increases at each { and decreases at each
    }.
*   Bit 31: Sign bit, not set.

Parsed message string:

*   Quotes (apostrophes) removed except where they were quoted. Text is always
    literal.
*   Syntactic (unquoted) braces removed. Arguments are found via the indexes.
*   Argument numbers are preparsed and stored as a UChar 0..0x3f. (This "limits"
    the number of numerically indexed arguments to 64.)
*   Argument names are trimmed and checked for \[:ID_START:\]\[:ID_CONTINUE:\]\*
    syntax.
*   Argument types are trimmed.

Example:

Original message string: "The {0} '{}' chased {1, select, female {her} other
{his}} tail."

Parsed message string: "The \\u0000 {} chased ==\\u0001select female her other
his== tail."

Indexes: 1+start+4, 1+end+5, ==1+start+16, 1+type+17, 1+style+23, 2+start+31,
2+end+34, 2+start+41, 2+end+44, 1+end+44==

### Formatter class

A formatter has-a Message and formats it, invoking argument formatters on the
way. Recursion eliminates the MessageFormat-Choice/Plural/Select RTTI magic.

**Design choice:**

1.  **We could provide Formatter subclasses** that know about the levels
    (simple/with numbers/all) of argument formatters. It would be customizable
    by writing additional subclasses.
    Pro: This is easy to implement.
    Con: You have to use a formatter that supports the argument types in your
    message.
    Con: (minor) Requires writing a Formatter subclass for supporting new
    argument types. (Probably very few users would provide argument formatters
    beyond what ICU provides.)
2.  **We could use registration:** Register an argument type with an argument
    formatter factory. The factory would take locale, type and style and return
    an argument formatter.
    Pro: After registration, the same Formatter class would be able to handle
    all registered argument types.
    Con: When would we do the registration in C++? Would we require users to
    call one of several functions to register some bundle of argument
    formatters?
    Con: Argument formatter access would be through a synchronized registry
    cache getter.

Public Formatter API: (not thread-safe; making this immutable seems limiting, or
seems to require a worker object)

*   Formatter(Locale locale\[, UErrorCode\])
*   aliasMessage(Message msg) -- uses the Message but does not take ownership of
    it
*   ?parseMessageString(CharSequence\[, UErrorCode\]) -- convenience?
*   (format() methods would be non-const, setting the args into the Formatter
    object, which will probably also have a cache of specific argument
    formatters like MessageFormat does)
*   format(Appendable dest, Object... args)
*   format(Appendable dest, Map namedArgs)

Protected Formatter API (if we use subclasses):

*   Object getArgument(int argNumber)
*   Object getArgument(String argName)
*   format(Appendable dest, Message msg, int styleStart /\* use
    msg.getIndex(styleStart) \*/, int argNumber, String argType)
*   format(Appendable dest, Message msg, int styleStart /\* use
    msg.getIndex(styleStart) \*/, String argName, String argType)
*   format(Appendable dest, Message msg, int msgStringStart, int msgStringLimit)
    -- for recursive calls from choice, and called with (0,
    msg.getParsedString().length()) from the public format() methods
*   format(Appendable dest, Message msg, int nestedIndex) -- for recursive calls
    from plural/select

### NumberFormat

NumberFormat should have an optimization for small integers. It could cache a
limit value such that formatting numbers below the limit is simple and fast. For
example, if the NumberFormat uses decimal digits from somewhere on the BMP, the
limit could be 1000 for standard grouping (for -1000<intValue<1000), or much
larger if the grouping separator is not used. Formatting a below-limit integer
should use very fast code.

### Formattable

In C++, we will have to move Formattable to the common library. (In Java, the
arguments are passed in as Object.) Check if we can do so without dragging
formatting code with it.

## Related Tickets

Mark wrote:

There are also some syntactic items that I think we should fix as a part of
this, in:

<http://bugs.icu-project.org/trac/ticket/7510>

The following are relevant other bugs that should be looked at. Most should be
fixed by your proposal (I think).

<http://bugs.icu-project.org/trac/ticket/7165>

<http://bugs.icu-project.org/trac/ticket/7048>

<http://bugs.icu-project.org/trac/ticket/7618>

<http://bugs.icu-project.org/trac/ticket/7682>

<http://bugs.icu-project.org/trac/ticket/7691>

<http://bugs.icu-project.org/trac/ticket/7457>

<http://bugs.icu-project.org/trac/ticket/2282>

<http://bugs.icu-project.org/trac/ticket/6563>

<http://bugs.icu-project.org/trac/ticket/7181>

<http://bugs.icu-project.org/trac/ticket/7860>

The following should be done as part of the Plural work

<http://bugs.icu-project.org/trac/ticket/6858>

<http://bugs.icu-project.org/trac/ticket/6881>

I think the following are done already.

<http://bugs.icu-project.org/trac/ticket/2322>

<http://bugs.icu-project.org/trac/ticket/4569>

<http://bugs.icu-project.org/trac/ticket/4510> // if the calendar keyword is in
the locale?

Should I go ahead and add the above to a section of the doc?
