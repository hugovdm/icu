# Canonicalization

Discussion of ICU ticket #6593 : Locale canonicalization should be externalized.
This ticket attempts to remove as many of the hard coded tables as possible in
favor of a data driven approach. Here is a summary of the current
canonicalization logic in ICU4J ( 49M2 ).
1). Use variantsToKeywords ( a hard-coded array of String ) to convert known
variants to a known key-value pair ( i.e. EURO = @currency=EUR )
2). Use CANONICALIZE_MAP ( a hard coded array of String ) to convert known
locales to their canonical equivalent.
3). Hack for language "nb" and variant "NY" ( still necessary ? ) mapping to nn.
4). Return the name as received from the LocaleIDParser.
LocaleIDParser performs the following modifications using hard-coded tables.
1). Language - converts 3-letter language codes to 2-letter codes.
2). Country - converts 3-letter country codes to 2-letter codes.
ICU Resource bundle logic ( ICUResourceTableAccess.getTableString() ) also has a
dependency on hard coded tables in the LocaleIDs class via the
LocaleIDs.getCurrentLanguageID() and LocaleIDs.getCurrentCountryID() methods,
that have lists of deprecated language and country codes and replace them with
their corresponding modern codes.
**Proposed Solution**
1). CLDR already contains supplemental metadata that can handle most of the
replacements listed above. This data is currently being imported into ICU format
in metadata.txt as languageAlias, scriptAlias, territoryAlias, and variantAlias.
This data could be accessed and used instead of the hard-coded tables to perform
3 to 2 letter code conversions as well as the replacement of deprecated codes in
LocaleIDs.getCurrentLanguageID() and LocaleIDs.getCurrentCountryID().
2). The variantsToKeywords and CANONICALIZE_MAP as outlined in points 1 and 2
above are really ICU specific data and shouldn't really be a part of the CLDR.
However, we can mimic the CLDR data structure by use of an ICU specific
structure for variantKeywordAlias and localeAlias and import them into ICU's
data during the normal data build process. The ICU specific data itself would
reside in source/data/xml/supplemental/supplementalMetadata.xml ( a new file ),
and would require some additional logic on the CLDR side in order to merge this.
In CLDR's ldmlICU.dtd:
<!ELEMENT icu:variantKeywordAlias EMPTY >
<!ATTLIST icu:variantKeywordAlias type NMTOKEN #IMPLIED >
<!ATTLIST icu:variantKeywordAlias replacement NMTOKEN #IMPLIED >
<!ELEMENT icu:localeAlias EMPTY >
<!ATTLIST icu:localeAlias type NMTOKEN #IMPLIED >
<!ATTLIST icu:localeAlias replacement NMTOKEN #IMPLIED >
Some example data:
<ldml>
<icu:localeAlias type="de_AT_PREEURO" replacement="de_AT@currency=ATS"/>
<icu:localeAlias type="C" replacement="en_US_POSIX"/>
<icu:variantKeywordAlias type="EURO" replacement="@currency=EUR"/>
<icu:variantKeywordAlias type="PINYIN" replacement="@collation=pinyin"/>
</ldml>
3). The end result is that all relevant canonicalization data would be available
in source/data/misc/metadata.txt ( whether it came from CLDR or from the ICU
specific extensions ), and could then be used during the canonicalization
instead of the hard-coded tables.
