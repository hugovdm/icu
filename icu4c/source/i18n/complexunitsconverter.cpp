// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include <math.h>
#include <utility>

#include "cmemory.h"
#include "complexunitsconverter.h"
#include "uassert.h"
#include "unicode/fmtable.h"
#include "unitconverter.h"

U_NAMESPACE_BEGIN

ComplexUnitsConverter::ComplexUnitsConverter(const MeasureUnitImpl &inputUnit,
                                             const MeasureUnitImpl &outputUnits,
                                             const ConversionRates &ratesInfo, UErrorCode &status) {

    if (outputUnits.complexity != UMeasureUnitComplexity::UMEASURE_UNIT_MIXED) {
        unitConverters_.emplaceBackAndCheckErrorCode(status, inputUnit, outputUnits, ratesInfo, status);
        if (U_FAILURE(status)) {
            return;
        }

        units_.emplaceBackAndCheckErrorCode(status, outputUnits, status);
        return;
    }

    // In case the `outputUnits` are `UMEASURE_UNIT_MIXED` such as `foot+inch`. In this case we need more
    // converters to convert from the `inputUnit` to the first unit in the `outputUnits`. Then, a
    // converter from the first unit in the `outputUnits` to the second unit and so on.
    //      For Example:
    //          - inputUnit is `meter`
    //          - outputUnits is `foot+inch`
    //              - Therefore, we need to have two converters:
    //                      1. a converter from `meter` to `foot`
    //                      2. a converter from `foot` to `inch`
    //          - Therefore, if the input is `2 meter`:
    //              1. convert `meter` to `foot` --> 2 meter to 6.56168 feet
    //              2. convert the residual of 6.56168 feet (0.56168) to inches, which will be (6.74016
    //              inches)
    //              3. then, the final result will be (6 feet and 6.74016 inches)
    MaybeStackVector<SingleUnitImpl> singleUnitsInOrder;
    for (int i = 0, n = outputUnits.units.length(); i < n; ++i) {
        /**
         *  TODO(younies): ensure units being in order by the biggest unit at first.
         *
         * HINT:
         *  MaybeStackVector<SingleUnitImpl> singleUnitsInOrder =
         * MeasureUnitImpl::forMeasureUnitMaybeCopy(outputUnits, status).units; uprv_sortArray(
         *      singleUnitsInOrder.getAlias(),
         *      singleUnitsInOrder.length(),
         *      sizeof(singleUnitsInOrder[0]),
         *      compareSingleUnits,
         *      nullptr,
         *      false,
         *      &status);
         */
        singleUnitsInOrder.emplaceBackAndCheckErrorCode(status, *outputUnits.units[i]);
    }

    if (singleUnitsInOrder.length() == 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    for (int i = 0; i < singleUnitsInOrder.length(); i++) {
        MeasureUnitImpl unit;
        unit.append(*singleUnitsInOrder[i], status);
        units_.emplaceBackAndCheckErrorCode(status, unit, status);
    }

    for (int i = 0, n = units_.length(); i < n; i++) {
        if (i == 0) { // first element
            unitConverters_.emplaceBackAndCheckErrorCode(status, inputUnit, *units_[i], ratesInfo,
                                                         status);
        } else {
            unitConverters_.emplaceBackAndCheckErrorCode(status, *units_[i - 1], *units_[i], ratesInfo,
                                                         status);
        }

        if (U_FAILURE(status)) {
            return;
        }
    }
}

UBool ComplexUnitsConverter::greaterThanOrEqual(double quantity, double limit) const {
    U_ASSERT(unitConverters_.length() > 0);

    // First converter converts to the biggest quantity.
    double newQuantity = unitConverters_[0]->convert(quantity);
    return newQuantity >= limit;
}

MaybeStackVector<Measure> ComplexUnitsConverter::convert(double quantity, UErrorCode &status) const {
    MaybeStackVector<Measure> result;

    for (int i = 0, n = unitConverters_.length(); i < n; ++i) {
        quantity = (*unitConverters_[i]).convert(quantity);
        if (i < n - 1) {
            int64_t newQuantity = floor(quantity);
            Formattable formattableNewQuantity(newQuantity);

            // NOTE: Measure would own its MeasureUnit.
            MeasureUnit *type = units_[i]->copy(status).build(status).clone();
            result.emplaceBackAndCheckErrorCode(status, formattableNewQuantity, type, status);

            // Keep the residual of the quantity.
            //   For example: `3.6 feet`, keep only `0.6 feet`
            quantity -= newQuantity;
        } else { // LAST ELEMENT
            Formattable formattableQuantity(quantity);

            // NOTE: Measure would own its MeasureUnit.
            MeasureUnit *type = units_[i]->copy(status).build(status).clone();
            result.emplaceBackAndCheckErrorCode(status, formattableQuantity, type, status);
        }
    }

    return result;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_FORMATTING */
