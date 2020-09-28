// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "cstring.h"
#include "unicode/ures.h"
#include "uresimp.h"
#include "charstr.h"
#include "number_formatimpl.h"
#include "unicode/numfmt.h"
#include "number_patternstring.h"
#include "number_utils.h"
#include "unicode/numberformatter.h"
#include "unicode/dcfmtsym.h"
#include "number_scientific.h"
#include "number_compact.h"
#include "uresimp.h"
#include "ureslocs.h"

using namespace icu;
using namespace icu::number;
using namespace icu::number::impl;


NumberFormatterImpl::NumberFormatterImpl(const MacroProps& macros, UErrorCode& status)
    : NumberFormatterImpl(macros, true, status) {
}

int32_t NumberFormatterImpl::formatStatic(const MacroProps &macros, UFormattedNumberData *results,
                                          UErrorCode &status) {
    DecimalQuantity &inValue = results->quantity;
    FormattedStringBuilder &outString = results->getStringRef();
    NumberFormatterImpl impl(macros, false, status);
    MicroProps& micros = impl.preProcessUnsafe(inValue, status);
    if (U_FAILURE(status)) { return 0; }
    int32_t length = writeNumber(micros, inValue, outString, 0, status);
    length += writeAffixes(micros, outString, 0, length, status);
    results->outputUnit = std::move(micros.outputUnit);
    return length;
}

int32_t NumberFormatterImpl::getPrefixSuffixStatic(const MacroProps& macros, Signum signum,
                                                   StandardPlural::Form plural,
                                                   FormattedStringBuilder& outString, UErrorCode& status) {
    NumberFormatterImpl impl(macros, false, status);
    return impl.getPrefixSuffixUnsafe(signum, plural, outString, status);
}

// NOTE: C++ SPECIFIC DIFFERENCE FROM JAVA:
// The "safe" apply method uses a new MicroProps. In the MicroPropsGenerator, fMicros is copied into the new instance.
// The "unsafe" method simply re-uses fMicros, eliminating the extra copy operation.
// See MicroProps::processQuantity() for details.

int32_t NumberFormatterImpl::format(UFormattedNumberData *results, UErrorCode &status) const {
    DecimalQuantity &inValue = results->quantity;
    FormattedStringBuilder &outString = results->getStringRef();
    MicroProps micros;
    preProcess(inValue, micros, status);
    if (U_FAILURE(status)) { return 0; }
    int32_t length = writeNumber(micros, inValue, outString, 0, status);
    length += writeAffixes(micros, outString, 0, length, status);
    results->outputUnit = std::move(micros.outputUnit);
    return length;
}

void NumberFormatterImpl::preProcess(DecimalQuantity& inValue, MicroProps& microsOut,
                                     UErrorCode& status) const {
    if (U_FAILURE(status)) { return; }
    if (fMicroPropsGenerator == nullptr) {
        status = U_INTERNAL_PROGRAM_ERROR;
        return;
    }
    fMicroPropsGenerator->processQuantity(inValue, microsOut, status);
    microsOut.integerWidth.apply(inValue, status);
}

MicroProps& NumberFormatterImpl::preProcessUnsafe(DecimalQuantity& inValue, UErrorCode& status) {
    if (U_FAILURE(status)) {
        return fMicros; // must always return a value
    }
    if (fMicroPropsGenerator == nullptr) {
        status = U_INTERNAL_PROGRAM_ERROR;
        return fMicros; // must always return a value
    }
    fMicroPropsGenerator->processQuantity(inValue, fMicros, status);
    fMicros.integerWidth.apply(inValue, status);
    return fMicros;
}

int32_t NumberFormatterImpl::getPrefixSuffix(Signum signum, StandardPlural::Form plural,
                                             FormattedStringBuilder& outString, UErrorCode& status) const {
    if (U_FAILURE(status)) { return 0; }
    // #13453: DecimalFormat wants the affixes from the pattern only (modMiddle, aka pattern modifier).
    // Safe path: use fImmutablePatternModifier.
    const Modifier* modifier = fImmutablePatternModifier->getModifier(signum, plural);
    modifier->apply(outString, 0, 0, status);
    if (U_FAILURE(status)) { return 0; }
    return modifier->getPrefixLength();
}

