# Coverage

Goals: 100% coverage of API functions via test code; 85% line coverage.

We currently fall short of the API coverage. We need to monitor it and improve
it.

What does "API" mean?

*   Any *function or method* that is in a public package or header, has public
    or protected visibility (not package-private, not private), and is not
    @internal nor @system.
*   Also any publicly reachable implementation or override of any such method.

"Coverage" usually means "invoked somehow while running the tests", including
indirectly. It would be better if API functions were called *directly from test
code*.

In addition, we should really cover every method in the library, public or not.
If a method is not covered, we can't be sure that it works. If every method is
covered, then we need not distinguish API+implementation+override from others
(i.e., easy to measure). On the other hand, we do not aspire to 100% line
coverage, and therefore may decide not to require 100% implementation method
coverage.

Ideally, we would also "cover" public API constants, typedefs, and macros, but
those are hard to measure. They would probably require static code analysis.

If there is code in the library that is only used for tests, tools, CLDR,
Unicode Tools, ... then we should move it out of the library. This will not only
improve library code coverage, but also reduce the size of the library. Do not
move UnicodeMap or Relation, because Mark wants to eventually make those public
(may need to discuss these).

Priorities for improving coverage where missing:

*   High: Public not-deprecated functions/methods and their direct
    implementation/override methods
*   Next: Implementation methods called by those
*   Last: Deprecated methods

See the Trac query for [tickets with the "coverage"
keyword](http://bugs.icu-project.org/trac/query?status=!closed&keywords=~coverage).

We might use exceptions for hard-to-cover or somehow irrelevant methods.
However, exception mechanisms are specific to each coverage tool (TODO: true?),
and it might be more efficient in the end to try to cover as much as physically
possible.

## Java

We have a continuous-build bot that will automatically fail if new methods are
added to the library without test coverage:
<http://bugs.icu-project.org/trac/build/icu4j-cov> (Since 2016-apr-29, [ICU
ticket #12445](http://bugs.icu-project.org/trac/ticket/12445).)

*   ICU4J ant target 'coverageJaCoCo' calls the JacocoReportCheck tool.
*   The tool extracts method coverage data from <icu4j>/out/jacoco/report.xml.
*   The tool reads an exclusion set from <icu4j>/coverage-exclusion.txt
    *   For non-default data loading etc. it might be necessary to run with a
        modified exclusion set:
    *   ant -Djacoco.exclusion.txt=/path/to/my-exclusion.txt coverageJaCoCo
*   When a method is not covered by ICU4J unit test, and not included in the
    exclusion set, then the build target prints the method class/signature and
    fails.
*   When a method is covered by ICU4J unit test, but included in the exclusion
    set, then the build target prints a warning including the method
    class/signature, but does not fail.

Therefore:

*   Whenever you add a new method in an ICU4J library class without any test
    case hitting the method, the check will fail.
*   You should add a test to improve the coverage
*   Only in exceptional cases should we need to add methods to
    coverage-exclusion.txt.
*   When we add test cases which cover methods in the exclusion set, then the
    build target prints out warning (it does not fail). We should remove newly
    covered methods from the exclusion set.

Download the report_html.zip from the latest build of the ["ICU4J trunk
coverage" build bot](http://bugs.icu-project.org/trac/build/icu4j-cov), unzip,
and open its index.html file. Look for Missed Methods. Drill down into packages
and classes.

Snapshot from around ICU 57: <http://icu-project.org/~yoshito/jacoco_57.1/#dn-j>

You can run the code coverage plug-in tool EclEmma which is based on JaCoCo. See
the section 'Test Coverage' in [Eclipse Setup for Java
Developers.](../setup/java/eclipse-setup-for-java-developers/index.md)

You can run the JaCoCo tool from ant: `ant coverageJaCoCo`. See the section
'Generating Test Code Coverage Report' in [Ant Setup for Java
Developers](../setup/java/ant/index.md) (download jar & set one env var).

See the email “ICU4J method coverage” from Yoshito 2016-mar-17 about ant
coverageJaCoCo failing with not-covered methods.

TODO: JaCoCo reports non-existent constructors in static-only classes as not
covered. What to do? (e.g.,
[CollationLoader.ASCII](http://icu-project.org/~yoshito/jacoco_57.1/com.ibm.icu.impl.coll/CollationLoader.java.html#L60))
[Test via
reflection?](http://stackoverflow.com/questions/4520216/how-to-add-test-coverage-to-a-private-constructor)

**Testing package private methods**:

We have been adding helper funcitons to
*icu4j/main/tests/framework/src/com/ibm/icu/dev/test/TestFmwk.java* which use
reflection to test common types of package private methods that are common
throughout the code base.

1.  Check that the default private constructor is private:
    *TestFmwk.checkDefaultPrivateConstructor(Class<?> classToBeTested) -
    TestFmwk.checkDefaultPrivateConstructor(String fullyQualifiedClassName)*

### Refactor ICU4J+CLDR+Unicode Tools

Before you move or remove internal (e.g., impl or dev) or @internal ICU4J
classes or methods, make sure that they are not used by CLDR and [Unicode
Tools](https://sites.google.com/site/unicodetools/home). Normally, those
projects use two pre-built jar files for ICU4J which makes their dependencies on
ICU4J not visible. Change their project settings as follows, then refactor, then
revert the project settings.

Eclipse workspace with projects for ICU, CLDR, and Unicode Tools.

Open Properties for the cldr-tools project.

*   "Java Build Path/Libraries": Remove icu4j.jar, utilities.jar.
*   "Java Build Path/Projects": Add icu4j-collate, icu4j-core, icu4j-core-tests,
    icu4j-test-framework, icu4j-tools, icu4j-translit, icu4j-translit-tests.

Open Properties for the cldr-unittest and unicodetools projects, make the same
changes.

If you move classes from one ICU package to another, you need to [update CLDR's
icu4j.jar](http://cldr.unicode.org/development/updating-icu-jar).

See also <https://sites.google.com/site/unicodetools/home/refactoring>

## C/C++

Go to the latest build of the ["icu4c trunk coverage" build
bot](http://bugs.icu-project.org/trac/build/icu4c-cov), scroll down to the
"gcov" section. Open a source file link in a new tab.

It shows line coverage but not method coverage. Might be tedious to track API
and other methods.

Does this merge the coverage from all test suites?
