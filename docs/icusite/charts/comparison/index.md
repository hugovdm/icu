# Feature Comparison Chart

This table lists down the differences between the functions provided by ICU,
ICU4J and JDK.

In the case where ICU and JDK supports the same functionality, ICU has a more
updated implementation that is Unicode conformant.

Functions ICU4C 4.0 or later
ICU4J 4.0 or later
JDK 6 **Character**
[Properties](http://icu-project.org/userguide/properties.html) [comparison
details](http://icu-project.org/charts/comparison/character.html)
[UnicodeSet](http://icu-project.org/userguide/unicodeSet.html) unicode/uniset.h
com.ibm.icu.text.UnicodeSet not available
**[Strings](http://icu-project.org/userguide/strings.html)** UTF16 manipulation
unicode/ustring.h
unicode/unistr.h
unicode/utf.h
unicode/utf16.h com.ibm.icu.lang.Character
com.ibm.icu.text.UTF16 java.lang.String
java.lang.Character
(Since Java 5) Code point order comparison not available Code unit order
comparison java.lang.String Unicode Character Iterator unicode/chariter.h
unicode/uiter.h com.ibm.icu.text.UCharacterIterator java.text.CharacterIterator
**[Converter](http://icu-project.org/userguide/codepageConverters.html)**
[Conversion Data](http://icu-project.org/userguide/conversion-data.html)
[Mapping tables](http://icu-project.org/charts/charset/)
Default build: [Converter Explorer](http://demo.icu-project.org/icu-bin/convexp)
icu4j-charsets-version.jar
Same data as icu4c. [ Supported
encodings](http://java.sun.com/javase/6/docs/technotes/guides/intl/encoding.doc.html)
[Unicode Compression](http://icu-project.org/userguide/compression.html)
[SCSU](http://www.unicode.org/unicode/reports/tr6/) unicode/ucnv.h
com.ibm.icu.text.SCSU not available [BOCU-1](http://www.unicode.org/notes/tn6/)
unicode/ucnv.h not available not available [Charset
Detection](http://www.icu-project.org/userguide/charsetDetection.html)
unicode/csdetect.h com.ibm.icu.text.CharsetDetector not available
**[Locale](http://icu-project.org/userguide/locale.html)** Locale data [ Locale
data](http://source.icu-project.org/repos/icu/icu/trunk/source/data/locales/)
Uses CLDR 1.5.0.1. [ Locale
data](http://source.icu-project.org/repos/icu/icu4j/trunk/src/com/ibm/icu/impl/data/icudata.jar)
Supports the same locale data as in ICU4C. [ Supported
locales](http://java.sun.com/javase/6/docs/technotes/guides/intl/locale.doc.html)
Uses some CLDR data. **[Resource
Management](http://icu-project.org/userguide/ResourceManagement.html)** String
and string array storage and retrieval unicode/ures.h
com.ibm.icu.util.UResourceBundle java.util.ResourceBundle Binary storage and
retrieval unicode/ures.h com.ibm.icu.util.UResourceBundle none that are locale
sensitive Object and object array storage and retrieval not available use Java
java.util.ResourceBundle **[Date/Time
Services](http://icu-project.org/userguide/dateTime.html)** Calendar Extensible
calendar base class unicode/calendar.h com.ibm.icu.util.Calendar
java.util.Calendar
unicode/calendar.h
use locale with option:
com.ibm.icu.util.Calendar
use locale with option:
or use class:
Gregorian "@calendar=gregorian" "@calendar=gregorian"
com.ibm.icu.util.GregorianCalendar java.util.GregorianCalendar Chinese
"@calendar=chinese" "@calendar=chinese"
com.ibm.icu.util.ChineseCalendar not available Coptic "@calendar=coptic"
"@calendar=coptic"
com.ibm.icu.util.CopticCalendar not available Ethiopic "@calendar=ethiopic"
"@calendar=ethiopic"
com.ibm.icu.util.EthiopicCalendar not available Hebrew "@calendar=hebrew"
"@calendar=hebrew"
com.ibm.icu.util.HebrewCalendar not available Indian "@calendar=indian"
"@calendar=indian"
com.ibm.icu.util.IndianCalendar not available Hijri/Islamic (civil)
"@calendar=islamic-civil" "@calendar=islamic-civil"
com.ibm.icu.util.IslamicCalendar not available Hijri/Islamic (religious /
astronomical) "@calendar=islamic" "@calendar=islamic"
com.ibm.icu.util.IslamicCalendar not available Japanese "@calendar=japanese"
"@calendar=japanese"
com.ibm.icu.util.JapaneseCalendar use locale: ja_JP_JP Persian
"@calendar=persian" not available not available Taiwan "@calendar=taiwan"
"@calendar=taiwan"
com.ibm.icu.util.TaiwanCalendar not available Thai Buddhist "@calendar=buddhist"
"@calendar=buddhist"
com.ibm.icu.util.BuddhistCalendar use locale: th_TH (any variant) Time Zone
unicode/timezone.h com.ibm.icu.util.TimeZone java.util.TimeZone Time Zone rule
based retrieval / comparison unicode/basictz.h
unicode/rbtz.h
com.ibm.icu.util.BasicTimeZone
com.ibm.icu.util.RuleBasedTimeZone not available **[Format &
Parse](http://icu-project.org/userguide/formatParse.html)** Extensible format
base class unicode/format.h use Java java.text.Format Date / time
unicode/datefmt.h com.ibm.icu.text.DateFormat java.text.DateFormat Date / time,
relative unicode/datefmt.h
(use relative style) com.ibm.icu.text.DateFormat
(use relative style) not available Date / time duration not available
com.ibm.icu.text.DurationFormat not available Decimals numbers
unicode/decimfmt.h com.ibm.icu.text.DecimalFormat java.text.DecimalFormat
Message unicode/msgfmt.h com.ibm.icu.text.MessageFormat java.text.MessageFormat
Extensible number base class unicode/numfmt.h com.ibm.icu.text.NumberFormat
java.text.NumberFormat Rule-based number unicode/rbnf.h
com.ibm.icu.text.RuleBasedNumberFormat not available Choice unicode/choicfmt.h
use Java java.text.ChoiceFormat
**[Transformations](http://icu-project.org/userguide/Transformations.html)**
[Case Mappings](http://icu-project.org/userguide/caseMappings.html)
unicode/uchar.h
unicode/ustring.h
unicode/unistr.h com.ibm.icu.lang.UCharacter java.lang.Character [BiDi
Algorithm](http://icu-project.org/userguide/bidi.html) unicode/ubidi.h
com.ibm.icu.text.Bidi java.text.Bidi
[Normalization](http://icu-project.org/userguide/normalization.html)
Decomposition unicode/unorm.h
unicode/normlzr.h
com.ibm.icu.text.Normalizer java.text.Normalizer
(Since Java 6) Composition Canonical string comparisons String concatenation
Note: result string is normalized. not available FC_NFKC closure set retrieval
not available [Quick check](http://www.unicode.org/unicode/reports/tr15/#Annex8)
not available [FCD](http://www.unicode.org/notes/tn5/#FCD) not available
[General/script
transformation](http://icu-project.org/userguide/Transformations.html)
[comparison details](http://icu-project.org/charts/comparison/transforms.html)
**[Layout Engine](http://icu-project.org/userguide/layoutEngine.html)**
layout/loengine.h not available internal
**[Collation](http://icu-project.org/userguide/Collate_Intro.html)** [comparison
details](http://icu-project.org/charts/comparison/collation.html) **[Regular
Expressions](http://icu-project.org/userguide/regexp.html)** unicode/regex.h use
Java java.util.regex **[Boundary
Analysis](http://icu-project.org/userguide/boundaryAnalysis.html)**
Dictionary-based break iterators unicode/brkiter.h
com.ibm.icu.text.BreakIterator not available Line boundaries unicode/brkiter.h
com.ibm.icu.text.BreakIterator java.text.BreakIterator Sentence boundaries
unicode/brkiter.h com.ibm.icu.text.BreakIterator java.text.BreakIterator Word
boundries unicode/brkiter.h com.ibm.icu.text.BreakIterator
java.text.BreakIterator Rule-based break iterators unicode/rbbi.h
com.ibm.icu.text.RuleBasedBreakIterator not available
