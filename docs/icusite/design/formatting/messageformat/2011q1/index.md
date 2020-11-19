# MessageFormat 2011q1

For ICU 4.8, we want to tackle some of the MessageFormat (and related) work that
has piled up. For more ideas and things to do see the [Message Formatting
Redesign](../redesign.md) page and the common parent page.

*   Simple message pattern string parser as separate, public API
*   Use the message pattern parser in the implementation of MessageFormat,
    ChoiceFormat, PluralFormat, and SelectFormat.
*   ~~We probably want to add an ICU version of ChoiceFormat in ICU4J.~~
*   Fix quoting and apostrophe handling.
*   Fix syntax and update docs. In particular, define where whitespace is
    allowed, what set of characters in argument names and plural form keys,
    syntax of argument numbers and plural offsets (only ASCII digits?), etc.
*   Java: Probably need to do something about compatible serialization.
*   PluralRules used in MessagePattern? Replace with simple interface for
    mapping double->String?
    *   No, moved dependency on PluralRules out of MessagePattern, into
        PluralFormat.
*   Is MessageFormat.format() supposed to be thread-safe?
    *   No, documented as not being thread-safe.

## Status

Implemented in ICU 4.8. For a summary of benefits and incompatible changes, see
the [ICU 4.8 download](../../../../download/48.md) page. (That page refers back
here and to subpages for details.)

## Demo

