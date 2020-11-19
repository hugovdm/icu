# ICU 4.6

[[TOC]](http://bugs.icu-project.org/trac/milestone/4.6)

[Trac Milestone 4.6](http://bugs.icu-project.org/trac/milestone/4.6)

## All

*   Unicode 6.0 ([#7264](http://bugs.icu-project.org/trac/ticket/7264))
*   CLDR 1.9

## Apple

*   Roll-in of ICU4C lenient date/time/number parsing changes from branch
    (#[3579](http://bugs.icu-project.org/trac/ticket/3579),
    #[6109](http://bugs.icu-project.org/trac/ticket/6109), etc.)
*   Regex enhancements (new API) (need to file tickets)
*   (Also in Google item) ICU features to support CLDR: list patterns, index
    characters, day periods (see tickets in Google section)
*   API access to more CLDR data, especially supplemental (need to file tickets)

## Google

*   Modularization
    *   Create MessageFormatBase
        ([#7165](http://bugs.icu-project.org/trac/ticket/7165)), use where
        possible (especially LocaleDisplayNames:
        [#7457](http://bugs.icu-project.org/trac/ticket/7457)), also
        [#7618](http://bugs.icu-project.org/trac/ticket/7618)
    *   Find tools to examine dependencies; limit new dependencies between ICU
        pieces?
*   CLDR
    *   data filtering ([#7646](http://bugs.icu-project.org/trac/ticket/7646))
    *   ICU features to support CLDR: locale matching
        ([#7512](http://bugs.icu-project.org/trac/ticket/7512)), list patterns
        ([#7516](http://bugs.icu-project.org/trac/ticket/7516)), index
        characters ([#7148](http://bugs.icu-project.org/trac/ticket/7148),
        [#7511](http://bugs.icu-project.org/trac/ticket/7511),
        [#7530](http://bugs.icu-project.org/trac/ticket/7530))
    *   *postponing day periods*
        ([#7150](http://bugs.icu-project.org/trac/ticket/7150))
    *   Index label data from Apple
*   Formatting
    *   Add ROUNDING_UNNECESSARY mode. In JDK, need in ICU4C.
        (#[7572](http://bugs.icu-project.org/trac/ticket/7572))
*   Unicode
    *   Spoof detection: various improvements
        ([#7399](http://bugs.icu-project.org/trac/ticket/7399),
        [#7643](http://bugs.icu-project.org/trac/ticket/7643),
        [#7645](http://bugs.icu-project.org/trac/ticket/7645)) \[M1 - ICU4J
        matches ICU4C 4.4 \]
    *   UTS 46 (IDNA 2008)
        ([#7144](http://bugs.icu-project.org/trac/ticket/7144)) \[M1, C++ only\]
    *   fix line break issues
        ([#7270](http://bugs.icu-project.org/trac/ticket/7270))
*   C++
    *   better C++, internal char \* string class
        ([#7496](http://bugs.icu-project.org/trac/ticket/7496)) \[M1\]
    *   try to use real RTTI, stop adding poor man's RTTI APIs
        ([#7455](http://bugs.icu-project.org/trac/ticket/7455)) \[M1\]

## IBM

**Note: This is a list of candidate items for ICU 4.8. IBM does not have a list for specific for 4.6. So some of these might be done for 4.6, and others might be done for 4.8.**

*   Modulaization

*   Time zone formatter ([
    IcuBug:6325](http://bugs.icu-project.org/trac/ticket/6325), [
    IcuBug:7325](http://bugs.icu-project.org/trac/ticket/7325))
*   API / implementation separation ([
    IcuBug:7098](http://bugs.icu-project.org/trac/ticket/7098))
*   Multiple Jars ([ IcuBug:7271](http://bugs.icu-project.org/trac/ticket/7271))
*   ICU4C plug-in (?)
*   Localization packs (eclipse - no ticket)

*   Performance

*   ICU4J resource bundle ([
    IcuBug:7115](http://bugs.icu-project.org/trac/ticket/7115), [
    IcuBug:7116](http://bugs.icu-project.org/trac/ticket/7116), [
    IcuBug:7081](http://bugs.icu-project.org/trac/ticket/7081)?)
*   Time zone parser memory footprint ([
    IcuBug:6099](http://bugs.icu-project.org/trac/ticket/6099))

*   Quality

*   Peformance regression tests ([
    IcuBug:7275](http://bugs.icu-project.org/trac/ticket/7275))
*   Fix ASCIIsms in tests - may need rethinking of invariant characters for
    escape sequences ([
    IcuBug:7502](http://bugs.icu-project.org/trac/ticket/7502) related to [
    IcuBug:7565](http://bugs.icu-project.org/trac/ticket/7565) IBM i)
*   C API tool should flag issues when APIs are 'born stable', and should report
    changes in enums and #defines ([
    IcuBug:7364](http://bugs.icu-project.org/trac/ticket/7364) and [
    IcuBug:7388](http://bugs.icu-project.org/trac/ticket/7388))
*   Restrict ticket checkins ([
    IcuBug:7589](http://bugs.icu-project.org/trac/ticket/7589))

*   Others

*   Collation sensitive regular expressions ([
    IcuBug:6310](http://bugs.icu-project.org/trac/ticket/6310))
*   Time zone display name list ([
    IcuBug:7293](http://bugs.icu-project.org/trac/ticket/7293))
*   Locale service implementation to support territory driven data properly ([
    IcuBug:6816](http://bugs.icu-project.org/trac/ticket/6816))
*   StringSearch? J port ([
    IcuBug:7218](http://bugs.icu-project.org/trac/ticket/7218))
*   API for data synchronization and age verification ([
    IcuBug:7365](http://bugs.icu-project.org/trac/ticket/7365) continyuing [
    IcuBug:5720](http://bugs.icu-project.org/trac/ticket/5720))

## Yahoo

*   Abstract date formats
    ([#7655)](http://bugs.icu-project.org/trac/ticket/7655)