int32_t NumberFormatterImpl::getPrefixSuffixUnsafe(Signum signum, StandardPlural::Form plural,
                                                   FormattedStringBuilder& outString, UErrorCode& status) {
    if (U_FAILURE(status)) { return 0; }
    // #13453: DecimalFormat wants the affixes from the pattern only (modMiddle, aka pattern modifier).
    // Unsafe path: use fPatternModifier.
    fPatternModifier->setNumberProperties(signum, plural);
    fPatternModifier->apply(outString, 0, 0, status);
    if (U_FAILURE(status)) { return 0; }
    return fPatternModifier->getPrefixLength();
}

NumberFormatterImpl::NumberFormatterImpl(const MacroProps& macros, bool safe, UErrorCode& status) {
    fMicroPropsGenerator = macrosToMicroGenerator(macros, safe, status);
}

//////////

const MicroPropsGenerator*
NumberFormatterImpl::macrosToMicroGenerator(const MacroProps& macros, bool safe, UErrorCode& status) {
    if (U_FAILURE(status)) { return nullptr; }
    const MicroPropsGenerator* chain = &fMicros;

    // Check that macros is error-free before continuing.
    if (macros.copyErrorTo(status)) {
        return nullptr;
    }

    // TODO: Accept currency symbols from DecimalFormatSymbols?

    // Pre-compute a few values for efficiency.
    bool isCurrency = utils::unitIsCurrency(macros.unit);
    bool isBaseUnit = utils::unitIsBaseUnit(macros.unit);
    bool isPercent = utils::unitIsPercent(macros.unit);
    bool isPermille = utils::unitIsPermille(macros.unit);
    bool isCompactNotation = macros.notation.fType == Notation::NTN_COMPACT;
    bool isAccounting =
            macros.sign == UNUM_SIGN_ACCOUNTING || macros.sign == UNUM_SIGN_ACCOUNTING_ALWAYS ||
            macros.sign == UNUM_SIGN_ACCOUNTING_EXCEPT_ZERO;
    CurrencyUnit currency(u"", status);
    if (isCurrency) {
        currency = CurrencyUnit(macros.unit, status); // Restore CurrencyUnit from MeasureUnit
    }
    UNumberUnitWidth unitWidth = UNUM_UNIT_WIDTH_SHORT;
    if (macros.unitWidth != UNUM_UNIT_WIDTH_COUNT) {
        unitWidth = macros.unitWidth;
    }
    // Use CLDR unit data for all MeasureUnits (not currency and not
    // no-unit), except use the dedicated percent pattern for percent and
    // permille. However, use the CLDR unit data for percent/permille if a
    // long name was requested OR if compact notation is being used, since
    // compact notation overrides the middle modifier (micros.modMiddle)
    // normally used for the percent pattern.
    bool isCldrUnit = !isCurrency
        && !isBaseUnit
        && (unitWidth == UNUM_UNIT_WIDTH_FULL_NAME
            || !(isPercent || isPermille)
            || isCompactNotation
        );
    bool isMixedUnit = isCldrUnit && (uprv_strcmp(macros.unit.getType(), "") == 0) &&
                       macros.unit.getComplexity(status) == UMEASURE_UNIT_MIXED;

    MeasureUnit unit = macros.unit;
    MeasureUnit perUnit = macros.perUnit;
    if (isCldrUnit && utils::unitIsBaseUnit(perUnit)) {
        // This code triggers a SIGSEGV in code further down, probably in
        // "outputUnit.units[0]->build(status)" (around line 310). FIXME.
        // #0 0x7fe59fc9b275 in icu_68::SingleUnitImpl::getSimpleUnitID() const /home/hugovdm/git/icu/icu4c/source/i18n/measunit_extra.cpp
        // #1 0x7fe59fca57ca in icu_68::(anonymous namespace)::serializeSingle(icu_68::SingleUnitImpl const&, bool, icu_68::CharString&, UErrorCode&) /home/hugovdm/git/icu/icu4c/source/i18n/measunit_extra.cpp:676:30
        // #2 0x7fe59fc9cb79 in icu_68::(anonymous namespace)::serialize(icu_68::MeasureUnitImpl&, UErrorCode&) /home/hugovdm/git/icu/icu4c/source/i18n/measunit_extra.cpp:706:5
        // #3 0x7fe59fc9b1b5 in icu_68::MeasureUnitImpl::build(UErrorCode&) && /home/hugovdm/git/icu/icu4c/source/i18n/measunit_extra.cpp:850:5
        // #4 0x7fe59fc9afd6 in icu_68::SingleUnitImpl::build(UErrorCode&) const /home/hugovdm/git/icu/icu4c/source/i18n/measunit_extra.cpp:784:28
        // #5 0x7fe59fd164ea in icu_68::number::impl::NumberFormatterImpl::macrosToMicroGenerator(icu_68::number::impl::MacroProps const&, bool, UErrorCode&) /home/hugovdm/git/icu/icu4c/source/i18n/number_formatimpl.cpp:301:60

        // // Split up unit when it isn't a built-in, similar to what is done by
        // // blueprint_helpers::parseIdentifierUnitOption(). This gives the
        // // current code a chance to format it as "built-in-unit" "per"
        // // "built-in-unit".
        // MeasureUnitImpl fullUnit = MeasureUnitImpl::forMeasureUnitMaybeCopy(unit, status);
        // unit = MeasureUnit();
        // if (fullUnit.complexity == UMEASURE_UNIT_COMPOUND) {
        //     // TODO(ICU-20941): Clean this up.
        //     for (int32_t i = 0; i < fullUnit.units.length(); i++) {
        //         SingleUnitImpl *subUnit = fullUnit.units[i];
        //         if (subUnit->dimensionality > 0) {
        //             unit = unit.product(subUnit->build(status), status);
        //         } else {
        //             subUnit->dimensionality *= -1;
        //             perUnit = perUnit.product(subUnit->build(status), status);
        //         }
        //     }
        // }
    } else if (isCldrUnit) {
        // Simplify away perUnit when appropriate
        MeasureUnit simplifiedUnit = unit;
        MeasureUnitImpl temp;
        const MeasureUnitImpl &perUnitImpl = MeasureUnitImpl::forMeasureUnit(perUnit, temp, status);
        for (int32_t i = 0; i < perUnitImpl.units.length(); i++) {
            const SingleUnitImpl *subUnit = perUnitImpl.units[i];
            if (subUnit->dimensionality > 0) {
                SingleUnitImpl newSub = *subUnit;
                newSub.dimensionality *= -1;
                simplifiedUnit = simplifiedUnit.product(newSub.build(status), status);
            } else {
                simplifiedUnit = simplifiedUnit.product(subUnit->build(status), status);
            }
        }
        if (unit.getComplexity(status) == UMEASURE_UNIT_COMPOUND ||
            uprv_strcmp(simplifiedUnit.getType(), "") != 0) {
            // * Compound-per-compound, e.g. meter-per-second per second
            // * Built-in units, e.g. pound-force-per-square-inch
            unit = simplifiedUnit;
            perUnit = MeasureUnit();
        }
    }

    // Select the numbering system.
    LocalPointer<const NumberingSystem> nsLocal;
    const NumberingSystem* ns;
    if (macros.symbols.isNumberingSystem()) {
        ns = macros.symbols.getNumberingSystem();
    } else {
        // TODO: Is there a way to avoid creating the NumberingSystem object?
        ns = NumberingSystem::createInstance(macros.locale, status);
        // Give ownership to the function scope.
        nsLocal.adoptInstead(ns);
    }
    const char* nsName = U_SUCCESS(status) ? ns->getName() : "latn";
    uprv_strncpy(fMicros.nsName, nsName, 8);
    fMicros.nsName[8] = 0; // guarantee NUL-terminated

    // Resolve the symbols. Do this here because currency may need to customize them.
    if (macros.symbols.isDecimalFormatSymbols()) {
        fMicros.symbols = macros.symbols.getDecimalFormatSymbols();
    } else {
        LocalPointer<DecimalFormatSymbols> newSymbols(
            new DecimalFormatSymbols(macros.locale, *ns, status), status);
        if (U_FAILURE(status)) {
            return nullptr;
        }
        if (isCurrency) {
            newSymbols->setCurrency(currency.getISOCurrency(), status);
            if (U_FAILURE(status)) {
                return nullptr;
            }
        }
        fMicros.symbols = newSymbols.getAlias();
        fSymbols.adoptInstead(newSymbols.orphan());
    }

    // Load and parse the pattern string. It is used for grouping sizes and affixes only.
    // If we are formatting currency, check for a currency-specific pattern.
    const char16_t* pattern = nullptr;
    if (isCurrency && fMicros.symbols->getCurrencyPattern() != nullptr) {
        pattern = fMicros.symbols->getCurrencyPattern();
    }
    if (pattern == nullptr) {
        CldrPatternStyle patternStyle;
        if (isCldrUnit) {
            patternStyle = CLDR_PATTERN_STYLE_DECIMAL;
        } else if (isPercent || isPermille) {
            patternStyle = CLDR_PATTERN_STYLE_PERCENT;
        } else if (!isCurrency || unitWidth == UNUM_UNIT_WIDTH_FULL_NAME) {
            patternStyle = CLDR_PATTERN_STYLE_DECIMAL;
        } else if (isAccounting) {
            // NOTE: Although ACCOUNTING and ACCOUNTING_ALWAYS are only supported in currencies right now,
            // the API contract allows us to add support to other units in the future.
            patternStyle = CLDR_PATTERN_STYLE_ACCOUNTING;
        } else {
            patternStyle = CLDR_PATTERN_STYLE_CURRENCY;
        }
        pattern = utils::getPatternForStyle(macros.locale, nsName, patternStyle, status);
        if (U_FAILURE(status)) {
            return nullptr;
        }
    }
    auto patternInfo = new ParsedPatternInfo();
    if (patternInfo == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    fPatternInfo.adoptInstead(patternInfo);
    PatternParser::parseToPatternInfo(UnicodeString(pattern), *patternInfo, status);
    if (U_FAILURE(status)) {
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /// START POPULATING THE DEFAULT MICROPROPS AND BUILDING THE MICROPROPS GENERATOR ///
    /////////////////////////////////////////////////////////////////////////////////////

    // Unit Preferences and Conversions as our first step
    if (macros.usage.isSet()) {
        if (!isCldrUnit) {
            // We only support "usage" when the input unit is specified, and is
            // a CLDR Unit.
            status = U_ILLEGAL_ARGUMENT_ERROR;
            return nullptr;
        }
        auto usagePrefsHandler =
            new UsagePrefsHandler(macros.locale, unit, macros.usage.fUsage, chain, status);
        fUsagePrefsHandler.adoptInsteadAndCheckErrorCode(usagePrefsHandler, status);
        chain = fUsagePrefsHandler.getAlias();
    } else if (isMixedUnit) {
        MeasureUnitImpl temp;
        const MeasureUnitImpl &outputUnit = MeasureUnitImpl::forMeasureUnit(unit, temp, status);
        auto unitConversionHandler =
            new UnitConversionHandler(outputUnit.units[0]->build(status), unit, chain, status);
        fUnitConversionHandler.adoptInsteadAndCheckErrorCode(unitConversionHandler, status);
        chain = fUnitConversionHandler.getAlias();
    }

    // Multiplier
    if (macros.scale.isValid()) {
        fMicros.helpers.multiplier.setAndChain(macros.scale, chain);
        chain = &fMicros.helpers.multiplier;
    }

    // Rounding strategy
    Precision precision;
    if (!macros.precision.isBogus()) {
        precision = macros.precision;
    } else if (isCompactNotation) {
        precision = Precision::integer().withMinDigits(2);
    } else if (isCurrency) {
        precision = Precision::currency(UCURR_USAGE_STANDARD);
    } else if (macros.usage.isSet()) {
        // Bogus Precision - it will get set in the UsagePrefsHandler instead
        precision = Precision();
    } else {
        precision = Precision::maxFraction(6);
    }
    UNumberFormatRoundingMode roundingMode;
    roundingMode = macros.roundingMode;
    fMicros.rounder = {precision, roundingMode, currency, status};
    if (U_FAILURE(status)) {
        return nullptr;
    }

    // Grouping strategy
    if (!macros.grouper.isBogus()) {
        fMicros.grouping = macros.grouper;
    } else if (isCompactNotation) {
        // Compact notation uses minGrouping by default since ICU 59
        fMicros.grouping = Grouper::forStrategy(UNUM_GROUPING_MIN2);
    } else {
        fMicros.grouping = Grouper::forStrategy(UNUM_GROUPING_AUTO);
    }
    fMicros.grouping.setLocaleData(*fPatternInfo, macros.locale);

    // Padding strategy
    if (!macros.padder.isBogus()) {
        fMicros.padding = macros.padder;
    } else {
        fMicros.padding = Padder::none();
    }

    // Integer width
    if (!macros.integerWidth.isBogus()) {
        fMicros.integerWidth = macros.integerWidth;
    } else {
        fMicros.integerWidth = IntegerWidth::standard();
    }

    // Sign display
    if (macros.sign != UNUM_SIGN_COUNT) {
        fMicros.sign = macros.sign;
    } else {
        fMicros.sign = UNUM_SIGN_AUTO;
    }

    // Decimal mark display
    if (macros.decimal != UNUM_DECIMAL_SEPARATOR_COUNT) {
        fMicros.decimal = macros.decimal;
    } else {
        fMicros.decimal = UNUM_DECIMAL_SEPARATOR_AUTO;
    }

    // Use monetary separator symbols
    fMicros.useCurrency = isCurrency;

    // Inner modifier (scientific notation)
    if (macros.notation.fType == Notation::NTN_SCIENTIFIC) {
        auto newScientificHandler = new ScientificHandler(&macros.notation, fMicros.symbols, chain);
        if (newScientificHandler == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return nullptr;
        }
        fScientificHandler.adoptInstead(newScientificHandler);
        chain = fScientificHandler.getAlias();
    } else {
        // No inner modifier required
        fMicros.modInner = &fMicros.helpers.emptyStrongModifier;
    }

    // Middle modifier (patterns, positive/negative, currency symbols, percent)
    auto patternModifier = new MutablePatternModifier(false);
    if (patternModifier == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    fPatternModifier.adoptInstead(patternModifier);
    patternModifier->setPatternInfo(
            macros.affixProvider != nullptr ? macros.affixProvider
                                            : static_cast<const AffixPatternProvider*>(fPatternInfo.getAlias()),
            kUndefinedField);
    patternModifier->setPatternAttributes(fMicros.sign, isPermille);
    if (patternModifier->needsPlurals()) {
        patternModifier->setSymbols(
                fMicros.symbols,
                currency,
                unitWidth,
                resolvePluralRules(macros.rules, macros.locale, status),
                status);
    } else {
        patternModifier->setSymbols(fMicros.symbols, currency, unitWidth, nullptr, status);
    }
    if (safe) {
        fImmutablePatternModifier.adoptInsteadAndCheckErrorCode(patternModifier->createImmutable(status),
                                                                status);
    }
    if (U_FAILURE(status)) {
        return nullptr;
    }

    // Outer modifier (CLDR units and currency long names)
    if (isCldrUnit) {
        if (macros.usage.isSet()) {
            fLongNameMultiplexer.adoptInsteadAndCheckErrorCode(
                LongNameMultiplexer::forMeasureUnits(
                    macros.locale, *fUsagePrefsHandler->getOutputUnits(), unitWidth,
                    resolvePluralRules(macros.rules, macros.locale, status), chain, status),
                status);
            chain = fLongNameMultiplexer.getAlias();
        } else if (isMixedUnit) {
            fMixedUnitLongNameHandler.adoptInsteadAndCheckErrorCode(new MixedUnitLongNameHandler(),
                                                                    status);
            MixedUnitLongNameHandler::forMeasureUnit(
                macros.locale, unit, unitWidth, resolvePluralRules(macros.rules, macros.locale, status),
                chain, fMixedUnitLongNameHandler.getAlias(), status);
            chain = fMixedUnitLongNameHandler.getAlias();
        } else {
            fLongNameHandler.adoptInsteadAndCheckErrorCode(new LongNameHandler(), status);
            LongNameHandler::forMeasureUnit(macros.locale, unit, perUnit, unitWidth,
                                            resolvePluralRules(macros.rules, macros.locale, status),
                                            chain, fLongNameHandler.getAlias(), status);
            chain = fLongNameHandler.getAlias();
        }
    } else if (isCurrency && unitWidth == UNUM_UNIT_WIDTH_FULL_NAME) {
        fLongNameHandler.adoptInsteadAndCheckErrorCode(
            LongNameHandler::forCurrencyLongNames(
                macros.locale, currency, resolvePluralRules(macros.rules, macros.locale, status), chain,
                status),
            status);
        chain = fLongNameHandler.getAlias();
    } else {
        // No outer modifier required
        fMicros.modOuter = &fMicros.helpers.emptyWeakModifier;
    }
    if (U_FAILURE(status)) {
        return nullptr;
    }

    // Compact notation
    if (isCompactNotation) {
        CompactType compactType = (isCurrency && unitWidth != UNUM_UNIT_WIDTH_FULL_NAME)
                                  ? CompactType::TYPE_CURRENCY : CompactType::TYPE_DECIMAL;
        auto newCompactHandler = new CompactHandler(
            macros.notation.fUnion.compactStyle,
            macros.locale,
            nsName,
            compactType,
            resolvePluralRules(macros.rules, macros.locale, status),
            patternModifier,
            safe,
            chain,
            status);
        if (U_FAILURE(status)) {
            return nullptr;
        }
        if (newCompactHandler == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return nullptr;
        }
        fCompactHandler.adoptInstead(newCompactHandler);
        chain = fCompactHandler.getAlias();
    }
    if (U_FAILURE(status)) {
        return nullptr;
    }

    // Always add the pattern modifier as the last element of the chain.
    if (safe) {
        fImmutablePatternModifier->addToChain(chain);
        chain = fImmutablePatternModifier.getAlias();
    } else {
        patternModifier->addToChain(chain);
        chain = patternModifier;
    }

    return chain;
}

const PluralRules*
NumberFormatterImpl::resolvePluralRules(const PluralRules* rulesPtr, const Locale& locale,
                                        UErrorCode& status) {
    if (rulesPtr != nullptr) {
        return rulesPtr;
    }
    // Lazily create PluralRules
    if (fRules.isNull()) {
        fRules.adoptInstead(PluralRules::forLocale(locale, status));
    }
    return fRules.getAlias();
}

int32_t NumberFormatterImpl::writeAffixes(const MicroProps& micros, FormattedStringBuilder& string,
                                          int32_t start, int32_t end, UErrorCode& status) {
    U_ASSERT(micros.modOuter != nullptr);
    // Always apply the inner modifier (which is "strong").
    int32_t length = micros.modInner->apply(string, start, end, status);
    if (micros.padding.isValid()) {
        length += micros.padding
                .padAndApply(*micros.modMiddle, *micros.modOuter, string, start, length + end, status);
    } else {
        length += micros.modMiddle->apply(string, start, length + end, status);
        length += micros.modOuter->apply(string, start, length + end, status);
    }
    return length;
}

int32_t NumberFormatterImpl::writeNumber(const MicroProps& micros, DecimalQuantity& quantity,
                                         FormattedStringBuilder& string, int32_t index,
                                         UErrorCode& status) {
    int32_t length = 0;
    if (quantity.isInfinite()) {
        length += string.insert(
                length + index,
                micros.symbols->getSymbol(DecimalFormatSymbols::ENumberFormatSymbol::kInfinitySymbol),
                {UFIELD_CATEGORY_NUMBER, UNUM_INTEGER_FIELD},
                status);

    } else if (quantity.isNaN()) {
        length += string.insert(
                length + index,
                micros.symbols->getSymbol(DecimalFormatSymbols::ENumberFormatSymbol::kNaNSymbol),
                {UFIELD_CATEGORY_NUMBER, UNUM_INTEGER_FIELD},
                status);

    } else {
        // Add the integer digits
        length += writeIntegerDigits(micros, quantity, string, length + index, status);

        // Add the decimal point
        if (quantity.getLowerDisplayMagnitude() < 0 || micros.decimal == UNUM_DECIMAL_SEPARATOR_ALWAYS) {
            length += string.insert(
                    length + index,
                    micros.useCurrency ? micros.symbols->getSymbol(
                            DecimalFormatSymbols::ENumberFormatSymbol::kMonetarySeparatorSymbol) : micros
                            .symbols
                            ->getSymbol(
                                    DecimalFormatSymbols::ENumberFormatSymbol::kDecimalSeparatorSymbol),
                    {UFIELD_CATEGORY_NUMBER, UNUM_DECIMAL_SEPARATOR_FIELD},
                    status);
        }

        // Add the fraction digits
        length += writeFractionDigits(micros, quantity, string, length + index, status);

        if (length == 0) {
            // Force output of the digit for value 0
            length += utils::insertDigitFromSymbols(
                    string,
                    index,
                    0,
                    *micros.symbols,
                    {UFIELD_CATEGORY_NUMBER, UNUM_INTEGER_FIELD},
                    status);
        }
    }

    return length;
}

int32_t NumberFormatterImpl::writeIntegerDigits(const MicroProps& micros, DecimalQuantity& quantity,
                                                FormattedStringBuilder& string, int32_t index,
                                                UErrorCode& status) {
    int length = 0;
    int integerCount = quantity.getUpperDisplayMagnitude() + 1;
    for (int i = 0; i < integerCount; i++) {
        // Add grouping separator
        if (micros.grouping.groupAtPosition(i, quantity)) {
            length += string.insert(
                    index,
                    micros.useCurrency ? micros.symbols->getSymbol(
                            DecimalFormatSymbols::ENumberFormatSymbol::kMonetaryGroupingSeparatorSymbol)
                                       : micros.symbols->getSymbol(
                            DecimalFormatSymbols::ENumberFormatSymbol::kGroupingSeparatorSymbol),
                    {UFIELD_CATEGORY_NUMBER, UNUM_GROUPING_SEPARATOR_FIELD},
                    status);
        }

        // Get and append the next digit value
        int8_t nextDigit = quantity.getDigit(i);
        length += utils::insertDigitFromSymbols(
                string,
                index,
                nextDigit,
                *micros.symbols,
                {UFIELD_CATEGORY_NUMBER,
                UNUM_INTEGER_FIELD},
                status);
    }
    return length;
}

int32_t NumberFormatterImpl::writeFractionDigits(const MicroProps& micros, DecimalQuantity& quantity,
                                                 FormattedStringBuilder& string, int32_t index,
                                                 UErrorCode& status) {
    int length = 0;
    int fractionCount = -quantity.getLowerDisplayMagnitude();
    for (int i = 0; i < fractionCount; i++) {
        // Get and append the next digit value
        int8_t nextDigit = quantity.getDigit(-i - 1);
        length += utils::insertDigitFromSymbols(
                string,
                length + index,
                nextDigit,
                *micros.symbols,
                {UFIELD_CATEGORY_NUMBER, UNUM_FRACTION_FIELD},
                status);
    }
    return length;
}

#endif /* #if !UCONFIG_NO_FORMATTING */
