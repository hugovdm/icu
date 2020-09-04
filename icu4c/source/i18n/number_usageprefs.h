// © 2020 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING
#ifndef __NUMBER_USAGEPREFS_H__
#define __NUMBER_USAGEPREFS_H__

#include "cmemory.h"
#include "complexunitsconverter.h"
#include "number_types.h"
#include "unicode/listformatter.h"
#include "unicode/localpointer.h"
#include "unicode/locid.h"
#include "unicode/measunit.h"
#include "unicode/stringpiece.h"
#include "unicode/uobject.h"
#include "unitsrouter.h"

U_NAMESPACE_BEGIN

using ::icu::units::ComplexUnitsConverter;
using ::icu::units::UnitsRouter;

namespace number {
namespace impl {

/**
 * A MicroPropsGenerator which uses UnitsRouter to produce output converted to a
 * MeasureUnit appropriate for a particular localized usage: see
 * NumberFormatterSettings::usage().
 */
class U_I18N_API UsagePrefsHandler : public MicroPropsGenerator, public UMemory {
  public:
    UsagePrefsHandler(const Locale &locale, const MeasureUnit &inputUnit, const StringPiece usage,
                      const MicroPropsGenerator *parent, UErrorCode &status);

    /**
     * Obtains the appropriate output value, MeasurementUnit and
     * rounding/precision behaviour from the UnitsRouter.
     *
     * The output unit is passed on to the LongNameHandler via
     * micros.outputUnit.
     */
    void processQuantity(DecimalQuantity &quantity, MicroProps &micros,
                         UErrorCode &status) const U_OVERRIDE;

    /**
     * Returns the list of possible output units, i.e. the full set of
     * preferences, for the localized, usage-specific unit preferences.
     *
     * The returned pointer should be valid for the lifetime of the
     * UsagePrefsHandler instance.
     */
    const MaybeStackVector<MeasureUnit> *getOutputUnits() const {
        return fUnitsRouter.getOutputUnits();
    }

    // Cannot be copied, because UnitsRouter cannot be copied.
    UsagePrefsHandler(const UsagePrefsHandler &) = delete;
    void operator=(const UsagePrefsHandler & /*other*/) = delete;
    // Can be moved though!
    UsagePrefsHandler(UsagePrefsHandler &&other) = default;
    UsagePrefsHandler &operator=(UsagePrefsHandler &&other) = default;

  private:
    UnitsRouter fUnitsRouter;
    const MicroPropsGenerator *fParent;

    static Precision parseSkeletonToPrecision(icu::UnicodeString precisionSkeleton, UErrorCode status);
};

} // namespace impl
} // namespace number

// Export explicit template instantiations of LocalPointerBase and LocalPointer.
// This is required when building DLLs for Windows. (See datefmt.h,
// collationiterator.h, erarules.h and others for similar examples.)
//
// Note: These need to be outside of the number::impl namespace, or Clang will
// generate a compile error.
#if U_PF_WINDOWS <= U_PLATFORM && U_PLATFORM <= U_PF_CYGWIN
#if defined(_MSC_VER)
// Ignore warning 4661 as LocalPointerBase does not use operator== or operator!=
#pragma warning(push)
#pragma warning(disable: 4661)
#endif
template class U_I18N_API LocalPointerBase<ComplexUnitsConverter>;
template class U_I18N_API LocalPointer<ComplexUnitsConverter>;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif

namespace number {
namespace impl {

/**
 * A MicroPropsGenerator which converts a measurement from a simple MeasureUnit
 * to a Mixed MeasureUnit.
 */
class U_I18N_API UnitConversionHandler : public MicroPropsGenerator, public UMemory {
  public:
    /**
     * Constructor.
     *
     * @param unit Specifies both the input and output MeasureUnit: if it is a
     *     MIXED unit, the input MeasureUnit will be just the biggest unit of
     *     the sequence.
     * @param parent The parent MicroPropsGenerator.
     * @param status Receives status.
     */
    UnitConversionHandler(const MeasureUnit &unit, const MicroPropsGenerator *parent,
                          UErrorCode &status);

    /**
     * Obtains the appropraite output values from the Unit Converter.
     */
    void processQuantity(DecimalQuantity &quantity, MicroProps &micros,
                         UErrorCode &status) const U_OVERRIDE;
  private:
    MeasureUnit fOutputUnit;
    LocalPointer<ComplexUnitsConverter> fUnitConverter;
    const MicroPropsGenerator *fParent;
};

} // namespace impl
} // namespace number
U_NAMESPACE_END

#endif // __NUMBER_USAGEPREFS_H__
#endif /* #if !UCONFIG_NO_FORMATTING */
