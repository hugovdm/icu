# Updating Measure Unit with new CLDR data

This document explains how to update the C and JAVA version of the MeasureUnit
class with new CLDR data.

After syncing CLDR data with ICU do the following. This documentation assumes
that you are updating the MeasureUnit clases for ICU54.1

* In ICU4J check out
  $SVN_ROOT/main/tests/core/src/com/ibm/icu/dev/test/format/MeasureUnitTest.java
* Open MeasureUnitTest.java in
  [eclipse](http://site.icu-project.org/setup/java/eclipse-setup-for-java-developers).
* Near the top of the file, find the testZZZ unit test.

      public void testZZZ() {
          // desired code goes here
      }

  Use this test to verify the changes work as intended. This test will execute
  last and will run the desired code.
* Make sure DRAFT_VERSIONS at top of MeasureUnitTest.java is set correctly.
  These are the ICU versions that have draft methods.

<!--
FOR REVIEW: I don't know what this above testZZZ thing is about exactly. "Draft
methods" are also something I'm not familiar with, but perhaps those that would
touch such methods would know what they are. Do you have suggestions fo
clarifying the text above a bit? Else I leave it as is.
-->

Update MeasureUnit.java

* Change testZZZ to run generateConstants(“54”);  // ICU 54.
* Within eclipse run MeasureUnitTest.java by clicking on the green play button on menu bar.
* Copy the generated code from the eclipse console to the clipboard
* Open MeasureUnit.java in eclipse:
  $SVN_ROOT/main/classes/core/src/com/ibm/icu/util/MeasureUnit.java
* Look for line containing:

      // Start generated MeasureUnit constants

* Look for line containing:

      // End generated MeasureUnit constants

* Replace all the generated code in between with the contents of the clipboard
* Run the MeasureUnitTest.java to ensure that the new code is backward
  compatible. These compatibility tests are called something like
  TestCompatible53_1, which tests backward compatibility with ICU 53.
* Create a compatibility test for ICU 54.1. Change testZZZ to run
  generateBackwardCompatibilityTest(“54.1”)
* Run tests.
* Copy generated test from eclipse console into MeasureUnitTest.java
* Run tests again to ensure that new code is backward compatible with itself

Update ICU4C:

* checkout ICU4C

Update measunit.h:

* Change testZZZ to run generateCXXHConstants(“54”);  // ICU 54.
* Within eclipse run MeasureUnitTest.java by clicking on the green play button
  on menu bar.
* Copy the generated code from the eclipse console to the clipboard
* switch to ICU4C SVN repository
* Open $SVN_ROOT/source/i18n/unicode/measunit.h. Look for line containing: 

      // Start generated createXXX methods

* Look for line:

      // End generated createXXX methods

* Replace all the generated code in between with the contents of the clipboard

Update measunit.cpp:

* Change testZZZ to run generateCXXConstants();
* Within eclipse run MeasureUnitTest.java by clicking on the green play button on menu bar.
* Copy the generated code from the eclipse console to the clipboard
* switch to ICU4C SVN repository
* Open $SVN_ROOT/source/i18n/measunit.cpp. Look for line containing:

      // Start generated code

* Look for lines

      // End generated code

* Replace all the generated code in between with the contents of the clipboard

// Update measfmt.cpp -- Update the value for MEAS_UNIT_COUNT - no longer necessary

Run C++ tests:

* Run `./intltest format/MeasureFormatTest` from `test/intltest` to ensure new
  code is backward compatible.
* Create a compatibility test for ICU 54.1. Change testZZZ in eclipse to run
  generateCXXBackwardCompatibilityTest(“54.1”)
* Run tests.
* Copy generated test from eclipse console into
  $SVN_ROOT/source/test/intltest/measfmttest.cpp. Make other necessary changes
  to make test compile. You can find these changes by searching for
  `TestCompatible53_1()`
* Run tests again to ensure that new code is backward compatible with itself

Finishing changes

These last changes are necessary to permanently record the ICU version number of
any new measure units. Without these changes any new functions for this release
will be considered new for the next release too.

* Change testZZZ to run updateJAVAVersions(“54”);
* Within eclipse run MeasureUnitTest.java by clicking on the green play button
  on menu bar.

Copy the generated code in eclipse console and append it to the values of the
JAVA_VERSIONS variable near the top of MeasureUnitTest.java. Important: what you
are copying are just the new functions for the current ICU version, in this case
54. Therefore append, do not replace.
