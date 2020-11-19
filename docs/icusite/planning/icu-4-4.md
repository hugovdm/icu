# ICU 4.4

[TOC]

## All

*   Unicode 5.2
*   CLDR 1.8
*   More compact data formats
*   ICU4J modularization

## Apple

### Tier 1

*   Regex using abstract text access APIs (UText), roll in work by Jordan Rose:
    [#4521](http://bugs.icu-project.org/trac/ticket/4521) (ensure perf OK,
    ensure UTF8 support)
*   Ensure that there are APIs providing access to all CLDR data: e.g.
    [#4836](http://bugs.icu-project.org/trac/ticket/4836),
    [#5478](http://bugs.icu-project.org/trac/ticket/5478), etc. (Google also
    interested) (Peter has CLDR task to enumerate the data that is missing;
    based on that we can file additional bugs and divide up the work)
*   Improved search capabilities (Peter to generate design doc) - mainly
    asymmetric search, i.e. type e, match e,é,è; type é, match é but probably
    not e and certainly not è
    (#[7093](http://bugs.icu-project.org/trac/ticket/7093)) (Google also
    interested). Other possibilities (lower priority) include:
    *   Position-dependent matching? (e.g. Arabic HEH and TEH MARBUTA should
        match for a search when both are at the end of a word)
    *   Use of search object distinct from collator? (Possible optimization, may
        not be necessary, not of interest to others)
*   Reduce ICU4C dynamically-allocated memory, especially for time zone data
    (more compact data formats may help with this):
    #[6873](http://bugs.icu-project.org/trac/ticket/6873),
    [#6879](http://bugs.icu-project.org/trac/ticket/6879) (Google also
    interested) (Peter will look at porting Yoshito's ICU4J work to C; requires
    interpreting const in a "logical" way - can do lazy loading, just make sure
    thread safe. Should document this interpretation. Peter to coordinate with
    Andy on this)

### Tier 2

*   Number spellout format & parse support for CJK numbers, including in dates.
    Note: CLDR 1.7 added relevant capability per
    [cldrbug:](goog_1250056180246)[1927](http://www.unicode.org/cldr/bugs/locale-bugs?findid=1927);
    is there anything else that needs to be done in ICU (may work if appropriate
    patterns are used, Peter will do some experiments)

    Support >2GB text length for search, regex, text break, encoding conversion,
    perhaps transliteration. Use of UText will provide appropriate interfaces
    for regex and RBBI with additional internal changes.
    #[5451](http://bugs.icu-project.org/trac/ticket/5451) is for the RBBI
    changes.

*   Encoding detection for a wider range of encodings, with some finer
    distinctions. For example pure ShiftJIS text should return both ShiftJIS and
    cp932 with 100% confidence; text including cp932 extensions should also
    return both but with lower confidence for ShiftJIS.
*   Additional conversion tables (not necessarily in default build). Don't need
    a ticket for this yet.

### Already implemented on branch

*   Lenient parsing for numbers
    (#[4942](http://bugs.icu-project.org/trac/ticket/4942),
    #[6109](http://bugs.icu-project.org/trac/ticket/6109)), date/time
    (#[3579](http://bugs.icu-project.org/trac/ticket/3579))

## Google

### Tier 1

*   General
    *   Spoof detection in ICU4J
        ([#7094](http://bugs.icu-project.org/trac/ticket/7094)) (IBM has also
        expressed interest)
    *   Complete BCP47 Support
        ([6177](http://bugs.icu-project.org/trac/ticket/6177),
        [6916](http://bugs.icu-project.org/trac/ticket/6916),
        [6791](http://bugs.icu-project.org/trac/ticket/6791),
        [6860](http://bugs.icu-project.org/trac/ticket/6860))
    *   Switch data formats to use UTrie2— requires UTrie2 in ICU4J as well
        ([#7077](http://bugs.icu-project.org/trac/ticket/7077))
    *   (apple) Provide APIs to access all CLDR data
        [#4836](http://bugs.icu-project.org/trac/ticket/4836),
        [#5478](http://bugs.icu-project.org/trac/ticket/5478)
    *   Fully customizable data for objects, formatters, and others; there are
        holes
*   Normalization/IDNA
    *   Support custom normalization data
        ([#7273](http://bugs.icu-project.org/trac/ticket/7273))
    *   Support UTS #46 ([#7144](http://bugs.icu-project.org/trac/ticket/7144))
        \[via custom normalization data\]
*   Formatting
    *   Able to select numbering systems
        ([#7160](http://bugs.icu-project.org/trac/ticket/7160))
    *   BigDecimal format/parse in ICU4C
        ([#5193](http://bugs.icu-project.org/trac/ticket/5193))
*   Footprint
    *   Split locale display names, time zone names, currency names out of
        locale data (into separate data)
        ([#7163](http://bugs.icu-project.org/trac/ticket/7163))
    *   Introduce Locale base class — only id, no display names
        ([#7164](http://bugs.icu-project.org/trac/ticket/7164))
    *   Introduce MessageFormat base class — only string substitution
        ([#7165](http://bugs.icu-project.org/trac/ticket/7165))
    *   Smaller "core" set (without formatting data, only
        manipulation/algorithms)
        [(#7161](http://bugs.icu-project.org/trac/ticket/7161))
    *   Generalized cache management for ICU4C
        ([#2863](http://bugs.icu-project.org/trac/ticket/2863),
        [#](/)[3035](http://bugs.icu-project.org/trac/ticket/3035),
        [#3118](http://bugs.icu-project.org/trac/ticket/3118),
        [#6029](http://bugs.icu-project.org/trac/ticket/6029),
        [#6030](http://bugs.icu-project.org/trac/ticket/6030),
        [#6031](http://bugs.icu-project.org/trac/ticket/6031),
        [#6099](http://bugs.icu-project.org/trac/ticket/6099),[
        #6708](http://bugs.icu-project.org/trac/ticket/6708))
    *   More compact collation (rules/binary) data
        *   Compact collation tailoring syntax for lists of characters with same
            level difference
            ([#7015](http://bugs.icu-project.org/trac/ticket/7015))
        *   Add import rule to collation tailoring syntax
            ([#7023](http://bugs.icu-project.org/trac/ticket/7023),
            [CldrBug:2268](http://unicode.org/cldr/trac/ticket/2268))
    *   Locale data filtering: display names for fewer codes
    *   (apple) Reduce C heap memory usage, especially for time zone
        #[6873](http://bugs.icu-project.org/trac/ticket/6873),
        [#6879](http://bugs.icu-project.org/trac/ticket/6879)

### Tier 2

*   General
    *   Collation script reordering
        ([#3984](http://bugs.icu-project.org/trac/ticket/3984),
        [CldrBug:2267](http://unicode.org/cldr/trac/ticket/2267))
    *   Best match for locale IDs:
        [#4712](http://bugs.icu-project.org/trac/ticket/4712)
    *   Better C++ implementation (scoped_ptr, byte string class; see
        [design/C++](../design/cpp/index.md) page)
        ([#7162](http://bugs.icu-project.org/trac/ticket/7162))
    *   Cast reduction — move methods into base
*   Formatting
    *   Day divisions — more than just am/pm, e.g. Chinese 'noon', 'midnight'
        ([#7150](http://bugs.icu-project.org/trac/ticket/7150),
        [CLDR#713](http://unicode.org/cldr/trac/ticket/713))(Apple also
        interested in this; Peter has a CLDR bug)
    *   More kinds of duration formatting (Apple also interested in this; Peter
        has a CLDR bug)
*   Footprint
    *   Filter out locales with insufficient data

## IBM

*   **General focus: Usability, Maintainability and Performance**
*   Code and Data Maintainability Improvements, e.g. Separating timezone data
    from code.
*   Overriding/updating locale information in an ICU installation: 4597 6633
*   Collation and string search service code clean up: 4562
*   Misc layout bug fixes: 5589 6625 5431 6113 6182
*   Improved ICU performance and regression for selected service areas only,
    e.g. Collation
*   Extended IETF BCP47 support: language/locale specification for
    HTTP/XML/OpenJDK
*   Lenient parsing, e.g. DateFormat. (Already implemented by Apple on branch)
*   Locale service SPI
*   JSR-310 Date and Time APIs
*   @provider multiple version support, Calling old ICU service code through new
    ICU API
*   Java 5 migration (ICU4J)
*   Supporting generics to match JDK APIs
*   ICU 4.4 will no longer support Java 1.4 or older versions
*   Java Logging support (ICU4J)
*   ICU Resource Bundle footprint optimization
