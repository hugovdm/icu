# Copy Shared Tests

Certain tests are structured so that they can be shared between CLDR and ICU.
That allows the tests to be run in CLDR earlier in the development process. The
tests include:

*   LocaleMatcherTest
*   TransformTest (to be cleaned up)

Copy all of these from CLDR to ICU, and sanity check the differences, then check
in.
