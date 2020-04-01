// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License

#if !UCONFIG_NO_FORMATTING

#include "unitsdata.h"        // for ConversionRateInfo, UnitPreference
#include "charstr.h"          // for CharString
#include "cmemory.h"          // for MaybeStackVector
#include "intltest.h"         // for TESTCASE_AUTO, IntlTest, TESTCASE_AUTO...
#include "unicode/measunit.h" // for MeasureUnit
#include "unicode/testlog.h"  // for IcuTestErrorCode
#include "unicode/umachine.h" // for UBool
#include "unicode/unistr.h"   // for operator+, UnicodeString
#include "unicode/utypes.h"   // for U_MEMORY_ALLOCATION_ERROR

class UnitsDataTest : public IntlTest {
  public:
    UnitsDataTest() {}

    void runIndexedTest(int32_t index, UBool exec, const char *&name, char *par = NULL);

    void testGetConversionRateInfo();
    void testGetUnitsData();
};

extern IntlTest *createUnitsDataTest() { return new UnitsDataTest(); }

void UnitsDataTest::runIndexedTest(int32_t index, UBool exec, const char *&name, char * /*par*/) {
    if (exec) { logln("TestSuite UnitsDataTest: "); }
    TESTCASE_AUTO_BEGIN;
    TESTCASE_AUTO(testGetConversionRateInfo);
    TESTCASE_AUTO(testGetUnitsData);
    TESTCASE_AUTO_END;
}

void UnitsDataTest::testGetConversionRateInfo() {
    const int MAX_NUM_RATES = 5;
    struct {
        // The source unit passed to getConversionRateInfo.
        const char *sourceUnit;
        // The target unit passed to getConversionRateInfo.
        const char *targetUnit;
        // Expected: units whose conversion rates are expected in the results.
        const char *expectedOutputs[MAX_NUM_RATES];
    } testCases[]{
        {"meter", "kilometer", {"meter", NULL, NULL, NULL, NULL}},
        {"liter", "gallon", {"liter", "gallon", NULL, NULL, NULL}},
        {"watt", "horsepower", {"watt", "horsepower", NULL, NULL, NULL}},
        {"mile-per-hour", "dekameter-per-hour", {"mile", "hour", "meter", NULL, NULL}},

        // Sequence
        {"stone-and-pound", "ton", {"pound", "stone", "ton", NULL, NULL}},

        // Compound
        {"centimeter-per-square-milligram",
         "inch-per-square-ounce",
         {"meter", "gram", "inch", "ounce", NULL}},
        {"therm-us",
         "kilogram-square-meter-per-square-second",
         {"therm-us", "kilogram", "meter", "second", NULL}},

        // "Reciprocal" example: consumption and consumption-inverse
        {"liter-per-100-kilometer",
         "mile-per-gallon",
         {"liter", "100-kilometer", "mile", "gallon", NULL}},
    };
    for (const auto &t : testCases) {
        logln("---testing: source=\"%s\", target=\"%s\"", t.sourceUnit, t.targetUnit);
        IcuTestErrorCode status(*this, "testGetConversionRateInfo");

        MaybeStackVector<MeasureUnit> units;
        MeasureUnit *item = units.emplaceBack(MeasureUnit::forIdentifier(t.sourceUnit, status));
        if (!item) {
            status.set(U_MEMORY_ALLOCATION_ERROR);
            return;
        }
        item = units.emplaceBack(MeasureUnit::forIdentifier(t.targetUnit, status));
        if (!item) {
            status.set(U_MEMORY_ALLOCATION_ERROR);
            return;
        }

        MaybeStackVector<ConversionRateInfo> conversionInfo = getConversionRatesInfo(units, status);
        if (status.errIfFailureAndReset("getConversionRatesInfo({<%s>, <%s>}, ...)", t.sourceUnit,
                                        t.targetUnit)) {
            continue;
        }

        int countExpected;
        for (countExpected = 0; countExpected < MAX_NUM_RATES; countExpected++) {
            auto expected = t.expectedOutputs[countExpected];
            if (expected == NULL) break;
            // Check if this conversion rate was expected
            bool found = false;
            for (int i = 0; i < conversionInfo.length(); i++) {
                auto cri = conversionInfo[i];
                if (strcmp(expected, cri->sourceUnit.data()) == 0) {
                    found = true;
                    break;
                }
            }
            assertTrue(UnicodeString("<") + expected + "> expected", found);
        }
        assertEquals("number of conversion rates", countExpected, conversionInfo.length());

        // Convenience output for debugging
        for (int i = 0; i < conversionInfo.length(); i++) {
            ConversionRateInfo *cri = conversionInfo[i];
            logln("* conversionInfo %d: source=\"%s\", baseUnit=\"%s\", factor=\"%s\", "
                  "offset=\"%s\"",
                  i, cri->sourceUnit.data(), cri->baseUnit.data(), cri->factor.data(),
                  cri->offset.data());
        }
    }
}

// We test "successfully loading some data", not specific output values, since
// this would duplicate some of the input data. We leave end-to-end testing to
// take care of that. Running `intltest` with `-v` will print out the loaded
// output for easy visual inspection.
void UnitsDataTest::testGetUnitsData() {
    struct {
        // Input parameters
        const char *outputRegion;
        const char *usage;
        const char *inputUnit;
    } testCases[]{
        {"US", "fluid", "centiliter"},
        {"BZ", "weather", "celsius"},
        {"ZA", "road", "yard"},
        {"XZ", "zz_nonexistant", "dekagram"},
    };
    for (const auto &t : testCases) {
        logln("---testing: region=\"%s\", usage=\"%s\", inputUnit=\"%s\"", t.outputRegion, t.usage,
              t.inputUnit);
        IcuTestErrorCode status(*this, "testGetUnitsData");
        MeasureUnit inputUnit = MeasureUnit::forIdentifier(t.inputUnit, status);
        if (status.errIfFailureAndReset("MeasureUnit::forIdentifier(\"%s\", ...)", t.inputUnit)) {
            continue;
        }

        CharString category;
        MeasureUnit baseUnit;
        MaybeStackVector<ConversionRateInfo> conversionInfo;
        MaybeStackVector<UnitPreference> unitPreferences;
        getUnitsData(t.outputRegion, t.usage, inputUnit, category, baseUnit, conversionInfo,
                     unitPreferences, status);
        if (status.errIfFailureAndReset("getUnitsData(\"%s\", \"%s\", \"%s\", ...)", t.outputRegion,
                                        t.usage, t.inputUnit)) {
            continue;
        }

        logln("* category: \"%s\", baseUnit: \"%s\"", category.data(), baseUnit.getIdentifier());
        assertTrue("category filled in", category.length() > 0);
        assertTrue("baseUnit filled in", strlen(baseUnit.getIdentifier()) > 0);
        assertTrue("at least one conversion rate obtained", conversionInfo.length() > 0);
        for (int i = 0; i < conversionInfo.length(); i++) {
            ConversionRateInfo *cri;
            cri = conversionInfo[i];
            logln("* conversionInfo %d: source=\"%s\", baseUnit=\"%s\", factor=\"%s\", offset=\"%s\"", i,
                  cri->sourceUnit.data(), cri->baseUnit.data(), cri->factor.data(), cri->offset.data());
            assertTrue("ConversionRateInfo has source, baseUnit, and factor",
                       cri->sourceUnit.length() > 0 && cri->baseUnit.length() > 0 &&
                           cri->factor.length() > 0);
        }
        assertTrue("at least one unit preference obtained", unitPreferences.length() > 0);
        for (int i = 0; i < unitPreferences.length(); i++) {
            UnitPreference *up;
            up = unitPreferences[i];
            logln("* unitPreference %d: \"%s\", geq=%f, skeleton=\"%s\"", i, up->unit.data(), up->geq,
                  up->skeleton.data());
            assertTrue("unitPreference has unit", up->unit.length() > 0);
        }
    }
}

#endif /* #if !UCONFIG_NO_FORMATTING */
