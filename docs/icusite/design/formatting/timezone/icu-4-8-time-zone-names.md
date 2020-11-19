# ICU 4.8 Time Zone Names

### Note: ICU4J TimeZoneNames/TimeZoneFormat were included in ICU4J 4.8 as technology preview.

### Background

Time zone display name support in the LDML specification is implemented by ICU,
which is rich and powerful comparing to other implementations. However, at the
same time, the algorithm used by the LDML specification is quite complicated,
therefore the ICU implementation is also heavy and hard to maintain. Since we
moved to the current LDML model, certain type of date format operation do not
perform well and it is almost impossible to customize the behavior even names
returned by ICU is not good for them.

### Requirements

There are several requirements related to time zone display names.

1.  **Performance**
    *   The current implementation gathers all possible display names (many are
        generated algorithmically, not fetched from resource) for parsing. The
        initialization of the name trie is extremely heavy and require huge
        memory footprint. It is desired to reduce the size of trie and also
        defer the initialization until it is really required.
    *   A set of all available names for a single time zone are initialized as
        whole. Even format method does not use a certain type of names, the
        current implementation require all of them initialized at the same time
        (and more than the simple locale data retrieval is required for
        initializing a certain type of names) .
2.  **Direct access to plain name resources** The ICU resource contains plain
    time zone names in relatively complex structure. There are a lot of code and
    logic involved for displaying a time zone name. For now, ICU does not expose
    the direct access to these plain names, instead, users can only access the
    names via format object (TimeZone#getDisplayName also internally call a part
    of format code). However, some ICU users want to access these names without
    other overhead. For example, Harmony/Android (or ICU's own Java
    TimeZoneNameProvider implementation) just want to get long/short specific
    names (such as Eastern Standard Time, PDT). These names are stored in the
    ICU resources as is and it does not require any complicated logic involved
    for their usage. Other users want to get names stored in the ICU resource,
    but not able to get them as is (always used as a part of format). For
    example, ICU import exemplar city name data from CLDR. Even someone want to
    compose a zone name using them - for example "GMT-05:00 New York, Detroit,
    Montreal", there is no way to collect localized exemplar city names with
    public APIs.
3.  **Customization**
    1.  Before the LDML "metazone" model was introduced, ICU users can get/set
        time zone names through DateFormatSymbols object. DateFormatSymbols
        represents time zone name in a plain 2 dimensional array. After
        "metazone" was introduced, we can no longer represent time zone names in
        this form, because the metazone require historic name mappings and some
        logic required for distinguishing one zone from another mapped to the
        same metazone. For now, the time zone array in DateFormatSymbols is not
        used, and not useful at all. We need public APIs matching the LDML time
        zone display name algorithm and data structure for allowing ICU users to
        access and customize display names. \[MD\] This is especially important
        for CLDR work as well, that we can override any of the ICU data.
    2.  GMT format (such as "GMT+03:00") is not customizable through public
        APIs. There is a demand to compose time zone string from different
        patterns.

### UTS#35 Time Zone Display Name Basics

