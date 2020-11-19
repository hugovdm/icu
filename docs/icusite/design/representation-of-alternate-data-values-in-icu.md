# Representation of alternate data values in ICU

Trying to come up with a uniform way to allow access to alternate values (
"@alt=something" ) from CLDR within ICU's resource bundles. There are a number
of different scenarios, both in terms of usage and in desired lookup style.
**Current Places in CLDR where we have alternate values:**

1.  Short or variant spellings of language names - such as:

<language type="az">Azerbaijani</language>
<language type="az" alt="short">Azeri</language>

2. Stand-alone forms of script names - such as:
<script type="Hans">Simplified</script>
<script type="Hans" alt="stand-alone">Simplified Han</script>
3. Short or variant forms of country names - such as:
<territory type="HK">Hong Kong SAR China</territory>
<territory type="HK" alt="short">Hong Kong</territory>
4. Variant forms of the day period names - such as:
<dayPeriodWidth type="wide">
<dayPeriod type="am">AM</dayPeriod>
<dayPeriod type="am" alt="variant">a.m.</dayPeriod>
<dayPeriod type="noon">noon</dayPeriod>
<dayPeriod type="pm">PM</dayPeriod>
<dayPeriod type="pm" alt="variant">p.m.</dayPeriod>
</dayPeriodWidth>
5. Short forms of unit names - such as:
<unit type="second">
<unitPattern count="one">{0} second</unitPattern>
<unitPattern count="one" alt="short">{0} sec</unitPattern>
<unitPattern count="other">{0} seconds</unitPattern>
<unitPattern count="other" alt="short">{0} secs</unitPattern>
</unit>
Possible ways you might want to do the lookup for item in locale: foo-bar-zip
alt=short

A. (“phonebook” style) (this is what we get if we use the existing, internal
lookup function that takes a path to the requested item, using it once with
“foo/bar/zip%short” and if it fails using the function again with “foo/bar/zip”)

foo-bar-zip alt=short

foo-bar alt=short

foo alt=short

foo-bar-zip

foo-bar

foo

B.("nice to have" style, where the alternate is a nice to have, but not as
important as getting a value from the locale you want

foo-bar-zip alt=short

foo-bar-zip

foo-bar alt=short

foo-bar

foo alt=short

foo

C. ( "exact match" style - places where the non-alt version is not a good
fallback for the alt version, i.e. the alt version has specific semantics that
you want to enforce in the lookup.

foo-bar-zip alt=short

foo-bar alt=short

foo alt=short

Likely desired lookups for each scenario above:

1.  Short or variant spellings of language names - A or B
2.  Stand-alone forms of script names - C
3.  Short or variant forms of country names - A or B
4.  Variant forms of the day period names - B
5.  Short forms of unit names - A

PROPOSED structures for the data:

**Original Proposal ( John & Yoshito )**

en{

Countries{

GY{"Guyana"}

HK{

default{"Hong Kong SAR China"}

short{"Hong Kong"}

}

HM{"Heard Island and McDonald Islands"}

}

}

Downside: Lack of forward compatibility - difficulty in mapping xpath to value
for LDML2ICUConversion

**MARK's idea:**

**en{**
**Countries{**
**GY{"Guyana"}**
**HK{"Hong Kong SAR China"}**
**HK_alt{**
**short{"Hong Kong"}**
**}**
**HM{"Heard Island and McDonald Islands"}**
**}**
**}**
**Downside: Difficult to count/enumerate values, because you have to "sift out" the alternate values.**
**MARKUS's idea**
en{**
Countries{**
GY{"Guyana"}**
HK{"Hong Kong SAR China"}**
HK%short{"Hong Kong”}**
HM{"Heard Island and McDonald Islands"}**
}**
}**
**Downside: Difficult to count/enumerate values, because you have to "sift out" the alternate values.**
**John's UPDATED Proposal:**
**en{**
**Countries{**
**GY{"Guyana"}**
**HK{"Hong Kong SAR China"}**
**HM{"Heard Island and McDonald Islands"}**
**}**
Countries%short{
HK{"Hong Kong"}
}
**}**
