# ICU-TC Home Page

### News

2020-10-28: [ICU 68](../download/68.md) released. It updates to CLDR 38. New
features including locale-dependent smart unit preferences (road distance,
temperature, etc.) and locale ID canonicalization conformant with CLDR.

2020-04-22: [ICU 67](../download/67.md)
[released](http://blog.unicode.org/2020/04/icu-67-released.html). It updates to
Unicode 13 & CLDR 37. Bug fixes for date and number formatting, enhanced support
for user preferences in the locale identifier. LocaleMatcher code and data
improved. Number skeletons have a new “concise” form that can be used in
MessageFormat strings.

2020-03-11: [ICU 66](../download/66.md) released. It updates to Unicode 13 &
CLDR 36.1. New, extra Q1 releases for low-risk integration of Unicode 13.

2019-10-03: [ICU 65](../download/65.md) released. It updates to CLDR 36 locale
data with many additions and corrections, and some new measurement units. The
Java LocaleMatcher API is improved, and ported to C++. For building ICU data,
there are new filtering options, and new tracing support for data loading in
ICU4C.

2019-04-17: [ICU 64.2](../download/64.md) released. This maintenance update for
ICU 64 includes draft Unicode 12.1 update, CLDR 35.1 locale data and support for
new Japanese era Reiwa (令和).

2019-04-12: ICU [63.2](../download/63.md) / [62.2](../download/62.md) /
[61.2](../download/61.md) / [60.3](../download/60.md) /
[59.2](../download/59.md) / [58.3](../download/58.md) /
[57.2](../download/57.md) / [56.2](../download/56.md) /
[55.2](../download/55.md) / [54.2](../download/54.md) /
[53.2](../download/53.md) / [52.2](../download/52.md) /
[51.3](../download/51.md) / [50.2](../download/50.md) /
[4.8.2](../download/48.md) released. These maintenance updates for past ICU
versions include support for new Japanese era Reiwa (令和).

2018-07-16: The ICU [source repository](../index.md) has also moved to GitHub.
([Unicode blog
post](http://blog.unicode.org/2018/07/icu-moves-to-github-and-jira.html).)

2018-07-01: The ICU bug tracker has moved! Please see [Submitting ICU Bugs and
Feature Requests](../bugs.md) for updated information.

[TOC]

### What is ICU?

ICU is a mature, widely used set of C/C++ and Java libraries providing Unicode
and Globalization support for software applications. ICU is widely portable and
gives applications the same results on all platforms and between C/C++ and Java
software.

ICU is released under a nonrestrictive [open source
license](http://www.unicode.org/copyright.html#License) that is suitable for use
with both commercial software and with other open source or free software.

Here are a few highlights of the services provided by ICU:

*   Code Page Conversion: Convert text data to or from Unicode and nearly any
    other character set or encoding. ICU's conversion tables are based on
    charset data collected by IBM over the course of many decades, and is the
    most complete available anywhere.
*   Collation: Compare strings according to the conventions and standards of a
    particular language, region or country. ICU's collation is based on the
    Unicode Collation Algorithm plus locale-specific comparison rules from the
    [Common Locale Data Repository](http://www.unicode.org/cldr/), a
    comprehensive source for this type of data.
*   Formatting: Format numbers, dates, times and currency amounts according the
    conventions of a chosen locale. This includes translating month and day
    names into the selected language, choosing appropriate abbreviations,
    ordering fields correctly, etc. This data also comes from the Common Locale
    Data Repository.
*   Time Calculations: Multiple types of calendars are provided beyond the
    traditional Gregorian calendar. A thorough set of timezone calculation APIs
    are provided.
*   Unicode Support: ICU closely tracks the Unicode standard, providing easy
    access to all of the many Unicode character properties, Unicode
    Normalization, Case Folding and other fundamental operations as specified by
    the [Unicode Standard](http://www.unicode.org/).
*   Regular Expression: ICU's regular expressions fully support Unicode while
    providing very competitive performance.
*   Bidi: support for handling text containing a mixture of left to right
    (English) and right to left (Arabic or Hebrew) data.
*   Text Boundaries: Locate the positions of words, sentences, paragraphs within
    a range of text, or identify locations that would be suitable for line
    wrapping when displaying the text.

And much more. Refer to the [ICU User Guide](http://userguide.icu-project.org/)
for details.

### Why Unicode?

Unicode (and the parallel ISO 10646 standard) defines the character set
necessary for efficiently processing text in any language and for maintaining
text data integrity. In addition to global character coverage, the Unicode
standard is unique among character set standards because it also defines data
and algorithms for efficient and consistent text processing. This simplifies
high-level processing and ensures that all conformant software produces the same
results. The widespread adoption of Unicode over the last decade made text data
truly portable and formed a cornerstone of the Internet.

*   [Unicode overview](http://www.ibm.com/software/globalization/topics/unicode)
*   [What is Unicode?](http://www.unicode.org/standard/WhatIsUnicode.html)

Globalized software, based on Unicode, maximizes market reach and minimizes
cost. Globalized software is built and installed once and yet handles text for
and from users worldwide and accomodates their cultural conventions. It
minimizes cost by eliminating per-language builds, installations, and
maintenance updates.

### Why ICU4C?

The C and C++ languages and many operating system environments do not provide
full support for Unicode and standards-compliant text handling services. Even
though some platforms do provide good Unicode text handling services, portable
application code can not make use of them. The ICU4C libraries fills in this
gap. ICU4C provides an open, flexible, portable foundation for applications to
use for their software globalization requirements. ICU4C closely tracks industry
standards, including Unicode and CLDR (Common Locale Data Repository).

### Why ICU4J?

Java provides a very strong foundation for global programs, and IBM and the ICU
team played a key role in providing globalization technology into Sun's Java.
But because of its long release schedule, Java cannot always keep up-to-date
with evolving standards. The ICU team continues to extend Java's Unicode and
internationalization support, focusing on improving performance, keeping current
with the Unicode standard, and providing richer APIs, while remaining as
compatible as possible with the original Java text and internationalization API
design.

See [Why Use ICU4J?](why-use-icu4j.md)

### ICU4JNI

New versions of ICU4JNI are no longer being created. If you need the
functionality of ICU4JNI, you should consider migrating to ICU4J.

### Who Uses ICU?

The following is a list of products, companies and organizations reported to be
using ICU. If you have any feedback on this list (corrections, additions, or
details), please [contact us](../contacts.md) (on icu-support).

#### Companies and Organizations using ICU

ABAS Software, Adobe, Amazon (Kindle), Amdocs, Apache, Appian, Apple, Argonne
National Laboratory, Avaya, BAE Systems Geospatial eXploitation Products, BEA,
BluePhoenix Solutions, BMC Software, Boost, BroadJump, Business Objects, caris,
CERN, CouchDB, Debian Linux, Dell, Eclipse, eBay, EMC Corporation, ESRI,
Facebook (HHVM), Firebird RDBMS, FreeBSD, Gentoo Linux, Google, GroundWork Open
Source, GTK+, Harman/Becker Automotive Systems GmbH, HP, Hyperion, IBM, Inktomi,
Innodata Isogen, Informatica, Intel, Interlogics, IONA, IXOS, Jikes, Library of
Congress, LibreOffice, Mathworks, Microsoft, Mozilla, Netezza, Node.js, Oracle
(Solaris, Java), Lawson Software, Leica Geosystems GIS & Mapping LLC, Mandrake
Linux, OCLC, Progress Software, Python, QNX, Rogue Wave, SAP, SIL, SPSS,
Software AG, SuSE, Sybase, Symantec, Teradata (NCR), ToolAware, Trend Micro,
Virage, webMethods, Wine, WMS Gaming, XyEnterprise, Yahoo!, Vuo, and many
others.

#### Apache Projects

Harmony, Lucene search library, OpenOffice, PDFBox library, Solr search engine
server, Tika metadata toolkits, Xalan XSLT, Xerces XML

#### Products from IBM

DB2, Lotus, Websphere, Tivoli, Rational, AIX, i/OS, z/OS

Ascential Software, Cloudant, Cognos, PSD Print Architecture, COBOL, Host Access
Client, InfoPrint Manager, Informix GLS, Language Analysis Systems, Lotus Notes,
Lotus Extended Search, Lotus Workplace, WebSphere Message Broker, NUMA-Q, OTI,
OmniFind, Pervasive Computing WECMS, Rational Business Developer and Rational
Application Developer, SS&S Websphere Banking Solutions, Tivoli Presentation
Services, Tivoli Identity Manager, WBI Adapter/ Connect/Modeler and Monitor/
Solution Technology Development/WBI-Financial TePI, Websphere Application
Server/ Studio Workload Simulator/Transcoding Publisher, XML Parser.

#### Products from Google

Web Search, Google+, Chrome/Chrome OS, Android, Adwords, Google Finance, Google
Maps, Blogger, Google Analytics, Google Groups, and others.

#### Products from Apple

macOS (OS & applications), iOS (iPhone, iPad, iPod touch), watchOS & tvOS,
Safari for Windows & other Windows applications and related support, Apple
Mobile Device Support in iTunes for Windows.

#### Products from Microsoft

Windows Bridge for iOS
([link](https://developer.microsoft.com/windows/bridges/ios)), Windows 10 -
Creators Update, Visual Studio 2017 \[Electron\], Visual Studio Code
\[Electron\], ChakraCore

#### Products from Harman/Becker

The following car brands are using ICU via the Harman/Becker automotive
software: *Alfa Romeo, Audi, Bentley, BMW, Buick,
[more...](http://www.harman.com/connected-car)*

#### Products from Adobe

[Creative Cloud apps](https://www.adobe.com/creativecloud.html) and [Document
Cloud](https://acrobat.adobe.com/)

#### Related Projects

There are also some [related projects](../related.md) that wrap the existing
functionality of ICU.