[UTS#35 LDML](http://www.unicode.org/reports/tr35/) defines several style of
time zone display names and fallbacks. Certain style of names are not stored in
CLDR directly and some algorithms involved for getting the final display name.
Also, each name style has its own goal and round trip requirements. To achieve
these goals, display name for a certain type may use a variant form. The table
below illustrates these styles, algorithm and variations.

Style Pattern
Examples Description Roundtrip Requirement
NoteRFC822Z-0500Short representation with actual offset amount. Used by RFC822
(internet mail) and other standard protocols.
UTC offset
Localized GMT
ZZ
ZZZ
ZZZZ
GMT-05:00
GMT+03:07:51
UTC+0100
User friendly representation with actual offset amount.
UTC offset
- Localized GMT Zero format (for example "GMT") is used if UTC offset is 0.
- "GMT", "UTC" and "UT" must be interpreted as GMT Zero independent from a
locale.
Specific Short
V
EDT
CIT
Acronym of zone name with standard/daylight distinction.
UTC offset when other fields sufficient for detecting the exact date are
available
\[Fallback\]
- Localized GMT is used when localized name is not available.
Specific Short Commonly Used
z
EDT
Acronym of zone name with standard/daylight distinction, but commonly used for
the target locale only.
same as above
same as above
Specific Long
zz
zzz
zzzz
Eastern Standard Time
Eastern Daylight Time
Zone name with standard/daylight distinction.
same as above
same as above
Generic Short
v
ET
Acronym of wall time, no distinction between standard/daylight.
UTC offset when other fields sufficient for detecting the exact date are
available, but except for the small window when local time overlaps (for
example, daylight -> standard time)
\[Fallback\]
1. Generic Location format when localized generic name is not available.
2. Localized GMT
\[Other Rules\]
1. When a generic name comes from a meta zone and the zone does not observe
daylight saving time during +- 184 days window, the name is substituted by
Specific Long name for standard time.
2. When a generic name comes from a meta zone and the zone's offset at the given
time is different from the preferred zone of the meta zone for the target
locale, Generic Partial Location name is generated.
Generic Partial Location name is composed by -
a) generic name, such as "CT"
b) location name, such as "Matamoros"
c) format pattern, such as "{0} ({1})" (0=generic name / 1=location name)
If the time zone's canonical zone is the only canonical zone of the time zone's
region, localized country name is used as the location name. For example,
"America/Port-au-Prince" is the only canonical zone for Haiti, so "Haiti" is
used for the location name.
Otherwise, localized exemplar city name is used for the location name.
Generic Long
vvvv
Eastern Time
Wall time, no distinction between standard/daylight.
same as above
same as above
Generic LocationVVVV
United States Time (New York)
Zone name with location information. Each canonical time zone has its unique
location name (with a few exceptions)
Canonical time zone
Generic Location name is composed by -
a) location name(s) - country | country + city
b) format pattern
If the time zone belongs to a country that has multiple canonical zones, both
localized country name and city name are used for the location. Otherwise
(single canoninical zone in the country), localized country name alone is used
for location.
There are two format patterns - one for country only and another for country +
city. For example,
- "{0} Time"
- "{0} Time ({1})"
When localized city name is not available, the default algorithm will compose
the city name (strictly speaking, it's a territory name) by extracting the most
right segment of the canonical ID separated by "/", then replace "_" with " ".
For example, the city name for canonical ID "America/New_York" will be "New
York".
\[Fallback\]
- Localized GMT is used when the time zone does not belong to a specific region

### Strategies

*   Create a new class for representing the CLDR time zone display name data
    model
    *   Make the class as an API class, so ICU users can fully customize the
        time zone display name data used by ICU. This makes CLDR ST to populate
        time zone names on the fly, allow us to write verification code for
        detecting name collisions, etc.
    *   We can deprecate the use of the legacy 2-dimensional zone strings in
        DateFormatSymbols for customizing the zone display names in DateFormat,
        which no longer works fully with the current algorithm.
    *   The new API class allows direct data access to raw time zone display
        names including "exemplar city" names which can be found only in a part
        of generic location format currently.
    *   Software only requiring specific long/short names (such as
        Android/Harmony, Java LocaleSPI TimeZoneNameProvider) can access the
        names directly through the API without dragging various overheads with
        time zone generic name handling.
    *   In ICU4J, the isolation of time zone display name data allows us to make
        a time zone display name data provider as a separate component.
*   Create a new public API - time zone format class interacting with the time
    zone display name data model class above
    *   Add setter/getter for the time zone format class in SimpleDateFormat.
        This allows ICU users to customize time zone display names at the data
        level as well as allow them to fully customize time zone display name
        format used by ICU date format.
    *   All pattern driven time zone name formatter and fallback logic belong to
        this class implementation.
*   Optimized for most common use cases
    *   The default time patterns used by DateFormat do not contain generic time
        zone name types (location / non-location). These generic time zone names
        are not simple data, instead, a lot of fallback/construction logic
        involved as you can see the table in the previous section. The current
        ICU implementation load and initialize generic names with other type of
        names used by the default time patterns. The new implementation defers
        the initialization for generic names until they are requested.
    *   The current ICU time zone parse require all names including generic
        names are collected and stored in a trie. By setting a limitation - do
        not parse names that are never generated by the date format instance,
        the new implementation can skip the initialization of all generic names
        and putting them into the trie.

### Proposed APIs

#### \[ICU4J\]

Technology preview version of APIs in ICU4J 4.8:

*   [TimeZoneNames](http://icu-project.org/apiref/icu4j48/com/ibm/icu/text/TimeZoneNames.html)
*   [TimeZoneFormat](http://icu-project.org/apiref/icu4j48/com/ibm/icu/text/TimeZoneFormat.html)

#### \[ICU4C\]

<TBD>

### Design Note

Q. Meta zone to/from TZID mapping is locale independent. Why are these methods
(getAvailableMetaZoneIDs(String tzID) / getMetaZoneID(String tzID, long date) /
getReferenceZoneID(String mzID, String region)) in TimeZoneNames class as
instance methods?
A. It is true that the mapping is locale independent in CLDR. Initially, we
thought the mapping could be different by locale, but we decided to use the
single set of mapping data for all locales because it looked impossible to
maintain such data in locale dependent manner. However, logically, the mapping
is integral part of time zone display name data and we should not stop ICU users
to define different mappings (or no mappings, i.e, no meta zones). Of course,
ICU implementation share the same implementation for all locales.
Q. Do we need both TimeZoneNames#getAvailableMetaZoneIDs() and
TimeZoneNames#getAvailableMetaZoneIDs(String tzID)?
A. Strictly speaking, no. The former one can be implemented if the latter one is
available. There is a need to access all available meta zone IDs in ICU
implementation, but generating a set of meta zone IDs from the latter version is
a little bit expensive. We could change the former one (no arg version) as
non-abstract, implements the method by iterating through all time zones, but
ICU's TimeZoneNames class overrides the method to return the set with more
efficient implementation.
Q. Do we need TimeZoneFormat.Style.SPECIFIC_SHORT_COMMONLY_USED?
A. After CLDR 2.0, we tentatively agreed to deprecate "commonly used". When the
decision becomes final, we should remove the style before promoting it to public
(@draft) API.

### Incompatible Changes

There are some incompatible changes introduced by this proposal. I assume the
impact of these changes were acceptable

*   SimpleDateFormat parse no longer looks up all possible time zone names. By
    default, it only supports RFC822 format, Localized GMT format and all names
    that are available for the time zone format pattern used by the
    SimpleDateFormat instance.
    *   If you really need to look up all possible names for a locale, you can;
        *   Create TimeZoneFormat instance and use one of parse methods which
            does not take "style" parameter.
        *   Create TimeZoneFormat instance and set setParseAllStyles(true). You
            can set the instance to SimpleDateFormat too.
*   DateFormatSymbols.getZoneStrings() used to return several more name types
    that were never documented before. The API doc has been updated to clarify
    the contents of the result array and no longer includes name types other
    than these 4 types (long/short \* standard/daylight).
*   DateFormatSymbols.setZoneStrings() still works, but it no longer affects the
    behavior of SimpleDateFormat using the instance of DateFormatSymbols. It's
    required to create a custom instance (subclass) of TimeZoneNames to provide
    custom time zone display name localizations.

### Performance Number Comparison

#### A. ICU4J DateFormatSymbols - getZoneStrings()

long t0 = System.currentTimeMillis();
DateFormatSymbols dfs = new DateFormatSymbols();
long t1 = System.currentTimeMillis();
String\[\]\[\] zones = dfs.getZoneStrings();
long t2 = System.currentTimeMillis();
System.out.println("DateFormatSymbols(init): " + (t1 - t0));
System.out.println("getZoneStrings() : " + (t2 - t1));
Per 10 iteration
DateFormatSymbols(init) getZoneStrings() trunk 57.2ms (sd=1.9)
469.8ms (sd=31.9)
new 48.6ms (sd=1.6)
220.4ms (sd=16.3)

#### B. ICU4J SimpleDateFormat GMT parse ([ticket#7081](http://bugs.icu-project.org/trac/ticket/7081))

long t0 = System.currentTimeMillis();
SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yy-HH:mm:ss-z");
long t1 = System.currentTimeMillis();
try {
Date d = sdf.parse("01-Jan-2015-00:00:20-GMT");
} catch (Throwable t) {
}
long t2 = System.currentTimeMillis();
System.out.println("SimpeDateFormat(init): " + (t1 - t0));
System.out.println("Parse : " + (t2 - t1));
System.out.println("Total : " + (t2 - t0));
Per 10 iteration
SimpleDateFormat(init)
Parse
Total trunk 194.2ms (sd=8.6)
469.1ms (sd=43.5)
663.3ms (sd=37.7)
new 385.3ms (sd=2.8) 32.1ms (sd=1.7) 417.4ms (sd=3.9) jdk 17.2ms (sd=0.8)
0.6ms (sd=0.5)
17.7ms (sd=0.9)

#### C. ICU4J Full Date/Time Format Roundtrip

Date d = new Date();
long t0 = System.currentTimeMillis();
DateFormat df = DateFormat.getDateTimeInstance(DateFormat.FULL,
DateFormat.FULL);
long t1 = System.currentTimeMillis();
String s = df.format(d);
long t2 = System.currentTimeMillis();
try {
Date d1 = df.parse(s);
} catch (Throwable t) {
}
long t3 = System.currentTimeMillis();
System.out.println("DateFormat getInstance: " + (t1 - t0));
System.out.println("format : " + (t2 - t1));
System.out.println("parse : " + (t3 - t2));
System.out.println("total : " + (t3 - t0));
Per 10 iteration
DateFormat getInstance
format parse total trunk 197.5ms (sd=2.8)
104.0ms (sd=4.3)
22.2ms (sd=0.8)
323.7 (sd=4.4)
new 248.4ms (sd=3.2)
20.4ms (sd=1.1)
28.4ms (sd=4.5)
297.2 (sd=6.5)
Note: The new implementation takes more time in getInstance() comparing to the
trunk because of TimeZoneFormat initialization. The difference for the second
invocation is almost none, because TimeZoneFormat instance is cached. We could
defer the TimeZoneFormat initialization until it's really used (for example, a
format pattern that does not have any time zone field does not need
TimeZoneFormat...).
