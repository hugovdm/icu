# Download ICU 49 M2 (Superseded by 49)

## Note: [ICU 49 has been released. Please click here.](49.md)

### Description

ICU is the [premier library for software
internationalization](http://site.icu-project.org/#TOC-What-is-ICU-). This page
contains the latest ICU 49 milestone (49.0.x) build links.

Note: We are changing the [ICU release version
numbering](http://userguide.icu-project.org/design#TOC-Version-Numbers-in-ICU),
combining the first two fields into one. The next release after ICU 4.8 will be
ICU 49 then ICU 50 etc.

Major changes since ICU 4.8 include the following:

#### Common Changes

*   [Unicode 6.1 beta](http://www.unicode.org/review/pri206/): New scripts &
    blocks; changes to grapheme break & line break property values; some
    characters change from symbol to Po or No; etc.
*   [CLDR 21 milestone 2](http://cldr.unicode.org/index/downloads): Changes in
    segmentation data to match Unicode 6.1; new calendar structures for support
    of Chinese calendar; new ar_001 locale for Modern Standard Arabic;
    deprecation of "commonlyUsed" element in timezone names; removal of
    "whole-locale" aliases.
*   Normalizer2 API additions
    *   Easier-to-use getInstance() variants; e.g., getNFDInstance()
        ([#8246](http://bugs.icu-project.org/trac/ticket/8246))
    *   Getter for the combining-class value for a code point
        ([#8606](http://bugs.icu-project.org/trac/ticket/8606))
    *   Getter for the raw Decomposition_Mapping
        ([#8804](http://bugs.icu-project.org/trac/ticket/8804))
    *   Pairwise composition
        ([#8804](http://bugs.icu-project.org/trac/ticket/8804))
*   TimeZone class: (C++) Getter for unknown time zone, (Java) fields for GMT &
    unknown zone ([#8779](http://bugs.icu-project.org/trac/ticket/8779))
*   Support for deprecation of the "commonlyUsed" element for CLDR metazones
    ([#8811](http://bugs.icu-project.org/trac/ticket/8811))
*   DateTImePatternGenerator can now use separate patterns for skeletons that
    differ only in MMM vs MMMM or EEE vs EEEE, etc.
    ([#7930](http://bugs.icu-project.org/trac/ticket/7930))

#### ICU4C Specific Changes

*   One platform.h file used on all platforms now
    ([#8452](http://bugs.icu-project.org/trac/ticket/8452))
*   Smaller binaries with static-linked ICU
    ([#8453](http://bugs.icu-project.org/trac/ticket/8453))
*   Explicit constructors in UnicodeString
    ([#7877](http://bugs.icu-project.org/trac/ticket/7877))
*   Option for not including utf headers
    ([#8575](http://bugs.icu-project.org/trac/ticket/8575))
*   Added function u_printf
    ([#8579](http://bugs.icu-project.org/trac/ticket/8579))
*   C++ namespace support
    ([#8680](http://bugs.icu-project.org/trac/ticket/8680))
*   Format and parse Chinese calendar dates including support for intercalary
    months ([#8958](http://bugs.icu-project.org/trac/ticket/8958))
*   DateFormat/SimpleDateFormat format/parse const methods are really const now
    ([#8844](http://bugs.icu-project.org/trac/ticket/8844))

#### ICU4J Specific Changes

*   Default ULocale sensitive to user.script system property even on Java 5 & 6
    ([#8446](http://bugs.icu-project.org/trac/ticket/8446))
*   TimeZone is now Freezable; getFrozenTimeZone()
    ([#8345](http://bugs.icu-project.org/trac/ticket/8345))
*   APIs for TimeZoneNames and TimeZoneFormat
    ([#8512](http://bugs.icu-project.org/trac/ticket/8512))
*   Add getMaxBytesForString to CharsetEncoderICU
    ([#8515](http://bugs.icu-project.org/trac/ticket/8515))
*   Support for Java 7 Locale Category
    ([#8630](http://bugs.icu-project.org/trac/ticket/8630)) \[Already back
    ported to ICU4J 4.4.2.2/4.6.1.1/4.8.1.1\]
*   A new utility class MessagePatternUtil supporting MessagePattern tree
    structure analysis ([#8745](http://bugs.icu-project.org/trac/ticket/8745))

### ICU4C Download

**Version:** 49M2 (49.0.2)
**Release Date:** 2011-12-14

*   [API Changes since ICU4C
    4.8](http://source.icu-project.org/repos/icu/icu/tags/milestone-49-0-2/APIChangeReport.html)
*   [Readme](http://www.icu-project.org/repos/icu/icu/tags/milestone-49-0-2/readme.html)

(If the list of files does not appear above, see [ICU4C
Binaries](http://apps.icu-project.org/icu-jsp/downloadPage.jsp?ver=49m2&base=c&svn=milestone-49-0-2).)

(If the list of files does not appear above, see [ICU4C
Source](http://apps.icu-project.org/icu-jsp/downloadPage.jsp?ver=49m2&base=cs&svn=milestone-49-0-2).)
To [extract the source code](http://site.icu-project.org/repository), use the
following command: `svn export
http://source.icu-project.org/repos/icu/icu/tags/milestone-49-0-2/`

#### Previous ICU4C 49 Milestones

---

**Version:** 49M1 (49.0.1)
**Release Date:** 2011-10-05

*   [ICU4C 49M1 files](http://download.icu-project.org/files/icu4c/49m1/)

### ICU4J Download

**Version:** 49M2 (49.0.2)
**Release Date:** 2011-12-14

*   [API Changes since ICU4J
    4.8](http://source.icu-project.org/repos/icu/icu4j/tags/milestone-49-0-2/APIChangeReport.html)
*   [Readme](http://www.icu-project.org/repos/icu/icu4j/tags/milestone-49-0-2/readme.html)

(If the list of files does not appear above, see [ICU4J
Source](http://apps.icu-project.org/icu-jsp/downloadPage.jsp?ver=49m1&base=j&svn=milestone-49-0-2).)

#### Previous ICU4J 49 Milestones

---

**Version:** 49M1 (49.0.1)
**Release Date:** 2011-10-05

*   [ICU4J 49M1 files](http://download.icu-project.org/files/icu4j/49m1)

---

[Legal Notice: Time Zone Database](../tznotice.md)