See the
[MessagePatternDemo](http://bugs.icu-project.org/trac/browser/icu4j/trunk/demos/src/com/ibm/icu/dev/demo/messagepattern/MessagePatternDemo.java)
class and the
[MiniMessageFormatter](http://bugs.icu-project.org/trac/browser/icu4j/trunk/demos/src/com/ibm/icu/dev/demo/messagepattern/MiniMessageFormatter.java)
it calls.
For output from the MessagePatternDemo see [this subpage](msgpatdemo.md).

## Docs

[MessageFormat syntax currently in the API
docs](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/MessageFormat.html)

[MessageFormat in the User
Guide](http://userguide.icu-project.org/formatparse/messages)

## Proposals & discussions

**See also the [Questions & Decisions](decisions.md) sub-page.**

icu-design 2011-02-08 00:59: [**ICU4J API \*pre\*-proposal: MessagePattern
class**](http://sourceforge.net/mailarchive/message.php?msg_id=27024895)

icu-design 2010-08-19 23:24: [need stronger form of
MessageFormat.autoQuoteApostrophe()](http://sourceforge.net/mailarchive/message.php?msg_id=25988192)

icu-design 2010-08-24 00:26: [ICU API proposal: MessageFormat with
autoQuoteApostrophe() behavior by
default](http://sourceforge.net/mailarchive/message.php?msg_id=26011900)

... branched
[here](http://sourceforge.net/mailarchive/message.php?msg_id=26018753)

... including on 2010-08-25 19:24:

We discussed this in the ICU team meeting today, and reached the following
consensus: - Change the default parsing behavior but implement both variants. -
In Java, the old behavior can be chosen via an ICUConfig class backed by a
.properties file. (Essentially at build time.) - In C++, the old behavior can be
chosen via a #define to uconfig.h . (Also at build time.)

icu-design 2010-08-27 00:28: [modify
MessageFormat.autoQuoteApostrophe()?](http://sourceforge.net/mailarchive/message.php?msg_id=26035578)

icu-design 2010-08-26 21:25: [ICU PluralFormat: not possible to have # in
fragment?](http://sourceforge.net/mailarchive/message.php?msg_id=26034771)

icu-design 2010-07-27 20:23: [proposal: extend plural format
syntax](http://sourceforge.net/mailarchive/message.php?msg_id=25827762)

## setFormat() etc.

**If we could get rid of MessageFormat.setFormat() & siblings** (deprecate, and
throw UnsupportedOperationException), then we need to fix or replace several
internal uses... **which proved non-trivial**, especially because such
MessageFormat objects are used not only for formatting (where a preformatted
string could have been passed in rather than using a custom Format object) but
also for parsing. **We [decided](decisions.md) that we needed to implement
setFormat() etc., and it was not too hard.**

We did remove one feature that already did not work well: We dropped support for
toPattern() when custom formats have been set, because there is no good way in
general to construct a pattern string. See the following ideas for some
discussion.

**If we had to implement setFormat() etc. with toPattern() support**, then here
are some ideas:

*   Add methods to the base class (Java: com.ibm.icu.text.UFormat; C++: Format)
    to support MessageFormat.toPattern(). For example:
    *   toPatternStyle() -- for the argument style (different from regular
        toPattern() in that it returns "medium" and similar if applicable)
    *   toPatternType() -- for the argument type
*   Add a MessagePattern.ArgType CUSTOM or similar which triggers special
    processing.
*   We could pretty certainly get away with requiring that only a NONE or CUSTOM
    argument can be replaced with a CUSTOM one. (But it might not matter.)

## Tickets

Umbrella ticket: [#8319](http://bugs.icu-project.org/trac/ticket/8319):
MessageFormat 2011q1

[#2322](http://bugs.icu-project.org/trac/ticket/2322): Add string ids to Message
Format (this got done in 2007, but review the API ideas here)

*   resolved as fixed; named arguments supported since ICU 3.8, ticket #5792

[#5904](http://bugs.icu-project.org/trac/ticket/5904): Implement
PluralFormat#formatToCharacterIterator

*   added comment pointing to recent decision not to write an attribute for the
    plural #, reduced priority to zero

[#6306](http://bugs.icu-project.org/trac/ticket/6306): C wrapper for PluralRules
and PluralFormat.

*   Peter (Apple) is adding a PluralRules C wrapper with ticket #8467;
    cross-referenced the tickets; we probably don't need a PluralFormat C
    wrapper (use umsg_xxx())

[#6466](http://bugs.icu-project.org/trac/ticket/6466): PluralFormat special case
for "zero" (superseded by #7858)

*   marked as "wontfix" with cross-reference to ticket #7858

[#6563](http://bugs.icu-project.org/trac/ticket/6563): ICU4J MessageFormat
allocates a big int\[\] on the first call

*   done

[#6858](http://bugs.icu-project.org/trac/ticket/6858): Use of PluralFormat is
poorly documented

*   done by Claire; more doc updates by Markus in ticket #8319

[#6881](http://bugs.icu-project.org/trac/ticket/6881): Issue: PluralFormat
interoperability with NumberFormat (MessageFormat)

*   rejected as "worksforme" (since this is an existing, working unit test) with
    explanation

[#6985](http://bugs.icu-project.org/trac/ticket/6985): PluralFormat should
return error if any mismatch between pattern and locale.

*   rejected as "needsmoreinfo"

[#7048](http://bugs.icu-project.org/trac/ticket/7048): Named arguments feature
in MessageFormat is not well documented

*   done

[#7165](http://bugs.icu-project.org/trac/ticket/7165): introduce MessageFormat
base class: only string substitution

*   made unscheduled, with comment

[#7181](http://bugs.icu-project.org/trac/ticket/7181): autoQuoteApostrophe
doesn't work in nested subformats

*   closed as "fixed" with comments

[#7457](http://bugs.icu-project.org/trac/ticket/7457): move LocaleDisplayNames
class (C++) to the common library once we have MessageFormatBase there

*   postponed

[#7510](http://bugs.icu-project.org/trac/ticket/7510): Fix remaining problems in
MessageFormat (getFormats(), setFormat(), setFormatByArgumentIndex(),
setFormatByArgumentName(), getFormatsByArgumentIndex(), named vs. numeric
arguments, ease of calling with named arguments)

*   responded, mostly done via tickets #8095 & #8319

[#7575](http://bugs.icu-project.org/trac/ticket/7575): selectFormat docs should
move to userguide

*   done

[#7618](http://bugs.icu-project.org/trac/ticket/7618): Use of Simple
MessageFormat alternative in ICU implementation in ICU4J

*   postponed

[#7682](http://bugs.icu-project.org/trac/ticket/7682): MessageFormat::format and
argument inconsistency

*   rejected as "wontfix" (behavior is specified in JDK MessageFormat API docs)

[#7691](http://bugs.icu-project.org/trac/ticket/7691): add Format method to
describe itself for MessageFormat::toPattern()

*   added comment pointing to recent decision not to return a pattern for custom
    formats, reduced priority to zero

[#7858](http://bugs.icu-project.org/trac/ticket/7858): Extend PluralFormat
syntax with offset and explicit values

*   done

[#7860](http://bugs.icu-project.org/trac/ticket/7860): ideas for better
PluralFormat/MessageFormat performance

*   more or less done

[#7904](http://bugs.icu-project.org/trac/ticket/7904): MessageFormat should
allow and trim whitespace around argument number/name

*   done

[#7905](http://bugs.icu-project.org/trac/ticket/7905):
MessageFormat.applyPattern() can fail on a Turkish system

*   done

[#7917](http://bugs.icu-project.org/trac/ticket/7917): PluralFormat replaces
quoted # signs

*   done

[#7918](http://bugs.icu-project.org/trac/ticket/7918): ICU4J MessageFormat
should not use java.text.ChoiceFormat

*   done

[#7938](http://bugs.icu-project.org/trac/ticket/7938): umsg.h documentation out
of sync with class MessageFormat

*   done

[#8095](http://bugs.icu-project.org/trac/ticket/8095): MessageFormat class
provide method to get argument names

*   TODO: proposed

[#8106](http://bugs.icu-project.org/trac/ticket/8106): ICU4J MessageFormat needs
performance improvement

*   marked as "needsmoreinfo", might come back

[#8191](http://bugs.icu-project.org/trac/ticket/8191): getArgTypeList should
have same behavior with named/numbered argument pattern.

*   rejected: getArgTypeList() can only work for numbered arguments

[#8245](http://bugs.icu-project.org/trac/ticket/8245): getFormatNames should get
the name but not

*   misunderstanding, rejected as "worksforme"

[#8325](http://bugs.icu-project.org/trac/ticket/8325): bug in the
MessageFormat::getFormat(const UnicodeString& formatName, UErrorCode& status)

*   done
