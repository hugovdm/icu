// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include <cmath>

#include "charstr.h"
#include "double-conversion.h"
#include "getunitsdata.h"
#include "measunit_impl.h"
#include "resource.h"
#include "uassert.h"
#include "unicode/stringpiece.h"
#include "unicode/unistr.h"
#include "unicode/utypes.h"
#include "unitconverter.h"
#include "uresimp.h"

U_NAMESPACE_BEGIN

namespace {

using icu::double_conversion::StringToDoubleConverter;

/* Internal Structure */

enum Constants {
    CONSTANT_FT2M,    // ft2m stands for foot to meter.
    CONSTANT_PI,      // PI
    CONSTANT_GRAVITY, // Gravity
    CONSTANT_G,
    CONSTANT_GAL_IMP2M3, // Gallon imp to m3
    CONSTANT_LB2KG,      // Pound to Kilogram

    // Must be the last element.
    CONSTANTS_COUNT
};

typedef enum SigNum {
    NEGATIVE = -1,
    POSITIVE = 1,
} SigNum;

enum UnitsCase {
    RECIPROCAL,
    CONVERTIBLE,
    UNCONVERTIBLE,
};

/* Helpers */

// Returns `double` from a scientific number(i.e. "1", "2.01" or "3.09E+4")
double strToDouble(StringPiece strNum) {
    // We are processing well-formed input, so we don't need any special options to
    // StringToDoubleConverter.
    StringToDoubleConverter converter(0, 0, 0, "", "");
    int32_t count;
    return converter.StringToDouble(strNum.data(), strNum.length(), &count);
}

// Returns `double` from a scientific number that could has a division sign (i.e. "1", "2.01", "3.09E+4"
// or "2E+2/3")
double strHasDivideSignToDouble(StringPiece strWithDivide) {
    int divisionSignInd = -1;
    for (int i = 0, n = strWithDivide.length(); i < n; ++i) {
        if (strWithDivide.data()[i] == '/') {
            divisionSignInd = i;
            break;
        }
    }

    if (divisionSignInd >= 0) {
        return strToDouble(strWithDivide.substr(0, divisionSignInd)) /
               strToDouble(strWithDivide.substr(divisionSignInd + 1));
    }

    return strToDouble(strWithDivide);
}

/* Represents a conversion factor */
struct Factor {
    double factorNum = 1;
    double factorDen = 1;
    double offset = 0;
    bool reciprocal = false;
    int32_t constants[CONSTANTS_COUNT] = {};

    void multiplyBy(const Factor &rhs) {
        factorNum *= rhs.factorNum;
        factorDen *= rhs.factorDen;
        for (int i = 0; i < CONSTANTS_COUNT; i++)
            constants[i] += rhs.constants[i];

        offset += rhs.offset;
    }

    void divideBy(const Factor &rhs) {
        factorNum *= rhs.factorDen;
        factorDen *= rhs.factorNum;
        for (int i = 0; i < CONSTANTS_COUNT; i++)
            constants[i] -= rhs.constants[i];

        offset += rhs.offset;
    }

    // Apply the power to the factor.
    void power(int32_t power) {
        // multiply all the constant by the power.
        for (int i = 0; i < CONSTANTS_COUNT; i++)
            constants[i] *= power;

        bool shouldFlip = power < 0; // This means that after applying the absolute power, we should flip
                                     // the Numerator and Denomerator.

        factorNum = std::pow(factorNum, std::abs(power));
        factorDen = std::pow(factorDen, std::abs(power));

        if (shouldFlip) {
            // Flip Numerator and Denomirator.
            std::swap(factorNum, factorDen);
        }
    }

    // Flip the `Factor`, for example, factor= 2/3, flippedFactor = 3/2
    void flip() {
        std::swap(factorNum, factorDen);

        for (int i = 0; i < CONSTANTS_COUNT; i++) {
            constants[i] *= -1;
        }
    }

    // Apply SI prefix to the `Factor`
    void applySiPrefix(UMeasureSIPrefix siPrefix) {
        if (siPrefix == UMeasureSIPrefix::UMEASURE_SI_PREFIX_ONE) return; // No need to do anything

        double siApplied = std::pow(10.0, std::abs(siPrefix));

        if (siPrefix < 0) {
            factorDen *= siApplied;
            return;
        }

        factorNum *= siApplied;
    }
};

//////////////////////////
/// BEGIN DATA LOADING ///
//////////////////////////

class UnitConversionRatesSink : public ResourceSink {
  public:
    explicit UnitConversionRatesSink(Factor *conversionFactor) : conversionFactor(conversionFactor) {}

