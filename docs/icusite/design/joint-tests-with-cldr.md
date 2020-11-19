# Joint Tests with CLDR

Update 2018-oct-22:

*   Until ICU 63, this was an ICU BRS task called "Copy shared CLDR tests to ICU
    (eg LocaleMatcherTest)".
*   We used to assign it to Mark who had done it at some point.
*   The instructions were not sufficient for anyone else to do this task.
*   Mark didn't get around to it for several releases.
*   Just before ICU 63, he said we can delete this task.
*   Markus removed it from the BRS Master.

There are two tests that historically have code that is shared between CLDR and
ICU:

*   LocaleMatcher
*   Transliteration

When ICU transitioned to JUnit, this stopped working. So for now, CLDR can't run
those tests in its environment, and we are dependent on running the tests after
integration with ICU.

The new LocaleMatcher has data-driven tests, which are then easy to port. We can
try to get the Transliteration tests working in both environments, or try to
have at least some subset of the tests be data-driven.
