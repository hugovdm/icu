# Root locale and fallback

### Issues

1.  [ULocale#getFallback()](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#getFallback%28%29)
    never get to ULocale.ROOT
    ([ticket#6673](http://bugs.icu-project.org/trac/ticket/6673)), instead, the
    final locale is empty locale (new ULocale("")), then null.
    *   *Question from Markus: Should any of the fallbacks ever get to null?
        Should they not stop at the root locale?*
2.  [ULocale.getFallback(String)](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#getFallback%28java.lang.String%29)
    never get to "root", the final fallback is "".
3.  [ULocale.getFallback(String)](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#getFallback%28java.lang.String%29)
    may return a locale string ending empty segment. For example, "en__POSIX" ->
    "en_" -> "en" -> ""

### Design Questions

1.  What is the canonical representation of root locale?
    1.  Three possible options - "" (empty string), "root", or "und"
        (undetermined)
    2.  JDK 1.6 added Locale.ROOT using empty language - new Locale("", "", "").
    3.  {Yoshito} I prefer "" (empty string) for several reasons
        1.  Logical (no special handling)
        2.  Same with Java
        3.  However, backward compatibility concern - can we change ULocale.ROOT
            from ULocale("root") to ULocale("") now?
2.  Normalization
    1.  What should be done in the locale constructor?
    2.  What is the expected behavior of
        [ULocale.canonicalize(String)](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#canonicalize%28java.lang.String%29)
        ?
    3.  {Yoshito} canonicalize should normalize casing and following mappings
        1.  Deprecated ICU locales
            1.  fr_FR_PREEURO -> fr_FR@currency=FRF
            2.  hi__DIRECT -> hr@collation=direct
            3.  other variants mapped to keywords
        2.  Grandfathered BCP 47 tags
            1.  art_LOJBAN -> jbo
            2.  zh_HAKKA / zh__HAKKA -> hak
            3.  other BCP47 grandfathered tag - preferred mapping
        3.  POSIX
            1.  C -> en_US_POSIX
        4.  .NET names
            1.  az_AZ_CYRL -> az_Cyrl_AZ
            2.  zh_CHS -> zh_Hans
        5.  Common mistakes
            1.  three-letter language codes (eng) that have two letter
                equivalents
            2.  three-letter region codes (xxx) that have two letter equivalents
            3.  three-digit codes (813) that have two letter equivalents
            4.  swapping script and region code (see also .NET names above)
        6.  Deprecated codes
            1.  iw -> he
            2.  some others

### Proposed Changes

1.  ULocale.ROOT
    1.  current: new ULocale("root");
    2.  proposed:new ULocale("");
2.  [ULocale#getFallback()](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#getFallback%28%29)
    1.  current: ULocale("en__POSIX") -> ULocale("en_") -> ULocale("en") ->
        ULocale("") -> null
    2.  proposed: ULocale("en__POSIX") -> ULocale("en") -> ULocale("") -> null
3.  [ULocale.getFallback(String)](http://www.icu-project.org/apiref/icu4j/com/ibm/icu/util/ULocale.html#getFallback%28java.lang.String%29)
    1.  current: "en__POSIX" -> "en_" -> "en" -> "" -> ""
    2.  proposed: "en_POSIX" -> "en" -> "" -> null?

### Conclusions

A conference call was held for discussing these design questions on 2009-11-17.
Attendees: Mark, Markus, Doug, Umesh and Yoshito.
Our conclusions are below:

*   ULocale.ROOT.toString() == "", not "root"
*   BCP47
    *   ULocale.ROOT to BCP47 "und"
    *   Locale.ROOT to BCP47 "und"
    *   BCP47 "und" to ULocale ""
*   getFallback() chops off from nominal form not from canonical form, never
    leaves trailing underscore, just works on '_'-separated strings.
*   class ULocale {
    ...
    static ULocale getCanonicalInstance(String); // Factory
    ULocale getCanonicalEquivalent(); // Uses cached internal pointer
    ...
    };
*   Canonicalize "und" to "". ULocale("und-DE") will have
    *   lang = "und", region = "DE"
    *   canonical lang = "", canonical region = "DE"
*   Resource bundles
    *   root.res
        *   de.res
            *   de-DE.res
        *   root-DE.res
*   en -> "" -> null?
    *   Yes in Java ULocale#getFallback() - e.g. ULocale("en") -> ULocale("") ->
        null
    *   No in Java ULocale#getFallback(String) - e.g. "en" -> "" -> ""
    *   No in C++
    *   Document well.
