// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
package com.ibm.icu.impl.number;

import java.util.EnumMap;
import java.util.Map;
import java.util.MissingResourceException;

import com.ibm.icu.impl.CurrencyData;
import com.ibm.icu.impl.ICUData;
import com.ibm.icu.impl.ICUResourceBundle;
import com.ibm.icu.impl.SimpleFormatterImpl;
import com.ibm.icu.impl.StandardPlural;
import com.ibm.icu.impl.UResource;
import com.ibm.icu.impl.number.Modifier.Signum;
import com.ibm.icu.impl.units.MeasureUnitImpl;
import com.ibm.icu.impl.units.SingleUnitImpl;
import com.ibm.icu.number.NumberFormatter.UnitWidth;
import com.ibm.icu.text.NumberFormat;
import com.ibm.icu.text.PluralRules;
import com.ibm.icu.util.Currency;
import com.ibm.icu.util.ICUException;
import com.ibm.icu.util.MeasureUnit;
import com.ibm.icu.util.MeasureUnit.Complexity;
import com.ibm.icu.util.ULocale;
import com.ibm.icu.util.UResourceBundle;

/**
 * Takes care of formatting currency and measurement unit names, as well as populating the gender of measure units.
 */
public class LongNameHandler
    implements MicroPropsGenerator, ModifierStore, LongNameMultiplexer.ParentlessMicroPropsGenerator {

    private static int i = 0;
    private static final int DNAM_INDEX = StandardPlural.COUNT + i++;
    private static final int PER_INDEX = StandardPlural.COUNT + i++;
    private static final int GENDER_INDEX = StandardPlural.COUNT + i++;
    static final int ARRAY_LENGTH = StandardPlural.COUNT + i++;

    // Returns the array index that corresponds to the given pluralKeyword.
    private static int getIndex(String pluralKeyword) {
        // pluralKeyword can also be "dnam", "per" or "gender"
        if (pluralKeyword.equals("dnam")) {
            return DNAM_INDEX;
        } else if (pluralKeyword.equals("per")) {
            return PER_INDEX;
        } else if (pluralKeyword.equals("gender")) {
            return GENDER_INDEX;
        } else {
            return StandardPlural.fromString(pluralKeyword).ordinal();
        }
    }

    static String getWithPlural(String[] strings, StandardPlural plural) {
        String result = strings[plural.ordinal()];
        if (result == null) {
            result = strings[StandardPlural.OTHER.ordinal()];
        }
        if (result == null) {
            // There should always be data in the "other" plural variant.
            throw new ICUException("Could not find data in 'other' plural variant");
        }
        return result;
    }

    private enum PlaceholderPosition { NONE, BEGINNING, MIDDLE, END }

    private static class ExtractCorePatternResult {
        // extractCorePatternResult(String coreUnit,
        //                          PlaceholderPosition placeholderPosition,
        //                          char joinerChar) {
        //     this.coreUnit = coreUnit;
        //     this.placeholderPosition = placeholderPosition;
        //     this.joinerChar = joinerChar;
        // }
        String coreUnit;
        PlaceholderPosition placeholderPosition;
        char joinerChar;
    }

    /**
     * Returns three outputs extracted from pattern.
     *
     * @param coreUnit is extracted as per Extract(...) in the spec:
     *   https://unicode.org/reports/tr35/tr35-general.html#compound-units
     * @param PlaceholderPosition indicates where in the string the placeholder was
     *   found.
     * @param joinerChar Iff the placeholder was at the beginning or end, joinerChar
     *   contains the space character (if any) that separated the placeholder from
     *   the rest of the pattern. Otherwise, joinerChar is set to NUL.
     */
    private static ExtractCorePatternResult extractCorePattern(String pattern) {
        ExtractCorePatternResult result = new ExtractCorePatternResult();
        result.joinerChar = 0;
        int len = pattern.length();
        if (pattern.startsWith("{0}")) {
            result.placeholderPosition = PlaceholderPosition.BEGINNING;
            if (len > 3 && Character.isSpace(pattern.charAt(3))) {
                result.joinerChar = pattern.charAt(3);
                result.coreUnit = pattern.substring(4);
                // Expecting no double spaces - FIXME(review): assert failure feels bad, even in dev mode
                // assert !Character.isSpaceChar(pattern.charAt(4)) : "Expecting no double spaces";
            } else {
                result.coreUnit = pattern.substring(3);
            }
        } else if (pattern.endsWith("{0}")) {
            result.placeholderPosition = PlaceholderPosition.END;
            if (Character.isSpaceChar(pattern.charAt(len - 4))) {
                result.coreUnit = pattern.substring(0, len - 4);
                result.joinerChar = pattern.charAt(len - 4);
                // Expecting no double spaces - FIXME(review): assert failure feels bad, even in dev mode
                // assert !Character.isSpaceChar(pattern.charAt(len - 5)) : "Expecting no double spaces";)
            } else {
                result.coreUnit = pattern.substring(0, len - 3);
            }
        } else if (pattern.indexOf("{0}", 1) == -1) {
            result.placeholderPosition = PlaceholderPosition.NONE;
            result.coreUnit = pattern;
        } else {
            result.placeholderPosition = PlaceholderPosition.MIDDLE;
            result.coreUnit = pattern;
        }
        return result;
    }

    //////////////////////////
    /// BEGIN DATA LOADING ///
    //////////////////////////

    // Gets the gender of a built-in unit: unit must be a built-in. Returns an empty
    // string both in case of unknown gender and in case of unknown unit.
    private static String getGenderForBuiltin(ULocale locale, MeasureUnit builtinUnit) {
        ICUResourceBundle unitsBundle = (ICUResourceBundle) UResourceBundle.getBundleInstance(ICUData.ICU_UNIT_BASE_NAME, locale);

        StringBuilder key = new StringBuilder();
        key.append("units/");
        key.append(builtinUnit.getType());
        key.append("/");

        // Map duration-year-person, duration-week-person, etc. to duration-year, duration-week, ...
        // TODO(ICU-20400): Get duration-*-person data properly with aliases.
        if (builtinUnit.getSubtype() != null && builtinUnit.getSubtype().endsWith("-person")) {
            key.append(builtinUnit.getSubtype(), 0, builtinUnit.getSubtype().length() - 7);
        } else {
            key.append(builtinUnit.getSubtype());
        }
        key.append("/gender");

        try {
            ICUResourceBundle stackBundle =
                (ICUResourceBundle)unitsBundle.getWithFallback(key.toString());
            return stackBundle.getString();
        } catch (Exception e) {
            // TODO(icu-units#28): "$unitRes/gender" does not exist. Do we want to
            // check whether the parent "$unitRes" exists? Then we could return
            // U_MISSING_RESOURCE_ERROR for incorrect usage (e.g. builtinUnit not
            // being a builtin).
            return "";
        }
    }

    // Loads data from a resource tree with paths matching
    // $key/$pluralForm/$gender/$case, with lateral inheritance for missing cases
    // and genders.
    //
    // An InflectedPluralSink is configured to load data for a specific gender and
    // case. It loads all plural forms, because selection between plural forms is
    // dependent upon the value being formatted.
    //
    // TODO(icu-units#138): Conceptually similar to PluralTableSink, however the
    // tree structures are different. After homogenizing the structures, we may be
    // able to unify the two classes.
    //
    // TODO: Spec violation: expects presence of "count" - does not fallback to an
    // absent "count"! If this fallback were added, getCompoundValue could be
    // superseded?
    private static final class InflectedPluralSink extends UResource.Sink {
        // NOTE: outArray MUST have a length of at least ARRAY_LENGTH. No bounds
        // checking is performed.
        public InflectedPluralSink(String gender, String caseVariant, String[] outArray) {
            this.gender = gender;
            this.caseVariant = caseVariant;
            this.outArray = outArray;
            for (int i = 0; i < ARRAY_LENGTH; i++) {
                outArray[i] = null;
            }
        }

        // See ResourceSink::put().
        @Override
        public void put(UResource.Key key, UResource.Value value, boolean noFallback) {
            UResource.Table pluralsTable = value.getTable();
            for (int i = 0; pluralsTable.getKeyAndValue(i, key, value); ++i) {
                String keyString = key.toString();
                int pluralIndex = getIndex(keyString);
                if (outArray[pluralIndex] != null) {
                    // We already have a pattern
                    continue;
                }
                UResource.Table genderTable = value.getTable();
                if (loadForPluralForm(genderTable, value)) {
                    outArray[pluralIndex] = value.getString();
                }
            }
        }

        // Tries to load data for the configured gender from `genderTable`. The
        // returned data will be for the configured gender if found, falling
        // back to "neuter" and no-gender. If none of those are found, null is
        // returned.
        private boolean loadForPluralForm(UResource.Table genderTable, UResource.Value value) {
            if (gender != null && !gender.isEmpty()) {
                if (loadForGender(genderTable, gender, value)) {
                    return true;
                }
                if (gender != "neuter") {
                    if (loadForGender(genderTable, "neuter", value)) {
                        return true;
                    }
                }
            }
            if (loadForGender(genderTable, "_", value)) {
                return true;
            }
            return false;
        }

        // Tries to load data for the given gender from `genderTable`. Returns true
        // if found, returning the data in `value`. The returned data will be for
        // the configured case if found, falling back to "nominative" and no-case if
        // not.
        private boolean
        loadForGender(UResource.Table genderTable, String genderVal, UResource.Value value) {
            if (!genderTable.findValue(genderVal, value)) {
                return false;
            }
            UResource.Table caseTable = value.getTable();
            if (caseVariant != null && !caseVariant.isEmpty()) {
                if (loadForCase(caseTable, caseVariant, value)) {
                    return true;
                }
                if (caseVariant != "nominative") {
                    if (loadForCase(caseTable, "nominative", value)) {
                        return true;
                    }
                }
            }
            if (loadForCase(caseTable, "_", value)) {
                return true;
            }
            return false;
        }

        // Tries to load data for the given case from `caseTable`. Returns null
        // if not found.
        private boolean loadForCase(UResource.Table caseTable, String caseValue, UResource.Value value) {
            if (!caseTable.findValue(caseValue, value)) {
                return false;
            }
            return true;
        }

        String gender;
        String caseVariant;
        String[] outArray;
    }

    static void getInflectedMeasureData(String subKey,
                                        ULocale locale,
                                        UnitWidth width,
                                        String gender,
                                        String caseVariant,
                                        String[] outArray) {
        InflectedPluralSink sink = new InflectedPluralSink(gender, caseVariant, outArray);
        ICUResourceBundle unitsBundle =
            (ICUResourceBundle)UResourceBundle.getBundleInstance(ICUData.ICU_UNIT_BASE_NAME, locale);

        StringBuilder key = new StringBuilder();
        key.append("units");
        if (width == UnitWidth.NARROW) {
            key.append("Narrow");
        } else if (width == UnitWidth.SHORT) {
            key.append("Short");
        }
        key.append("/");
        key.append(subKey);

        try {
            unitsBundle.getAllItemsWithFallback(key.toString(), sink);
            if (width == UnitWidth.SHORT) {
                return;
            }
        } catch (Exception e) {
            // Continue: fall back to short
        }

        // TODO(ICU-13353): The ICU4J fallback mechanism works differently, is
        // this explicit fallback necessary? FIXME(review)
        key.setLength(0);
        key.append("unitsShort/");
        key.append(subKey);
        unitsBundle.getAllItemsWithFallback(key.toString(), sink);
    }

    private static final class PluralTableSink extends UResource.Sink {

        String[] outArray;

        // NOTE: outArray MUST have at least ARRAY_LENGTH entries. No bounds
        // checking is performed.
        public PluralTableSink(String[] outArray) {
            this.outArray = outArray;
        }

        @Override
        public void put(UResource.Key key, UResource.Value value, boolean noFallback) {
            UResource.Table pluralsTable = value.getTable();
            for (int i = 0; pluralsTable.getKeyAndValue(i, key, value); ++i) {
                String keyString = key.toString();

                if (keyString.equals("case")) {
                    continue;
                }

                int index = getIndex(keyString);
                if (outArray[index] != null) {
                    continue;
                }

                String formatString = value.getString();
                outArray[index] = formatString;
            }
        }
    }

    // NOTE: outArray MUST have at least ARRAY_LENGTH entries. No bounds checking is performed.
    static void getMeasureData(
            ULocale locale,
            MeasureUnit unit,
            UnitWidth width,
            String unitDisplayCase,
            String[] outArray) {
        PluralTableSink sink = new PluralTableSink(outArray);
        ICUResourceBundle resource;
        resource = (ICUResourceBundle) UResourceBundle.getBundleInstance(ICUData.ICU_UNIT_BASE_NAME,
                locale);
        StringBuilder key = new StringBuilder();
        key.append("units");
        // TODO(icu-units#140): support gender for other unit widths.
        if (width == UnitWidth.NARROW) {
            key.append("Narrow");
        } else if (width == UnitWidth.SHORT) {
            key.append("Short");
        }
        key.append("/");
        key.append(unit.getType());
        key.append("/");

        // Map duration-year-person, duration-week-person, etc. to duration-year, duration-week, ...
        // TODO(ICU-20400): Get duration-*-person data properly with aliases.
        if (unit.getSubtype() != null && unit.getSubtype().endsWith("-person")) {
            key.append(unit.getSubtype(), 0, unit.getSubtype().length() - 7);
        } else {
            key.append(unit.getSubtype());
        }

        // Grab desired case first, if available. Then grab nominative case to fill
        // in the gaps.
        //
        // TODO(icu-units#138): check that fallback is spec-compliant
        if (width == UnitWidth.FULL_NAME
                        && unitDisplayCase != null
                        && !unitDisplayCase.isEmpty()) {
            StringBuilder caseKey = new StringBuilder();
            caseKey.append(key);
            caseKey.append("/case/");
            caseKey.append(unitDisplayCase);

            try {
                // TODO(icu-units#138): our fallback logic is not spec-compliant:
                // lateral fallback should happen before locale fallback. Switch to
                // getInflectedMeasureData after homogenizing data format? Find a unit
                // test case that demonstrates the incorrect fallback logic (via
                // regional variant of an inflected language?)
                resource.getAllItemsWithFallback(caseKey.toString(), sink);
                // TODO(icu-units#138): our fallback logic is not spec-compliant: we
                // check the given case, then go straight to the no-case data. The spec
                // states we should first look for case="nominative". As part of #138,
                // either get the spec changed, or add unit tests that warn us if
                // case="nominative" data differs from no-case data?
            } catch (MissingResourceException e) {
                // continue.
            }
        }
        try {
            resource.getAllItemsWithFallback(key.toString(), sink);
        } catch (MissingResourceException e) {
            throw new IllegalArgumentException("No data for unit " + unit + ", width " + width, e);
        }
    }

    private static void getCurrencyLongNameData(ULocale locale, Currency currency, String[] outArray) {
        // In ICU4J, this method gets a CurrencyData from CurrencyData.provider.
        // TODO(ICU4J): Implement this without going through CurrencyData, like in ICU4C?
        Map<String, String> data = CurrencyData.provider.getInstance(locale, true).getUnitPatterns();
        for (Map.Entry<String, String> e : data.entrySet()) {
            String pluralKeyword = e.getKey();
            int index = getIndex(pluralKeyword);
            String longName = currency.getName(locale, Currency.PLURAL_LONG_NAME, pluralKeyword, null);
            String simpleFormat = e.getValue();
            // Example pattern from data: "{0} {1}"
            // Example output after find-and-replace: "{0} US dollars"
            simpleFormat = simpleFormat.replace("{1}", longName);
            // String compiled = SimpleFormatterImpl.compileToStringMinMaxArguments(simpleFormat, sb, 1,
            // 1);
            // SimpleModifier mod = new SimpleModifier(compiled, Field.CURRENCY, false);
            outArray[index] = simpleFormat;
        }
    }

    private static String getCompoundValue(String compoundKey, ULocale locale, UnitWidth width) {
        ICUResourceBundle resource;
        resource = (ICUResourceBundle) UResourceBundle.getBundleInstance(ICUData.ICU_UNIT_BASE_NAME,
                locale);
        StringBuilder key = new StringBuilder();
        key.append("units");
        if (width == UnitWidth.NARROW) {
            key.append("Narrow");
        } else if (width == UnitWidth.SHORT) {
            key.append("Short");
        }
        key.append("/compound/");
        key.append(compoundKey);
        try {
            return resource.getStringWithFallback(key.toString());
        } catch (MissingResourceException e) {
            // FIXME(review): catch all exceptions? Might other issues happen?
            throw new IllegalArgumentException(
                    "Could not find x-per-y format for " + locale + ", width " + width);
            // FIXME(review): how does Java's fallback process differ from
            // ICU4C's? In ICU4C we have code for explicitly falling back to
            // unitsShort.
        }
    }

    /**
     * Loads and applies deriveComponent rules from CLDR's
     * grammaticalFeatures.xml.
     * <p>
     * Consider a deriveComponent rule that looks like this:
     * <pre>
     *   &lt;deriveComponent feature="case" structure="per" value0="compound" value1="nominative"/&gt;
     * </pre>
     * Instantiating an instance as follows:
     * <pre>
     *   DerivedComponents d(loc, "case", "per");
     * </pre>
     * <p>
     * Applying the rule in the XML element above, <code>d.value0("foo")</code>
     * will be "foo", and <code>d.value1("foo")</code> will be "nominative".
     * <p>
     * In case of any kind of failure, value0() and value1() will simply return
     * "".
     */
    private static class DerivedComponents {
        /**
         * Constructor.
         */
        DerivedComponents(ULocale locale, String feature, String structure) {
            try {
                ICUResourceBundle derivationsBundle =
                    (ICUResourceBundle)UResourceBundle.getBundleInstance(ICUData.ICU_BASE_NAME,
                                                                         "grammaticalFeatures");
                derivationsBundle = (ICUResourceBundle)derivationsBundle.get("grammaticalData");
                derivationsBundle = (ICUResourceBundle)derivationsBundle.get("derivations");

                ICUResourceBundle stackBundle;
                try {
                    // TODO: use standard normal locale resolution algorithms rather than just grabbing
                    // language:
                    stackBundle = (ICUResourceBundle)derivationsBundle.get(locale.getLanguage());
                } catch (MissingResourceException e) {
                    stackBundle = (ICUResourceBundle)derivationsBundle.get("root");
                }

                stackBundle = (ICUResourceBundle)stackBundle.get("component");
                stackBundle = (ICUResourceBundle)stackBundle.get(feature);
                stackBundle = (ICUResourceBundle)stackBundle.get(structure);

                String value = stackBundle.getString(0);
                if (value.compareTo("compound") == 0) {
                    this.value0 = null;
                } else {
                    this.value0 = value;
                }

                value = stackBundle.getString(1);
                if (value.compareTo("compound") == 0) {
                    this.value1 = null;
                } else {
                    this.value1 = value;
                }
            } catch (Exception e) {
                // Fall back to uninflected.
            }
        }

        // FIXME(review): is "getValue0" more idiomatic?
        String value0(String compoundValue) {
            return (this.value0 != null) ? this.value0 : compoundValue;
        }

        // FIXME(review): is "getValue1" more idiomatic?
        String value1(String compoundValue) {
            return (this.value1 != null) ? this.value1 : compoundValue;
        }

        private String value0 = "", value1 = "";
    }

    // TODO(icu-units#28): test somehow? Associate with an ICU ticket for adding
    // testsuite support for testing with synthetic data?
    /**
     * Loads and returns the value in rules that look like these:
     *
     * <deriveCompound feature="gender" structure="per" value="0"/>
     * <deriveCompound feature="gender" structure="times" value="1"/>
     *
     * Currently a fake example, but spec compliant:
     * <deriveCompound feature="gender" structure="power" value="feminine"/>
     *
     * NOTE: If U_FAILURE(status), returns an empty string.
     */
    private static String getDeriveCompoundRule(ULocale locale, String feature, String structure) {
        ICUResourceBundle derivationsBundle =
                (ICUResourceBundle) UResourceBundle
                        .getBundleInstance(ICUData.ICU_BASE_NAME, "grammaticalFeatures");

        derivationsBundle = (ICUResourceBundle) derivationsBundle.get("grammaticalData");
        derivationsBundle = (ICUResourceBundle) derivationsBundle.get("derivations");

        ICUResourceBundle stackBundle;
        try {
            // TODO: use standard normal locale resolution algorithms rather than just grabbing language:
            stackBundle = (ICUResourceBundle) derivationsBundle.get(locale.getLanguage());
        } catch (MissingResourceException e) {
            stackBundle = (ICUResourceBundle) derivationsBundle.get("root");
        }

        stackBundle = (ICUResourceBundle) stackBundle.get("compound");
        stackBundle = (ICUResourceBundle) stackBundle.get(feature);

        return stackBundle.getString(structure);
    }

    // Returns the gender string for structures following these rules:
    //
    // <deriveCompound feature="gender" structure="per" value="0"/>
    // <deriveCompound feature="gender" structure="times" value="1"/>
    //
    // Fake example:
    // <deriveCompound feature="gender" structure="power" value="feminine"/>
    //
    // data0 and data1 should be pattern arrays (UnicodeString[ARRAY_SIZE]) that
    // correspond to value="0" and value="1".
    //
    // Pass a null to data1 if the structure has no concept of value="1" (e.g.
    // "prefix" doesn't).
    private static String
    getDerivedGender(ULocale locale, String structure, String[] data0, String[] data1) {
        String val = getDeriveCompoundRule(locale, "gender", structure);
        if (val.length() == 1) {
            switch (val.charAt(0)) {
            case '0':
                return data0[GENDER_INDEX];
            case '1':
                if (data1 == null) {
                    return null; // FIXME(review): null or ""?
                }
                return data1[GENDER_INDEX];
            }
        }
        return val;
    }

    ////////////////////////
    /// END DATA LOADING ///
    ////////////////////////

    private final Map<StandardPlural, SimpleModifier> modifiers;
    private final PluralRules rules;
    private final MicroPropsGenerator parent;
    // Grammatical gender of the formatted result.
    private String gender = "";

    private LongNameHandler(
            Map<StandardPlural, SimpleModifier> modifiers,
            PluralRules rules,
            MicroPropsGenerator parent) {
        this.modifiers = modifiers;
        this.rules = rules;
        this.parent = parent;
    }

    public static String getUnitDisplayName(ULocale locale, MeasureUnit unit, UnitWidth width) {
        String[] measureData = new String[ARRAY_LENGTH];
        getMeasureData(locale, unit, width, "", measureData);
        return measureData[DNAM_INDEX];
    }

    public static LongNameHandler forCurrencyLongNames(
            ULocale locale,
            Currency currency,
            PluralRules rules,
            MicroPropsGenerator parent) {
        String[] simpleFormats = new String[ARRAY_LENGTH];
        getCurrencyLongNameData(locale, currency, simpleFormats);
        // TODO(ICU4J): Reduce the number of object creations here?
        Map<StandardPlural, SimpleModifier> modifiers = new EnumMap<>(
                StandardPlural.class);
        LongNameHandler result = new LongNameHandler(modifiers, rules, parent);
        result.simpleFormatsToModifiers(simpleFormats, NumberFormat.Field.CURRENCY);
        return result;
    }

    /**
     * Construct a localized LongNameHandler for the specified MeasureUnit.
     * <p>
     * Mixed units are not supported, use MixedUnitLongNameHandler.forMeasureUnit.
     *
     * @param locale The desired locale.
     * @param unit The measure unit to construct a LongNameHandler for.
     * @param width Specifies the desired unit rendering.
     * @param unitDisplayCase
     * @param rules Plural rules.
     * @param parent Plural rules.
     */
    public static LongNameHandler forMeasureUnit(
            ULocale locale,
            MeasureUnit unit,
            UnitWidth width,
            String unitDisplayCase,
            PluralRules rules,
            MicroPropsGenerator parent) {
        // From https://unicode.org/reports/tr35/tr35-general.html#compound-units -
        // Points 1 and 2 are mostly handled by MeasureUnit:
        //
        // 1. If the unitId is empty or invalid, fail
        // 2. Put the unitId into normalized order
        if (unit.getType() != null) {
            String[] simpleFormats = new String[ARRAY_LENGTH];
            getMeasureData(locale, unit, width, unitDisplayCase, simpleFormats);
            // TODO(ICU4J): Reduce the number of object creations here?
            Map<StandardPlural, SimpleModifier> modifiers = new EnumMap<>(StandardPlural.class);
            LongNameHandler result = new LongNameHandler(modifiers, rules, parent);
            result.simpleFormatsToModifiers(simpleFormats, NumberFormat.Field.MEASURE_UNIT);
            if (simpleFormats[GENDER_INDEX] != null) {
                result.gender = simpleFormats[GENDER_INDEX];
            }
            return result;
        } else {
            assert unit.getComplexity() != Complexity.MIXED
                : "Mixed units not supported by LongNameHandler: use MixedUnitLongNameHandler";
            return forArbitraryUnit(locale, unit, width, unitDisplayCase, rules, parent);
        }
    }

    private static LongNameHandler forArbitraryUnit(ULocale locale,
                                                    MeasureUnit unit,
                                                    UnitWidth width,
                                                    String unitDisplayCase,
                                                    PluralRules rules,
                                                    MicroPropsGenerator parent) {
        // Not a built-in unit. Split it up, since we can already format
        // "builtin-per-builtin".
        // TODO(ICU-20941): support more generic case than builtin-per-builtin.
        MeasureUnitImpl fullUnit = unit.getCopyOfMeasureUnitImpl();
        unit = null;
        MeasureUnit perUnit = null;
        for (SingleUnitImpl subUnit : fullUnit.getSingleUnits()) {
            if (subUnit.getDimensionality() > 0) {
                if (unit == null) {
                    unit = subUnit.build();
                } else {
                    unit = unit.product(subUnit.build());
                }
            } else {
                // It's okay to mutate fullUnit, we made a temporary copy:
                subUnit.setDimensionality(subUnit.getDimensionality() * -1);
                if (perUnit == null) {
                    perUnit = subUnit.build();
                } else {
                    perUnit = perUnit.product(subUnit.build());
                }
            }
        }

        if (unit.getType() == null || perUnit.getType() == null) {
            // TODO(ICU-20941): Unsanctioned unit. Not yet fully supported. Set an
            // error code.
            throw new UnsupportedOperationException("Unsanctioned units, not yet supported: " + unit +
                                                    "/" + perUnit);
        }

        DerivedComponents derivedPerCases = new DerivedComponents(locale, "case", "per");

        String[] primaryData = new String[ARRAY_LENGTH];
        getMeasureData(locale, unit, width, derivedPerCases.value0(unitDisplayCase), primaryData);
        String[] secondaryData = new String[ARRAY_LENGTH];
        getMeasureData(locale, perUnit, width, derivedPerCases.value1(unitDisplayCase), secondaryData);

        // TODO(icu-units#139): implement these rules:
        //    - <deriveComponent feature="plural" structure="per" ...>
        //    - This has impact on multiSimpleFormatsToModifiers(...) below too.
        //
        // These rules are currently (ICU 69) all the same and hard-coded below.
        String perUnitFormat;
        if (secondaryData[PER_INDEX] != null) {
            perUnitFormat = secondaryData[PER_INDEX];
        } else {
            String rawPerUnitFormat = getCompoundValue("per", locale, width);
            // rawPerUnitFormat is something like "{0}/{1}"; we need to substitute in the secondary unit.
            // TODO: Lots of thrashing. Improve?
            StringBuilder sb = new StringBuilder();
            String compiled = SimpleFormatterImpl
                    .compileToStringMinMaxArguments(rawPerUnitFormat, sb, 2, 2);
            String secondaryFormat = getWithPlural(secondaryData, StandardPlural.ONE);

            // Some "one" pattern may not contain "{0}". For example in "ar" or "ne" locale.
            String secondaryCompiled = SimpleFormatterImpl
                    .compileToStringMinMaxArguments(secondaryFormat, sb, 0, 1);
            String secondaryFormatString = SimpleFormatterImpl.getTextWithNoArguments(secondaryCompiled);

            // TODO(icu-units#28): do not use regular expression
            String secondaryString = secondaryFormatString.replaceAll("(^\\h*)|(\\h*$)",""); // Trim all spaces.

            perUnitFormat = SimpleFormatterImpl.formatCompiledPattern(compiled, "{0}", secondaryString);
        }
        Map<StandardPlural, SimpleModifier> modifiers = new EnumMap<>(
                StandardPlural.class);
        LongNameHandler result = new LongNameHandler(modifiers, rules, parent);
        result.multiSimpleFormatsToModifiers(primaryData, perUnitFormat, NumberFormat.Field.MEASURE_UNIT);

        // Gender
        String val = getDeriveCompoundRule(locale, "gender", "per");

        assert (val != null && val.length() == 1);
        switch (val.charAt(0)) {
        case '0':
            result.gender = primaryData[GENDER_INDEX];
            break;
        case '1':
            result.gender = secondaryData[GENDER_INDEX];
            break;
        default:
            // Data error. Assert-fail in debug mode, else return no gender.
            assert false;
        }

        return result;
    }

    /**
     * Roughly corresponds to patternTimes(...) in the spec:
     * https://unicode.org/reports/tr35/tr35-general.html#compound-units
     */
    private static void processPatternTimes(MeasureUnitImpl productUnit,
                                            ULocale loc,
                                            UnitWidth width,
                                            String caseVariant,
                                            String[] outArray) {
    }

    /** Sets modifiers to use the patterns from simpleFormats. */
    private void simpleFormatsToModifiers(
            String[] simpleFormats,
            NumberFormat.Field field) {
        StringBuilder sb = new StringBuilder();
        for (StandardPlural plural : StandardPlural.VALUES) {
            String simpleFormat = getWithPlural(simpleFormats, plural);
            String compiled = SimpleFormatterImpl.compileToStringMinMaxArguments(simpleFormat, sb, 0, 1);
            Modifier.Parameters parameters = new Modifier.Parameters();
            parameters.obj = this;
            parameters.signum = null;// Signum ignored
            parameters.plural = plural;
            modifiers.put(plural, new SimpleModifier(compiled, field, false, parameters));
        }
    }

    /**
     * Sets modifiers to a combination of `leadFormats` (one per plural form)
     * and `trailFormat` appended to each.
     *
     * With a leadFormat of "{0}m" and a trailFormat of "{0}/s", it produces a
     * pattern of "{0}m/s" by inserting each leadFormat pattern into
     * trailFormat.
     */
    private void multiSimpleFormatsToModifiers(
            String[] leadFormats,
            String trailFormat,
            NumberFormat.Field field) {
        StringBuilder sb = new StringBuilder();
        String trailCompiled = SimpleFormatterImpl.compileToStringMinMaxArguments(trailFormat, sb, 1, 1);
        for (StandardPlural plural : StandardPlural.VALUES) {
            String leadFormat = getWithPlural(leadFormats, plural);
            String compoundFormat = SimpleFormatterImpl.formatCompiledPattern(trailCompiled, leadFormat);
            String compoundCompiled = SimpleFormatterImpl
                    .compileToStringMinMaxArguments(compoundFormat, sb, 0, 1);
            Modifier.Parameters parameters = new Modifier.Parameters();
            parameters.obj = this;
            parameters.signum = null; // Signum ignored
            parameters.plural = plural;
            modifiers.put(plural, new SimpleModifier(compoundCompiled, field, false, parameters));
        }
    }

    @Override
    public MicroProps processQuantity(DecimalQuantity quantity) {
        MicroProps micros = parent.processQuantity(quantity);
        StandardPlural pluralForm = RoundingUtils.getPluralSafe(micros.rounder, rules, quantity);
        micros.modOuter = modifiers.get(pluralForm);
        micros.gender = this.gender;
        return micros;
    }

    /**
     * Produces a plural-appropriate Modifier for a unit: `quantity` is taken as
     * the final smallest unit, while the larger unit values must be provided
     * via `micros.mixedMeasures`.
     *
     * Does not call parent.processQuantity, so cannot get a MicroProps instance
     * that way. Instead, the instance is passed in as a parameter.
     */
    @Override
    public MicroProps processQuantityWithMicros(DecimalQuantity quantity, MicroProps micros) {
        StandardPlural pluralForm = RoundingUtils.getPluralSafe(micros.rounder, rules, quantity);
        micros.modOuter = modifiers.get(pluralForm);
        return micros;
    }

    @Override
    public Modifier getModifier(Signum signum, StandardPlural plural) {
        // Signum ignored
        return modifiers.get(plural);
    }
}
