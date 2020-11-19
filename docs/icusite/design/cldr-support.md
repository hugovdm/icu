# CLDR Support

[TOC]

The following are features based on CLDR 1.8 that were only partially completed
in ICU 4.4, and need to be completed in the near future.

## Index Characters

For letter indexes, such as used in a phone.

### Done so far

The data was converted, and is present in the locales. Example:

ExemplarCharactersIndex{"\[A B C D E F G H I J K L M N O P Q R S T U V W X Y
Z\]"}

There is already API in com.ibm.icu.text.IndexCharacters, which synthesizes the
list of index characters.

### TODO

*   ICU4J: wire up the code so that it gets the data from resources, and only
    synthesizes if there is no data available. Add tests.
*   ICU4C: Propose parallel API, port from ICU4J.

---

## List patterns

For composing a list such as "A, B et C".

### Done so far

The data was converted, and is present in the locales. Example:

listPattern{

standard{

2{"{0} en {1}"}

end{"{0} en {1}"}

}

}

The code is pretty simple; see UTS #35 for how it is to work.

### TODO

*   ICU4J: Propose API, code, tests.
*   ICU4C: Propose API, code, tests.

---

## Language Matching Data

For matching requested locales against supported locales.

### Done so far

The data was converted, and is present in supplementalData.txt. Example:

languageMatching{

written{

{

"no",

"nb",

"100",

}

There is code in ICU4J (com.ibm.icu.util.LocaleMatcher). It needs to be updated
to use the CLDR data, with additional tests.

### TODO

*   ICU4J: Update code, add tests.
*   ICU4C: Propose API, code, tests.

---

## Day Periods

For use with calendars that don't use periods of 12 hours with 12 hour clocks.
This one is a bit trickier; we ran into some issues when we started supporting,
and since the data was gathered through the Easy Steps, the translators didn't
have examples in front of them. So we don't have the same level of confidence in
the data, and we'd need to review it.

### Done so far

There is code to convert the data in LDML2ICUConverter, but it was suppressed
late in the cycle. The data is in two parts: the rules for the locales (in
supplemental), and the translations in in the locales (in main).

### TODO

This needs to be a longer-term goal, because there is more work to be done.

*   Design: consider having additional pattern letter to choose between am/pm
    style, and other day periods.
*   Data: review and vet; modify LDML2ICUConverter to convert.
*   ICU4J: Propose API, code.
*   ICU4C: Propose API, code.