    void put(const char *key, ResourceValue &value, UBool /*noFallback*/,
             UErrorCode &status) U_OVERRIDE {
        ResourceTable conversionRateTable = value.getTable(status);
        if (U_FAILURE(status)) { return; }

        for (int32_t i = 0; conversionRateTable.getKeyAndValue(i, key, value); ++i) {
            StringPiece keySP(key);

            if (keySP == "factor") {
                value.getUnicodeString(status);

            }

            else if (keySP == "offset") {
                value.getUnicodeString(status);
            }

            else if (keySP == "target") {
                // TODO(younies): find a way to convert UnicodeStirng to StringPiece
                // conversionRate->target.set(value.getUnicodeString(status));
            }

            if (U_FAILURE(status)) { return; }
        }
    }

  private:
    Factor *conversionFactor;
};

const ConversionRateInfo& extractConversionRateInfo(StringPiece source,
                                             const MaybeStackVector<ConversionRateInfo> &ratesInfo,
                                             UErrorCode &status) {
    for (int i = 0, n = ratesInfo.length(); i < n; ++i) {
        if (ratesInfo[i]->sourceUnit == source) return *ratesInfo[i];
    }

    status = U_INTERNAL_PROGRAM_ERROR;
    // WIP/TODO(review): cargo-culting or magic-incantation, this fixes the warning:
    // unitconverter.cpp:197:12: warning: returning reference to local temporary object [-Wreturn-stack-address]
    // But I'm not confident in what I'm doing, having only done some casual
    // reading about the possible negative consequencies of returning std::move.
    return std::move(ConversionRateInfo("pound", "kilogram", "0.453592", "0", status));
}

/*/
 * Add single factor element to the `Factor`. e.g "ft3m", "2.333" or "cup2m3". But not "cup2m3^3".
 */
void addSingleFactorConstant(Factor &factor, StringPiece baseStr, int32_t power, SigNum sigNum) {

    if (baseStr == "ft_to_m") {
        factor.constants[CONSTANT_FT2M] += power * sigNum;
    } else if (baseStr == "ft2_to_m2") {
        factor.constants[CONSTANT_FT2M] += 2 * power * sigNum;
    } else if (baseStr == "ft3_to_m3") {
        factor.constants[CONSTANT_FT2M] += 3 * power * sigNum;
    } else if (baseStr == "in3_to_m3") {
        factor.constants[CONSTANT_FT2M] += 3 * power * sigNum;
        factor.factorDen *= 12 * 12 * 12;
    } else if (baseStr == "gal_to_m3") {
        factor.factorNum *= 231;
        factor.constants[CONSTANT_FT2M] += 3 * power * sigNum;
        factor.factorDen *= 12 * 12 * 12;
    } else if (baseStr == "gal_imp_to_m3") {
        factor.constants[CONSTANT_GAL_IMP2M3] += power * sigNum;
    } else if (baseStr == "G") {
        factor.constants[CONSTANT_G] += power * sigNum;
    } else if (baseStr == "gravity") {
        factor.constants[CONSTANT_GRAVITY] += power * sigNum;
    } else if (baseStr == "lb_to_kg") {
        factor.constants[CONSTANT_LB2KG] += power * sigNum;
    } else if (baseStr == "PI") {
        factor.constants[CONSTANT_PI] += power * sigNum;
    } else {
        if (sigNum == SigNum::NEGATIVE) {
            factor.factorDen *= std::pow(strToDouble(baseStr), power);
        } else {
            factor.factorNum *= std::pow(strToDouble(baseStr), power);
        }
    }
}

/*
  Adds single factor for a `Factor` object. Single factor means "23^2", "23.3333", "ft2m^3" ...etc.
  However, complext factor are not included, such as "ft2m^3*200/3"
*/
void addFactorElement(Factor &factor, StringPiece elementStr, SigNum sigNum) {
    StringPiece baseStr;
    StringPiece powerStr;
    int32_t power =
        1; // In case the power is not written, then, the power is equal 1 ==> `ft2m^1` == `ft2m`

    // Search for the power part
    int32_t powerInd = -1;
    for (int32_t i = 0, n = elementStr.length(); i < n; ++i) {
        if (elementStr.data()[i] == '^') {
            powerInd = i;
            break;
        }
    }

    if (powerInd > -1) {
        // There is power
        baseStr = elementStr.substr(0, powerInd);
        powerStr = elementStr.substr(powerInd + 1);

        power = static_cast<int32_t>(strToDouble(powerStr));
    } else {
        baseStr = elementStr;
    }

    addSingleFactorConstant(factor, baseStr, power, sigNum);
}

/*
 * Extracts `Factor` from a complete string factor. e.g. "ft2m^3*1007/cup2m3*3"
 */
void extractFactor(Factor &factor, StringPiece stringFactor, UErrorCode &status) {
    // Set factor to `1`
    factor.factorNum = 1;
    factor.factorDen = 1;

    SigNum sigNum = SigNum::POSITIVE;
    auto factorData = stringFactor.data();
    for (int32_t i = 0, start = 0, n = stringFactor.length(); i < n; i++) {
        if (factorData[i] == '*' || factorData[i] == '/') {
            StringPiece factorElement = stringFactor.substr(start, i - start);
            addFactorElement(factor, factorElement, sigNum);

            start = i + 1; // Set `start` to point to the start of the new element.
        } else if (i == n - 1) {
            // Last element
            addFactorElement(factor, stringFactor.substr(start, i + 1), sigNum);
        }

        if (factorData[i] == '/')
            sigNum = SigNum::NEGATIVE; // Change the sigNum because we reached the Denominator.
    }
}

// Load factor for a single source
void loadSingleFactor(Factor &factor, StringPiece source,
                      const MaybeStackVector<ConversionRateInfo> &ratesInfo, UErrorCode &status) {
    const auto& conversionUnit = extractConversionRateInfo(source, ratesInfo, status);
    if (U_FAILURE(status)) return;

    extractFactor(factor, conversionUnit.factor.toStringPiece(), status);
    factor.offset = strHasDivideSignToDouble(conversionUnit.offset.toStringPiece());
    factor.reciprocal = factor.reciprocal;
}

// Load Factor for compound source
void loadCompoundFactor(Factor &factor, StringPiece source,
                        const MaybeStackVector<ConversionRateInfo> &ratesInfo, UErrorCode &status) {
    auto compoundSourceUnit = MeasureUnitImpl::forIdentifier(source, status);
    for (int32_t i = 0, n = compoundSourceUnit.units.length(); i < n; i++) {
        auto singleUnit = *compoundSourceUnit.units[i]; // a TempSingleUnit

        Factor singleFactor;
        loadSingleFactor(singleFactor, singleUnit.identifier, ratesInfo, status);

        // You must apply SiPrefix before the power, because the power may be will flip the factor.
        singleFactor.applySiPrefix(singleUnit.siPrefix);

        singleFactor.power(singleUnit.dimensionality);

        factor.multiplyBy(singleFactor);
    }
}

void substituteSingleConstant(Factor &factor, int32_t constantPower,
                              double constantValue /* constant actual value, e.g. G= 9.88888 */) {
    constantValue = std::pow(constantValue, std::abs(constantPower));

    if (constantPower < 0) {
        factor.factorDen *= constantValue;
    } else {
        factor.factorNum *= constantValue;
    }
}

void substituteConstants(Factor &factor, UErrorCode &status) {
    double constantsValues[CONSTANTS_COUNT];

    constantsValues[CONSTANT_FT2M] = 0.3048;
    constantsValues[CONSTANT_PI] = 411557987.0 / 131002976.0;
    constantsValues[CONSTANT_GRAVITY] = 9.80665;
    constantsValues[CONSTANT_G] = 6.67408E-11;
    constantsValues[CONSTANT_LB2KG] = 0.45359237;
    constantsValues[CONSTANT_GAL_IMP2M3] = 0.00454609;

    for (int i = 0; i < CONSTANTS_COUNT; i++) {
        if (factor.constants[i] == 0) continue;

        substituteSingleConstant(factor, factor.constants[i], constantsValues[i]);
        factor.constants[i] = 0;
    }
}

/**
 * Checks if the source unit and the target unit are singular. For example celsius or fahrenheit. But not
 * square-celsius or square-fahrenheit.
 */
UBool checkSimpleUnit(StringPiece unitIdentifier, UErrorCode &status) {
    auto compoundSourceUnit = MeasureUnitImpl::forIdentifier(unitIdentifier, status);
    if (compoundSourceUnit.complexity != UMEASURE_UNIT_SINGLE) { return false; }

    U_ASSERT(compoundSourceUnit.units.length() == 1);
    auto singleUnit = *(compoundSourceUnit.units[0]);

    if (singleUnit.dimensionality != 1 || singleUnit.siPrefix != UMEASURE_SI_PREFIX_ONE) {
        return false;
    }
    return true;
}

/**
 *  Extract conversion rate from `source` to `target`
 */
void loadConversionRate(ConversionRate &conversionRate, StringPiece source, StringPiece target,
                        UnitsCase unitsCase, const MaybeStackVector<ConversionRateInfo> &ratesInfo,
                        UErrorCode &status) {
    // Represents the conversion factor from the source to the target.
    Factor finalFactor;

    // Represents the conversion factor from the source to the target that specified in the conversion
    // data which is considered as the root of the source and the target.
    Factor SourceToRoot;
    Factor TargetToRoot;

    /* Load needed factors. */
    // Load the conversion factor from the source to the target in the  which is considered as the Root
    // between
    loadCompoundFactor(SourceToRoot, source, ratesInfo, status);
    loadCompoundFactor(TargetToRoot, target, ratesInfo, status);

    // Merger Factors
    finalFactor.multiplyBy(SourceToRoot);
    if (unitsCase == UnitsCase::CONVERTIBLE) {
        finalFactor.divideBy(TargetToRoot);
    } else if (unitsCase == UnitsCase::RECIPROCAL) {
        finalFactor.multiplyBy(TargetToRoot);
    } else {
        status = UErrorCode::U_ARGUMENT_TYPE_MISMATCH;
        return;
    }

    // Substitute constants
    substituteConstants(finalFactor, status);

    conversionRate.factorNum = finalFactor.factorNum;
    conversionRate.factorDen = finalFactor.factorDen;

    if (checkSimpleUnit(source, status) && checkSimpleUnit(target, status)) {
        conversionRate.sourceOffset =
            SourceToRoot.offset * SourceToRoot.factorDen / SourceToRoot.factorNum;
        conversionRate.targetOffset =
            TargetToRoot.offset * TargetToRoot.factorDen / TargetToRoot.factorNum;
    }

    conversionRate.reciprocal = unitsCase == UnitsCase::RECIPROCAL;
}

StringPiece getTarget(StringPiece source, const MaybeStackVector<ConversionRateInfo> &ratesInfo,
                      UErrorCode &status) {
    const auto& convertUnit = extractConversionRateInfo(source, ratesInfo, status);
    if (U_FAILURE(status)) return StringPiece("");
    return convertUnit.baseUnit.toStringPiece();
}

// TODO(ICU-20568): Add more test coverage for this function.
// Returns the target of a source unit.
MeasureUnit extractTarget(MeasureUnit source, const MaybeStackVector<ConversionRateInfo> &ratesInfo,
                          UErrorCode &status) {
    MeasureUnit result; // Empty unit.
    int32_t singleUnitsLength;
    auto singleUnits = source.splitToSingleUnits(singleUnitsLength, status);

    for (int i = 0; i < singleUnitsLength; i++) {
        const auto &singleUnit = singleUnits[i];
        StringPiece target = getTarget(singleUnit.getIdentifier(), ratesInfo, status);

        if (U_FAILURE(status)) return result;

        MeasureUnit targetUnit = MeasureUnit::forIdentifier(target, status);
        int32_t targetSingleUnitsLength;
        auto targetSingleUnits = targetUnit.splitToSingleUnits(targetSingleUnitsLength, status);
        if (U_FAILURE(status)) return result;

        for (int i = 0; i < targetSingleUnitsLength; ++i) {
            auto targetUnit =
                targetSingleUnits[i].withDimensionality(singleUnit.getDimensionality(status), status);
            if (U_FAILURE(status)) return result;

            result = result.product(targetUnit, status);
            if (U_FAILURE(status)) return result;
        }
    }

    return result;
}

// Checks whether conversion from source to target is possible by checking
// whether their conversion information pivots on the same base unit. If
// UnitsCase::RECIPROCAL is returned, it means one's base unit is the inverse of
// the other's.
UnitsCase checkUnitsCase(const MeasureUnit &source, const MeasureUnit &target,
                         const MaybeStackVector<ConversionRateInfo> &ratesInfo, UErrorCode &status) {
    MeasureUnit sourceTargetUnit = extractTarget(source, ratesInfo, status);
    MeasureUnit targetTargetUnit = extractTarget(target, ratesInfo, status);

    if (sourceTargetUnit == targetTargetUnit) return UnitsCase::CONVERTIBLE;
    if (sourceTargetUnit == targetTargetUnit.reciprocal(status)) return UnitsCase::RECIPROCAL;

    return UnitsCase::UNCONVERTIBLE;
}

} // namespace

UnitConverter::UnitConverter(MeasureUnit source, MeasureUnit target,
                             const MaybeStackVector<ConversionRateInfo> &ratesInfo, UErrorCode &status) {
    UnitsCase unitsCase = checkUnitsCase(source, target, ratesInfo, status);
    if (U_FAILURE(status)) return;

    conversionRate_.source = source;
    conversionRate_.target = target;

    loadConversionRate(conversionRate_, source.getIdentifier(), target.getIdentifier(), unitsCase,
                       ratesInfo, status);
}

double UnitConverter::convert(double inputValue) const {
    double result =
        inputValue + conversionRate_.sourceOffset; // Reset the input to the target zero index.
    // Convert the quantity to from the source scale to the target scale.
    result *= conversionRate_.factorNum / conversionRate_.factorDen;

    result -= conversionRate_.targetOffset; // Set the result to its index.

    if (conversionRate_.reciprocal && result != 0 /* TODO(younies): address zero issue*/)
        result = 1 / result;
    return result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */