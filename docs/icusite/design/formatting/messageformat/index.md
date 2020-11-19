# MessageFormat

Here is a rough draft for restructuring MessageFormat for modularity.

Clean up the inheritance

1.  Add new interfaces Parser and Formatter
2.  Have Format descend from both Parser and Formatter
3.  Where it makes sense, change APIs that take a Format as an input parameter
    to take either a Parser or a Formatter
4.  Where it makes sense, split a Format subclass (like NumberFormat) into a
    formatter and a parser, but keep the old class have have it delegate.
5.  Add a TimeZoneParser and TimeZoneFormatter, separating them so that we don't
    have to build up all the parsing data structures.
6.  For Java, it is a bit more complicated: maybe have UFormat descend from
    Format, Parser, and Formatter

Restructure MessageFormat

1.  Structure the code so that when it hits {0,XXX} in the message string, it
    calls a registry to get a formatter (or parser, when parsing) to handle XXX.
2.  We add API to register formatters and parsers along the lines of other
    registries we already have.
3.  The registry successively calls the registered factories with XXX until it
    hits a match.
4.  If there is no match, and XXX consists of YYY(,ZZZ)?, then try to
    instanciate a class MessageFormatFactory_YYY (where the YYY is cleaned up by
    lowercasing and trimming whitespace). For example, {0, Number, ###.0} will
    cause the instanciation of MessageFormatRegister_number.
5.  If the instanciation fails, we mark that so we never try again.
6.  If the instanciation succeeds, the class is created and its register()
    method is called. That method calls the registration APIs to add formatters
    and or parsers.
7.  If after doing all of this, there is no match for {0,XXX}, then we return an
    error. (Alternatively, we could have a setting that defaults the behavior.)
