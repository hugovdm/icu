# Territory & Region APIs

### ICU Ticket #8347 (Original ICU4J APIs - Tech Preview in ICU 4.8) & ICU Ticket #8451 ( Port ICU4J APIs to ICU4C ) - Ticket #9587 ( Move ICU4J APIs to draft status )

In order to provide information about territories and containment, this proposal
attempts to not only provide containment APIs, but also provides a framework by
which other territory and region based information can be provided to the user.

### Terminology

A Unicode Region Code (aka Unicode Region Subtag) is defined in Unicode LDML
(<http://unicode.org/reports/tr35/#Unicode_Language_and_Locale_Identifiers>),
based on BCP47. There are different types of Region Codes that are important to
distinguish:
**Macroregion** - A code for a "macro geographical (continental) region,
geographical sub-region, or selected economic and other grouping" as defined in
UN M.49 (<http://unstats.un.org/unsd/methods/m49/m49regin.htm>). These are
typically 3-digit codes, but contain some 2-letter codes, such as the LDML code
QO added for Outlying Oceania. Macroregions are to be represented in ICU by one
of three region types: WORLD ( region code 001 ),
CONTINENTS ( regions contained directly by WORLD ), and SUBCONTINENTS ( things
contained directly by a continent ).
**Territory** - A Region that is not a Macroregion. These are typically codes
for countries, but also include areas that are not separate countries, such as
the code "AQ" for Antarctica or the code "HK" for Hong Kong (SAR China).
Overseas dependencies of countries may or may not have separate codes. The codes
are typically 2-letter codes, but BCP47 allows for the use of 3-digit codes in
the future.
**Grouping -** A widely understood grouping of territories that has a well defined membership such that a region code has been assigned for it. Some of these are UNM.49 codes that do't fall into the world/continent/sub-continent hierarchy, while others are just well known groupings that have their own region code. Region "EU" (European Union) is one such region code that is a grouping. Groupings will never be returned by the getContainingRegion() API, since a different type of region ( WORLD, CONTINENT, or SUBCONTINENT ) will always be the containing region instead.
**Unknown Region** - The code ZZ is defined by Unicode LDML for use to indicate
that the Region is unknown, or that the value supplied as a region was invalid.
**Deprecated** - Region codes that have been defined in the past but are no
longer in modern usage, usually due to a country splitting into multiple
territories or changing its name.

### New Class - Region

The following new class and methods are proposed for inclusion in ICU:

### ICU4J - com.ibm.icu.util.Region

Parent class - java.lang.Object
**Nested Class Summary**
**RegionType** - An enumeration containing the various types of regions. For now
it will just contain the following types as in the terminology statement above :
WORLD, CONTINENT, SUBCONTINENT, TERRITORY, GROUPING, DEPRECATED, or UNKNOWN.
This type could be extended in the future to cover other types of groupings,
such as states, provinces, etc, provided that such data was contained in the
CLDR.

#### Factory Methods Summary

public static Region getInstance(String region_code)
Returns a Region using the given region code. The region code can be either
2-letter ISO code, 3-letter ISO code, UNM.49 numeric code, or other valid
Unicode Region Code as defined by the CLDR. The function ULocale.getCountry()
should return suitable input for this function. If the region code is NULL or
not recognized, the appropriate exception ( NullPointerException or
IllegalArgumentException, respectively ) will be thrown.
public static Region getInstance(int code)
Returns a Region using the given numeric region code. If the numeric region code
is not recognized, an illegal argument exception will be thrown.

#### Method Summary

public static Set<Region> getAvailable(RegionType type)
Returns an unmodifiable set of all known regions (as defined by ICU's territory
containment data) that match the given type.
public Region getContainingRegion( )
Return the region that geographically contains this region. Returns NULL if this
region is code "001" (World) or "ZZ" (Unknown region). For example, calling this
method with region "IT" (Italy) returns the region "039" (Southern Europe).
public Region getContainingRegion( RegionType type )
Return the region that geographically contains this region and matches the given
type, moving multiple steps up the containment chain if necessary. Returns NULL
if no containing region can be found that matches the given type. ( Note: The
RegionTypes = "GROUPING", "DEPRECATED", or "UNKNOWN" are not appropriate for use
in this API. An IllegalArgumentException will be thrown in this case. ) For
example, calling this method with region "IT" (Italy) for type "CONTINENT"
returns the region "150" ( Europe ).
public Set<Region> getContainedRegions()
Return an unmodifiable set containing all the regions that are immediate
children of this region in the region hierarchy. These returned regions could be
either macro regions, territories, or a mixture of the two, depending on the
containment data as defined in CLDR. This API may return an empty set if this
region doesn't have any sub-regions. For example, calling this method with
region "150" (Europe) returns a set containing the various sub regions of Europe
- "039" (Southern Europe) - "151" (Eastern Europe) - "154" (Northern Europe) and
"155" (Western Europe).
public Set<Region> getContainedRegions( RegionType type)
Return an unmodifiable set containing all the regions that are children of this
region anywhere in the region hierarchy and match the given type. This API may
return an empty set if this region doesn't have any sub-regions that match the
given type. For example, calling this method with region "150" (Europe) and type
"TERRITORY" returns a set containing all the territories in Europe ( "FR"
(France) - "IT" (Italy) - "DE" (Germany) etc. )
public boolean contains(Region other)
Returns true if this region contains the supplied other region anywhere in the
region hierarchy.
public List<Region> getPreferredValues()
For deprecated regions, return a list of the regions that are the preferred
replacement regions for this region. Returns null for a non-deprecated region.
For example, calling this method with region "SU" (Soviet Union) would return a
list of the regions containing "RU" (Russia), "AM" (Armenia), "AZ" (Azerbaijan),
etc...
public String toString()
Returns a string representation of this object, which is the region's canonical
Unicode region code.
public int getNumericCode()
Return the region's numeric code. Returns UNDEFINED_NUMERIC_CODE (-1) if the
given region does not have a numeric code assigned to it.
public Region.Type getType()
Returns the region type of this region.
public int compareTo(Region other)
Used to implement the Comparable interface for regions. A region's natural
ordering is alphabetical based on it's string representation ( canonical ID ).

#### Equivalent APIs ICU4C

The scope of this design is to provide a C++ class interface only, without
implementing a pure C language interface. The enumeration for region types will
be a C enumeration "URegionType", in order to allow us to add a pure C interface
in the future if we should decide that it is necessary.
Territory Class Reference
The C++ class name shall be "Region".
#include <unicode/region.h>
**Public Enumerations**
enum URegionType {
/\*\*
\* Type representing the unknown region.
\*/
URGN_UNKNOWN,
/\*\*
\* Type representing a territory.
\*/
URGN_TERRITORY,
/\*\*
\* Type representing the whole world.
\*/
URGN_WORLD,
/\*\*
\* Type representing a continent.
\*/
URGN_CONTINENT,
/\*\*
\* Type representing a sub-continent.
\*/
URGN_SUBCONTINENT,
/\*\*
\* Type representing a grouping of territories that is not to be used in
\* the normal WORLD/CONTINENT/SUBCONTINENT/TERRITORY containment tree.
\*/
URGN_GROUPING,
/\*\*
\* Type representing a region whose code has been deprecated, usually
\* due to a country splitting into multiple territories or changing its name.
\*/
URGN_DEPRECATED,
}
**Public Member Functions**
UBool operator == (const Region &that) const
Returns true if the two regions are equal.
UBool operator != (const Region &that) const
Returns true if the two regions are NOT equal; that is, if operator ==() returns
false.
const Region \*getContainingRegion( )
Returns a pointer to the region that contains this region. Returns NULL if this
region is code "001" (World) or "ZZ" (Unknown region). For example, calling this
method with region "IT" (Italy) returns the region "039" (Southern Europe).
const Region\* getContainingRegion( URegionType type )
Return a pointer to the region that geographically contains this region and
matches the given type, moving multiple steps up the containment chain if
necessary. Returns NULL if no containing region can be found that matches the
given type. ( Note: The RegionTypes = "GROUPING", "DEPRECATED", or "UNKNOWN" are
not appropriate for use in this API. NULL will be returned in this case. ) For
example, calling this method with region "IT" (Italy) for type "CONTINENT"
returns the region "150" ( Europe ).
StringEnumeration \*getContainedRegions()
Return an enumeration over the IDs of all the regions that are immediate
children of this region in the region hierarchy. These returned regions could be
either macro regions, territories, or a mixture of the two, depending on the
containment data as defined in CLDR. This API may return NULL if this region
doesn't have any sub-regions. For example, calling this method with region "150"
(Europe) returns an enumeration containing the various sub regions of Europe -
"039" (Southern Europe) - "151" (Eastern Europe) - "154" (Northern Europe) and
"155" (Western Europe).
StringEnumeration \*getContainedRegions( URegionType type)
Returns an enumeration over the IDs of all the regions that are children of this
region anywhere in the region hierarchy and match the given type. This API may
return an empty set if this region doesn't have any sub-regions that match the
given type. For example, calling this method with region "150" (Europe) and type
"TERRITORY" returns a set containing all the territories in Europe ( "FR"
(France) - "IT" (Italy) - "DE" (Germany) etc. )
UBool contains(const Region &other) const
Returns true if this region contains the supplied other region anywhere in the
region hierarchy.
StringEnumeration \*getPreferredValues()
For deprecated regions, return an enumeration over the IDs of the regions that
are the preferred replacement regions for this region. Returns null for a
non-deprecated region. For example, calling this method with region "SU" (Soviet
Union) would return a list of the regions containing "RU" (Russia), "AM"
(Armenia), "AZ" (Azerbaijan), etc...
const char \*getRegionCode()
Return this region's canonical region code.
int32_t getNumericCode()
Return this region's numeric code. Returns UNDEFINED_NUMERIC_CODE (-1) if the
given region does not have a numeric code assigned to it.
URegionType getType()
Returns the region type of this region.
**Factory Methods**
static const Region \*getInstance( const char \*region_code, UErrorCode &status
)
Returns a pointer to a Region using the given region code. The region code can
be either 2-letter ISO code, 3-letter ISO code, UNM.49 numeric code, or other
valid Unicode Region Code as defined by the LDML specification. The identifier
will be canonicalized internally using the supplemental metadata as defined in
the CLDR. If the region code is NULL or not recognized, the appropriate error
code will be set ( U_ILLEGAL_ARGUMENT_ERROR )
static const Region \*getInstance (int32_t code, UErrorCode &status )
Returns a pointer to a Region using the given numeric region code. If the
numeric region code is not recognized, the appropriate error code will be set (
U_ILLEGAL_ARGUMENT_ERROR )
**Static Public Member Functions**
static StringEnumeration \* getAvailable(RegionType type)
Returns an enumeration over the IDs of all known regions that match the given
type.
