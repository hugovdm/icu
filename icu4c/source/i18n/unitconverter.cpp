// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "charstr.h"
#include "measunit_impl.h"
#include "unicode/errorcode.h"
#include "unicode/measunit.h"
#include "unicode/stringpiece.h"
#include "unitconverter.h"

U_NAMESPACE_BEGIN

namespace {

const ConversionRateInfo &
extractConversionInfo(StringPiece source,
                      const MaybeStackVector<ConversionRateInfo> &conversionRateInfoList,
                      UErrorCode &status) {
    for (size_t i = 0, n = conversionRateInfoList.length(); i < n; ++i) {
        if (conversionRateInfoList[i]->sourceUnit.toStringPiece() == source)
            return *(conversionRateInfoList[i]);
    }

    status = U_INTERNAL_PROGRAM_ERROR;
    return ConversionRateInfo();
}

MeasureUnit extractBaseUnit(const MeasureUnit &source,
                            const MaybeStackVector<ConversionRateInfo> &conversionRateInfoList,
                            UErrorCode &status) {
    MeasureUnit result;
    int32_t count;
    fprintf(stderr, "initial status: %s, splitting: %s\n", u_errorName(status), source.getIdentifier());
    const auto singleUnits = source.splitToSingleUnits(count, status);
    fprintf(stderr, "singleUnits count: %d\n", count);
    if (U_FAILURE(status)) {
        fprintf(stderr, "FAILED: splitToSingleUnits(<%s>) - %s\n", source.getIdentifier(),
                u_errorName(status));
    }
    if (U_FAILURE(status)) return result;

    for (int i = 0; i < count; ++i) {
        const auto &singleUnit = singleUnits[i];
        fprintf(stderr, "singleUnits[%d]: %s\n", i, singleUnit.getIdentifier());
        // Extract `ConversionRateInfo` using the absolute unit. For example: in case of `square-meter`,
        // we will use `meter`

        if (U_FAILURE(status)) {
            fprintf(stderr, "calling forMeasureUnit %s\n", singleUnit.getIdentifier());
        }
        const auto singleUnitImpl = SingleUnitImpl::forMeasureUnit(singleUnit, status);
        fprintf(stderr, "singleUnitImpl: %s\n", singleUnitImpl.build(status).getIdentifier());
        if (U_FAILURE(status)) {
            fprintf(stderr, "extractConversionInfo failure: %s\n", u_errorName(status));
        }
        const auto &rateInfo =
            extractConversionInfo(singleUnitImpl.identifier, conversionRateInfoList, status);
        fprintf(stderr, "rateInfo.baseUnit: %s\n", rateInfo.baseUnit.data());
        if (U_FAILURE(status)) {
            fprintf(stderr, "extractConversionInfo failure: %s\n", u_errorName(status));
        }
        if (U_FAILURE(status)) return result;

        // Multiply the power of the singleUnit by the power of the baseUnit. For example, square-hectare
        // must be p4-meter. (NOTE: hectare --> square-meter)
        auto baseUnit = MeasureUnit::forIdentifier(rateInfo.baseUnit.toStringPiece(), status);
        auto singleBaseUnit = SingleUnitImpl::forMeasureUnit(baseUnit, status);
        singleBaseUnit.dimensionality *= singleUnit.getDimensionality(status);

        fprintf(stderr, "result: <%s> * <%s> ->", result.getIdentifier(),
                singleBaseUnit.build(status).getIdentifier());
        result = result.product(singleBaseUnit.build(status), status);
        fprintf(stderr, "<%s>\n", result.getIdentifier());
    }

    return result;
}

} // namespace

UnitsMatchingState U_I18N_API
checkUnitsState(const MeasureUnit &source, const MeasureUnit &target,
                const MaybeStackVector<ConversionRateInfo> &conversionRateInfoList, UErrorCode &status) {
    fprintf(stderr, "\nInitial: %s - checkUnitsState(%s)\n", u_errorName(status), source.getIdentifier());
    auto sourceBaseUnit = extractBaseUnit(source, conversionRateInfoList, status);
    fprintf(stderr, "sourceBaseUnit: %s - %s\n", sourceBaseUnit.getIdentifier(), u_errorName(status));
    auto targetBaseUnit = extractBaseUnit(target, conversionRateInfoList, status);
    fprintf(stderr, "targetBaseUnit: %s - %s\n", sourceBaseUnit.getIdentifier(), u_errorName(status));

    if (U_FAILURE(status)) return UNCONVERTIBLE;

    if (sourceBaseUnit == targetBaseUnit) return CONVERTIBLE;
    if (sourceBaseUnit == targetBaseUnit.reciprocal(status)) return RECIPROCAL;

    return UNCONVERTIBLE;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */