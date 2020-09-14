// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html


package com.ibm.icu.dev.test.impl;

import com.ibm.icu.dev.test.TestUtil;
import com.ibm.icu.impl.Pair;
import com.ibm.icu.impl.units.ComplexUnitsConverter;
import com.ibm.icu.impl.units.ConversionRates;
import com.ibm.icu.impl.units.MeasureUnitImpl;
import com.ibm.icu.impl.units.UnitConverter;
import com.ibm.icu.impl.units.UnitsRouter;
import com.ibm.icu.util.Measure;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;


public class UnitsTest {

    public static boolean compareTwoBigDecimal(BigDecimal expected, BigDecimal actual, BigDecimal delta) {
        BigDecimal diff =
                expected.abs().compareTo(BigDecimal.ZERO) < 1 ?
                        expected.subtract(actual).abs() :
                        (expected.subtract(actual).divide(expected, MathContext.DECIMAL128)).abs();

        if (diff.compareTo(delta) == -1) return true;
        return false;
    }

    @Test
    public void testComplexUnitConverterSorting() {

        MeasureUnitImpl source = MeasureUnitImpl.forIdentifier("meter");
        MeasureUnitImpl target = MeasureUnitImpl.forIdentifier("inch-and-foot");
        ConversionRates conversionRates = new ConversionRates();

        ComplexUnitsConverter complexConverter = new ComplexUnitsConverter(source, target, conversionRates);
        List<Measure> measures = complexConverter.convert(BigDecimal.valueOf(10.0));

        assertEquals(measures.size(), 2);
        assertEquals("inch-and-foot unit 0", "foot", measures.get(0).getUnit().getIdentifier());
        assertEquals("inch-and-foot unit 1", "inch", measures.get(1).getUnit().getIdentifier());

        assertTrue("inch-and-foot value 0", compareTwoBigDecimal(BigDecimal.valueOf(32), BigDecimal.valueOf(measures.get(0).getNumber().doubleValue()), BigDecimal.valueOf(0.0001)));
        assertTrue("inch-and-foot value 1", compareTwoBigDecimal(BigDecimal.valueOf(9.7008), BigDecimal.valueOf(measures.get(1).getNumber().doubleValue()), BigDecimal.valueOf(0.0001)));
    }


    @Test
    public void testExtractConvertibility() {
        class TestData {
            MeasureUnitImpl source;
            MeasureUnitImpl target;
            UnitConverter.Convertibility expected;

            TestData(String source, String target, UnitConverter.Convertibility convertibility) {
                this.source = MeasureUnitImpl.UnitsParser.parseForIdentifier(source);
                this.target = MeasureUnitImpl.UnitsParser.parseForIdentifier(target);
                this.expected = convertibility;
            }
        }

        TestData[] tests = {
                new TestData("meter", "foot", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("square-meter-per-square-hour", "hectare-per-square-second", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("hertz", "revolution-per-second", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("millimeter", "meter", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("yard", "meter", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("ounce-troy", "kilogram", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("percent", "portion", UnitConverter.Convertibility.CONVERTIBLE),
                new TestData("ofhg", "kilogram-per-square-meter-square-second", UnitConverter.Convertibility.CONVERTIBLE),

                new TestData("second-per-meter", "meter-per-second", UnitConverter.Convertibility.RECIPROCAL),
        };
        ConversionRates conversionRates = new ConversionRates();

        for (TestData test :
                tests) {
            assertEquals(test.expected, UnitConverter.extractConvertibility(test.source, test.target, conversionRates));
        }
    }

    @Test
    public void testConverterForTemperature() {
        class TestData {
            MeasureUnitImpl source;
            MeasureUnitImpl target;
            BigDecimal input;
            BigDecimal expected;

            TestData(String source, String target, double input, double expected) {
                this.source = MeasureUnitImpl.UnitsParser.parseForIdentifier(source);
                this.target = MeasureUnitImpl.UnitsParser.parseForIdentifier(target);
                this.input = BigDecimal.valueOf(input);
                this.expected = BigDecimal.valueOf(expected);
            }

        }

        TestData[] tests = {
                new TestData("celsius", "fahrenheit", 1000, 1832),
                new TestData("fahrenheit", "fahrenheit", 1000, 1000),
        };

        ConversionRates conversionRates = new ConversionRates();

        for (TestData test :
                tests) {
            UnitConverter converter = new UnitConverter(test.source, test.target, conversionRates);
            assertEquals(test.expected.doubleValue(), converter.convert(test.input).doubleValue(), (0.001));
        }

    }

    @Test
    public void testConverterFromUnitTests() throws IOException {
        class TestCase {
            String category;
            String sourceString;
            String targetString;
            MeasureUnitImpl source;
            MeasureUnitImpl target;
            BigDecimal input;
            BigDecimal expected;

            TestCase(String line) {
                String[] fields = line
                        .replaceAll(" ", "") // Remove all the spaces.
                        .replaceAll(",", "") // Remove all the commas.
                        .replaceAll("\t", "")
                        .split(";");

                this.category = fields[0].replaceAll(" ", "");
                this.sourceString = fields[1];
                this.targetString = fields[2];
                this.source = MeasureUnitImpl.UnitsParser.parseForIdentifier(fields[1]);
                this.target = MeasureUnitImpl.UnitsParser.parseForIdentifier(fields[2]);
                this.input = BigDecimal.valueOf(1000);
                this.expected = new BigDecimal(fields[4]);
            }
        }

        String codePage = "UTF-8";
        BufferedReader f = TestUtil.getDataReader("cldr/units/unitsTest.txt", codePage);
        ArrayList<TestCase> tests = new ArrayList<>();
        while (true) {
            String line = f.readLine();
            if (line == null) break;
            if (line.isEmpty() || line.startsWith("#")) continue;
            tests.add(new TestCase(line));
        }

        ConversionRates conversionRates = new ConversionRates();

        for (TestCase testCase :
                tests) {
            UnitConverter converter = new UnitConverter(testCase.source, testCase.target, conversionRates);
            if (compareTwoBigDecimal(testCase.expected, converter.convert(testCase.input), BigDecimal.valueOf(0.000001))) {
                continue;
            } else {
                fail(new StringBuilder()
                        .append(testCase.category)
                        .append(" ")
                        .append(testCase.sourceString)
                        .append(" ")
                        .append(testCase.targetString)
                        .append(" ")
                        .append(converter.convert(testCase.input).toString())
                        .append(" expected  ")
                        .append(testCase.expected.toString())
                        .toString());
            }
        }
    }

    @Test
    public void testUnitPreferencesFromUnitTests() throws IOException {
        class TestCase {

            final ArrayList<Pair<String, MeasureUnitImpl>> outputUnitInOrder = new ArrayList<>();
            final ArrayList<BigDecimal> expectedInOrder = new ArrayList<>();
            /**
             * Test Case Data
             */
            String category;
            String usage;
            String region;
            Pair<String, MeasureUnitImpl> inputUnit;
            BigDecimal input;

            TestCase(String line) {
                String[] fields = line
                        .replaceAll(" ", "") // Remove all the spaces.
                        .replaceAll(",", "") // Remove all the commas.
                        .replaceAll("\t", "")
                        .split(";");

                String category = fields[0];
                String usage = fields[1];
                String region = fields[2];
                String inputValue = fields[4];
                String inputUnit = fields[5];
                ArrayList<Pair<String, String>> outputs = new ArrayList<>();

                for (int i = 6; i < fields.length - 2; i += 2) {
                    if (i == fields.length - 3) { // last field
                        outputs.add(Pair.of(fields[i + 2], fields[i + 1]));
                    } else {
                        outputs.add(Pair.of(fields[i + 1], fields[i]));
                    }
                }

                this.insertData(category, usage, region, inputUnit, inputValue, outputs);
            }

            private void insertData(String category,
                                    String usage,
                                    String region,
                                    String inputUnitString,
                                    String inputValue,
                                    ArrayList<Pair<String, String>> outputs /* Unit Identifier, expected value */) {
                this.category = category;
                this.usage = usage;
                this.region = region;
                this.inputUnit = Pair.of(inputUnitString, MeasureUnitImpl.UnitsParser.parseForIdentifier(inputUnitString));
                this.input = new BigDecimal(inputValue);
                for (Pair<String, String> output :
                        outputs) {
                    outputUnitInOrder.add(Pair.of(output.first, MeasureUnitImpl.UnitsParser.parseForIdentifier(output.first)));
                    expectedInOrder.add(new BigDecimal(output.second));
                }
            }
        }

        // Read Test data from the unitPreferencesTest
        String codePage = "UTF-8";
        BufferedReader f = TestUtil.getDataReader("cldr/units/unitPreferencesTest.txt", codePage);
        ArrayList<TestCase> tests = new ArrayList<>();
        while (true) {
            String line = f.readLine();
            if (line == null) break;
            if (line.isEmpty() || line.startsWith("#")) continue;
            tests.add(new TestCase(line));
        }

        for (TestCase testCase :
                tests) {
            UnitsRouter router = new UnitsRouter(testCase.inputUnit.second, testCase.region, testCase.usage);
            List<Measure> measures = router.route(testCase.input).measures;

            assertEquals("Measures size must be the same as expected units",
                    measures.size(), testCase.expectedInOrder.size());
            assertEquals("Measures size must be the same as output units",
                    measures.size(), testCase.outputUnitInOrder.size());


            for (int i = 0; i < measures.size(); i++) {
                if (!UnitsTest
                        .compareTwoBigDecimal(testCase.expectedInOrder.get(i),
                                BigDecimal.valueOf(measures.get(i).getNumber().doubleValue()),
                                BigDecimal.valueOf(0.00001))) {
                    fail(testCase.toString() + measures.toString());
                }
            }
        }
    }
}
