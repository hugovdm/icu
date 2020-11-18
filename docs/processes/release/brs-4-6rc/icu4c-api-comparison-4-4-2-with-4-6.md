# ICU4C API Comparison: 4.4.2 with 4.6

[TOC]

---

## Removed from 4.4.2

**File API 4.4.2 4.6 comments**dcfmtsym.hvoid
DecimalFormatSymbols::setSymbol(ENumberFormatSymbol, const UnicodeString&)Stable
2.0None
OK, added third parameter with default value (source compatible), see under
"promoted to stable" and "added"format.hUClassID Format::getDynamicClassID()
const =0Stable
2.0None
Ok: Was redundant with pure-virtual declaration in UObject.fpositer.hUClassID
FieldPositionIterator::getDynamicClassID() constDraft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.fpositer.hstatic UClassID
FieldPositionIterator::getStaticClassID()Draft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.LESwaps.hstatic le_uint16 LESwaps::swapWord(const le_uint16&)Stable
2.8None
Ok: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762LESwaps.hstatic le_uint32
LESwaps::swapLong(const le_uint32&)Stable
2.8None
Ok: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762locdspnm.hstatic UClassID
LocaleDisplayNames::getStaticClassID()Internal
4.4 // TODONone
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.normalizer2.hUClassID FilteredNormalizer2::getDynamicClassID()
constDraft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.normalizer2.hUClassID Normalizer2::getDynamicClassID() const =0Draft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.normalizer2.hstatic UClassID
FilteredNormalizer2::getStaticClassID()Draft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.normalizer2.hstatic UClassID Normalizer2::getStaticClassID()Draft
4.4None
Ok: We agreed to not have poor man's RTTI in new class
hierarchies.regex.hRegexMatcher& RegexMatcher::region(int32_t, int32_t,
UErrorCode&)Stable
4.0None
**OK**, int32_t params changed to int64_t, see under "promoted to stable" and
"added"regex.hRegexMatcher& RegexMatcher::reset(int32_t, UErrorCode&)Stable
2.8None
**OK,** int32_t param changed to int64_t, see under "promoted to stable" and
"added"regex.hUBool RegexMatcher::find(int32_t, UErrorCode&)Stable
2.4None
**OK**, int32_t param changes to int64_t, see under "promoted to stable" and
"added"regex.hUBool RegexMatcher::lookingAt(int32_t, UErrorCode&)Stable
2.8None
**OK**, int32_t param changed to int64_t, see under "promoted to stable" and
"added"regex.hUBool RegexMatcher::matches(int32_t, UErrorCode&)Stable
2.8None
**OK**, int32_t param changed to int64_t, see under "promoted to stable" and
"added"regex.hUText\* RegexMatcher::appendTail(UText\*)Internal
4.4 technology previewNone
OK, added error code and updated tag for 4.6, see under "added"regex.hUText\*
RegexMatcher::getInput(UText\*) constInternal
4.4 technology previewNone
OK, added error code and updated tag for 4.6, see under "added"regex.hUText\*
RegexMatcher::group(UText\*, MatcherDestIsUTextFlag, UErrorCode&) constInternal
4.4 technology previewNone
OK, removed - Peterregex.hUText\* RegexPattern::patternText() constInternal
4.4 technology previewNone
OK, added error code and updated tag for 4.6, see under "added"regex.henum
RegexMatcher::MatcherDestIsUTextFlag {}Internal
.None
OK, removed - Peterregex.henum
RegexMatcher::MatcherDestIsUTextFlag::MATCHER_DEST_IS_UTEXTInternal
.None
OK, removed - Peteruidna.h#define UIDNA_ALLOW_UNASSIGNEDStable
2.6None
OK, changed to use enum, see under "promoted to stable" and
"added"uidna.h#define UIDNA_DEFAULTStable
2.6None
OK, changed to use enum, see under "promoted to stable" and
"added"uidna.h#define UIDNA_USE_STD3_RULESStable
2.6None
OK, changed to use enum, see under "promoted to stable" and
"added"unifilt.hUClassID UnicodeFilter::getDynamicClassID() const =0Stable
2.2None
uregex.hUText\* uregex_appendTailUText(URegularExpression\*, UText\*)Internal
4.4 technology previewNone
OK, changed signature and updated tag for 4.6, see under "added"uregex.hUText\*
uregex_groupUText(URegularExpression\*, int32_t, UText\*, UErrorCode\*)Internal
4.4 technology previewNone
OK, removed - Peter

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---

## Deprecated or Obsoleted in 4.6

(none)

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---

## Changed in 4.6

**File API 4.4.2 4.6 comments**LEScripts.henum
ScriptCodes::bamuScriptCodeDraft»Stable
4.4We agreed to have Unicode constants "born stable"regex.hRegexMatcher&
RegexMatcher::appendReplacement(UText\*, UText\*, UErrorCode&)Internal
4.4 technology previewDraft
4.4
regex.hRegexMatcher& RegexMatcher::reset(UText\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hRegexMatcher\*
RegexPattern::matcher(UText\*, PatternIsUTextFlag, UErrorCode&) constInternal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hRegexMatcher::RegexMatcher(UText\*, UText\*,
uint32_t, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hRegexMatcher::RegexMatcher(UText\*,
uint32_t, UErrorCode&)Internal
4.4 technology previewDraft
4.4 fixed to be 4.6 per #8102 regex.hUText\* RegexMatcher::inputText()
constInternal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hUText\* RegexMatcher::replaceAll(UText\*,
UText\*, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hUText\* RegexMatcher::replaceFirst(UText\*,
UText\*, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.henum RegexPattern::PatternIsUTextFlag
{}Internal
.Draft
4.4fixed to be 4.6 per #8102 regex.henum
RegexPattern::PatternIsUTextFlag::PATTERN_IS_UTEXTInternal
.Draft
4.4fixed to be 4.6 per #8102 regex.hint32_t RegexMatcher::split(UText\*,
UText\*dest\[\], int32_t, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hint32_t RegexPattern::split(UText\*,
UText\*dest\[\], int32_t, UErrorCode&) constInternal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hstatic RegexPattern\*
RegexPattern::compile(UText\*, UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hstatic RegexPattern\*
RegexPattern::compile(UText\*, uint32_t, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hstatic RegexPattern\*
RegexPattern::compile(UText\*, uint32_t, UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 regex.hstatic UBool RegexPattern::matches(UText\*,
UText\*, UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uchar.henum UBlockCode::UBLOCK_AVESTANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BAMUMDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_CDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_COMMON_INDIC_NUMBER_FORMSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_DEVANAGARI_EXTENDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_EGYPTIAN_HIEROGLYPHSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENTDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENTDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_HANGUL_JAMO_EXTENDED_ADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_HANGUL_JAMO_EXTENDED_BDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_IMPERIAL_ARAMAICDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_INSCRIPTIONAL_PAHLAVIDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_INSCRIPTIONAL_PARTHIANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_JAVANESEDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_KAITHIDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_LISUDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MEETEI_MAYEKDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MYANMAR_EXTENDED_ADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_OLD_SOUTH_ARABIANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_OLD_TURKICDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_RUMI_NUMERAL_SYMBOLSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_SAMARITANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TAI_THAMDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TAI_VIETDraft»Stable
4.4We agreed to have Unicode constants "born
stable"uchar.henumUBlockCode::UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_VEDIC_EXTENSIONSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UCharNameChoice::U_CHAR_NAME_ALIASDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_FARSI_YEHDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_NYADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CASE_IGNORABLEDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_CASEFOLDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_CASEMAPPEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_LOWERCASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_NFKC_CASEFOLDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_TITLECASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_UPPERCASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uregex.hURegularExpression\*
uregex_openUText(UText\*, uint32_t, UParseError\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hUText\*
uregex_getUText(URegularExpression\*, UText\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hUText\* uregex_patternUText(const
URegularExpression\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hUText\*
uregex_replaceAllUText(URegularExpression\*, UText\*, UText\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hUText\*
uregex_replaceFirstUText(URegularExpression\*, UText\*, UText\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hint32_t
uregex_splitUText(URegularExpression\*, UText\*destFields\[\], int32_t,
UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hvoid
uregex_appendReplacementUText(URegularExpression\*, UText\*, UText\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uregex.hvoid uregex_setUText(URegularExpression\*,
UText\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.4fixed to be 4.6 per #8102 uscript.henum UScriptCode::USCRIPT_MANDAEANStable
2.2Stable
3.6
**(changed)**2.2 was incorrectuscript.henum UScriptCode::USCRIPT_MEROITICStable
2.2Stable
3.6
**(changed)**2.2 was incorrect

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---

## Promoted to stable in 4.6

**File API 4.4.2 4.6 comments**dcfmtsym.hvoid
DecimalFormatSymbols::setSymbol(ENumberFormatSymbol, const UnicodeString&, const
UBool)None
Stable
2.0Was already stable, just added 3rd param with default valueLEScripts.henum
ScriptCodes::bamuScriptCodeDraft»Stable
4.4We agreed to have Unicode constants "born stable"LEScripts.henum
ScriptCodes::bassScriptCodeNone
Stable
4.6We agreed to have Unicode constants "born stable"LEScripts.henum
ScriptCodes::duplScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::elbaScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::granScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::kpelScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::lomaScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::mendScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::mercScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::narbScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::nbatScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::palmScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::sindScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::waraScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LESwaps.hstatic
le_uint16 LESwaps::swapWord(le_uint16)None
Stable
2.8See above: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762LESwaps.hstatic le_uint32
LESwaps::swapLong(le_uint32)None
Stable
2.8See above: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762regex.hRegexMatcher&
RegexMatcher::region(int64_t, int64_t, UErrorCode&)None
Stable
4.0Was already stable, just changed int32_t params to
int64_tregex.hRegexMatcher& RegexMatcher::reset(int64_t, UErrorCode&)None
Stable
2.8Was already stable, just changed int32_t params to int64_tregex.hUBool
RegexMatcher::find(int64_t, UErrorCode&)None
Stable
2.4Was already stable, just changed int32_t params to int64_tregex.hUBool
RegexMatcher::lookingAt(int64_t, UErrorCode&)None
Stable
2.8Was already stable, just changed int32_t params to int64_tregex.hUBool
RegexMatcher::matches(int64_t, UErrorCode&)None
Stable
2.8Was already stable, just changed int32_t params to int64_tuchar.henum
UBlockCode::UBLOCK_ALCHEMICAL_SYMBOLSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_AVESTANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BAMUM_SUPPLEMENTNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BAMUMDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BATAKNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BRAHMINone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_CDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_DNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_COMMON_INDIC_NUMBER_FORMSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_DEVANAGARI_EXTENDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_EGYPTIAN_HIEROGLYPHSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_EMOTICONSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENTDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENTDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ETHIOPIC_EXTENDED_ANone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_HANGUL_JAMO_EXTENDED_ADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_HANGUL_JAMO_EXTENDED_BDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_IMPERIAL_ARAMAICDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_INSCRIPTIONAL_PAHLAVIDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_INSCRIPTIONAL_PARTHIANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_JAVANESEDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_KAITHIDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_KANA_SUPPLEMENTNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_LISUDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MANDAICNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MEETEI_MAYEKDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MYANMAR_EXTENDED_ADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_OLD_SOUTH_ARABIANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_OLD_TURKICDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_PLAYING_CARDSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_RUMI_NUMERAL_SYMBOLSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_SAMARITANDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TAI_THAMDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TAI_VIETDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TRANSPORT_AND_MAP_SYMBOLSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_VEDIC_EXTENSIONSDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UCharNameChoice::U_CHAR_NAME_ALIASDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_FARSI_YEHDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_NYADraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_TEH_MARBUTA_GOALNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CASE_IGNORABLEDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_CASEFOLDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_CASEMAPPEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_LOWERCASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_NFKC_CASEFOLDEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_TITLECASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_CHANGES_WHEN_UPPERCASEDDraft»Stable
4.4We agreed to have Unicode constants "born stable"uidna.henum
(anonymous)::UIDNA_ALLOW_UNASSIGNEDNone
Stable
2.6Was already stable, just changed from #define to enumuidna.henum
(anonymous)::UIDNA_DEFAULTNone
Stable
2.6Was already stable, just changed from #define to enumuidna.henum
(anonymous)::UIDNA_USE_STD3_RULESNone
Stable
2.6Was already stable, just changed from #define to enumuscript.henum
UScriptCode::USCRIPT_BASSA_VAHNone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_DUPLOYAN_SHORTANDNone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_ELBASANNone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_GRANTHANone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_KPELLENone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_LOMANone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_MANDAICNone
Stable
4.6We agreed to have Unicode constants "born stable"uscript.henum
UScriptCode::USCRIPT_MENDENone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_MEROITIC_CURSIVENone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_MEROITIC_HIEROGLYPHSNone
Stable
4.6We agreed to have Unicode constants "born stable"uscript.henum
UScriptCode::USCRIPT_NABATAEANNone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_OLD_NORTH_ARABIANNone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_PALMYRENENone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_SINDHINone
Stable
2.2API status was missing; Unicode constant "born stable"uscript.henum
UScriptCode::USCRIPT_WARANG_CITINone
Stable
2.2API status was missing; Unicode constant "born stable"

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---

## Added in 4.6

**File API 4.4.2 4.6 comments**bytestream.hCheckedArrayByteSink&
CheckedArrayByteSink::Reset()None
Draft
4.6 bytestream.hint32_t CheckedArrayByteSink::NumberOfBytesAppended() constNone
Draft
4.6 coll.hint32_t Collator::getScriptOrder(int32_t\*, const int32_t,
UErrorCode&) constNone
Internal
.Will be renamed, remain internalcoll.hvoid Collator::setScriptOrder(const
int32_t\*, const int32_t, UErrorCode&)None
Internal
.Will be renamed, remain internaldcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kEightDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kFiveDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kFourDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kNineDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kOneDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kSevenDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kSixDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kThreeDigitSymbolNone
Draft
4.6 dcfmtsym.henum
DecimalFormatSymbols::ENumberFormatSymbol::kTwoDigitSymbolNone
Draft
4.6 dcfmtsym.hvoid DecimalFormatSymbols::setSymbol(ENumberFormatSymbol, const
UnicodeString&, const UBool)None
Stable
2.0 Not new, just added 3rd param with default
valueidna.hIDNAInfo::IDNAInfo()None
Draft
4.6 idna.hUBool IDNAInfo::hasErrors() constNone
Draft
4.6 idna.hUBool IDNAInfo::isTransitionalDifferent() constNone
Draft
4.6 idna.hUnicodeString& IDNA::labelToASCII(const UnicodeString&,
UnicodeString&, IDNAInfo&, UErrorCode&) const =0None
Draft
4.6 idna.hUnicodeString& IDNA::labelToUnicode(const UnicodeString&,
UnicodeString&, IDNAInfo&, UErrorCode&) const =0None
Draft
4.6 idna.hUnicodeString& IDNA::nameToASCII(const UnicodeString&, UnicodeString&,
IDNAInfo&, UErrorCode&) const =0None
Draft
4.6 idna.hUnicodeString& IDNA::nameToUnicode(const UnicodeString&,
UnicodeString&, IDNAInfo&, UErrorCode&) const =0None
Draft
4.6 idna.hstatic IDNA\* IDNA::createUTS46Instance(uint32_t, UErrorCode&)None
Draft
4.6 idna.huint32_t IDNAInfo::getErrors() constNone
Draft
4.6 idna.hvoid IDNA::labelToASCII_UTF8(const StringPiece&, ByteSink&, IDNAInfo&,
UErrorCode&) constNone
Draft
4.6 idna.hvoid IDNA::labelToUnicodeUTF8(const StringPiece&, ByteSink&,
IDNAInfo&, UErrorCode&) constNone
Draft
4.6 idna.hvoid IDNA::nameToASCII_UTF8(const StringPiece&, ByteSink&, IDNAInfo&,
UErrorCode&) constNone
Draft
4.6 idna.hvoid IDNA::nameToUnicodeUTF8(const StringPiece&, ByteSink&, IDNAInfo&,
UErrorCode&) constNone
Draft
4.6 LEScripts.henum ScriptCodes::bassScriptCodeNone
Stable
4.6We agreed to have Unicode constants "born stable"LEScripts.henum
ScriptCodes::duplScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::elbaScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::granScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::kpelScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::lomaScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::mendScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::mercScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::narbScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::nbatScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::palmScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::sindScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LEScripts.henum
ScriptCodes::waraScriptCodeNone
Stable
2.2API status was missing; Unicode constant "born stable"LESwaps.hstatic
le_uint16 LESwaps::swapWord(le_uint16)None
Stable
2.8See above: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762LESwaps.hstatic le_uint32
LESwaps::swapLong(le_uint32)None
Stable
2.8See above: Had to fix function signatures to conform to C++ standard. See
http://bugs.icu-project.org/trac/ticket/7762normalizer2.hUBool
FilteredNormalizer2::getDecomposition(UChar32, UnicodeString&) constNone
Draft
4.6 normalizer2.hUBool Normalizer2::getDecomposition(UChar32, UnicodeString&)
const =0None
Draft
4.6 numsys.h#define NUMSYS_NAME_CAPACITYNone
Internal
.Move to internal file?
Can't, maybe a const int. srlnumsys.hconst char\* NumberingSystem::getName()None
Draft
4.6 regex.h#define REGEXFINDPROGRESS_INTERRUPTNone
Draft
4.6Public macro that references a private instance variable, is that OK?
Fixed by Stuart? srlregex.hRegexMatcher& RegexMatcher::region(int64_t, int64_t,
UErrorCode&)None
Stable
4.0Not new, just changed int32_t params to int64_tregex.hRegexMatcher&
RegexMatcher::region(int64_t, int64_t, int64_t, UErrorCode&)None
Draft
4.6
regex.hRegexMatcher& RegexMatcher::reset(int64_t, UErrorCode&)None
Stable
2.8Not new, just changed int32_t param to int64_tregex.hUBool
RegexMatcher::find(int64_t, UErrorCode&)None
Stable
2.4Not new, just changed int32_t param to int64_tregex.hUBool
RegexMatcher::lookingAt(int64_t, UErrorCode&)None
Stable
2.8Not new, just changed int64_t param to int32_tregex.hUBool
RegexMatcher::matches(int64_t, UErrorCode&)None
Stable
2.8Not new, just changed int32_t param to int64_tregex.hUText\*
RegexMatcher::appendTail(UText\*, UErrorCode&)None
Draft
4.6Signature changed from @internal 4.4 tech preview versionregex.hUText\*
RegexMatcher::getInput(UText\*, UErrorCode&) constNone
Draft
4.6Signature changed from @internal 4.4 tech preview versionregex.hUText\*
RegexMatcher::group(UText\*, int64_t&, UErrorCode&) constNone
Draft
4.6 regex.hUText\* RegexMatcher::group(int32_t, UText\*, int64_t&, UErrorCode&)
constNone
Draft
4.6 regex.hUText\* RegexPattern::patternText(UErrorCode&) constNone
Draft
4.6Signature changed from @internal 4.4 tech preview versionregex.hint64_t
RegexMatcher::end64(UErrorCode&) constNone
Draft
4.6 regex.hint64_t RegexMatcher::end64(int32_t, UErrorCode&) constNone
Draft
4.6 regex.hint64_t RegexMatcher::regionEnd64() constNone
Draft
4.6 regex.hint64_t RegexMatcher::regionStart64() constNone
Draft
4.6 regex.hint64_t RegexMatcher::start64(UErrorCode&) constNone
Draft
4.6 regex.hint64_t RegexMatcher::start64(int32_t, UErrorCode&) constNone
Draft
4.6 regex.hvoid
RegexMatcher::getFindProgressCallback(URegexFindProgressCallback\*&, const
void\*&, UErrorCode&)None
Draft
4.6 regex.hvoid
RegexMatcher::setFindProgressCallback(URegexFindProgressCallback\*, const
void\*, UErrorCode&)None
Draft
4.6 tblcoll.hint32_t RuleBasedCollator::getScriptOrder(int32_t\*, const int32_t,
UErrorCode&) constNone
Internal
.Will be renamed, remain internaltblcoll.hvoid
RuleBasedCollator::setScriptOrder(const int32_t\*, const int32_t,
UErrorCode&)None
Internal
.Will be renamed, remain internalubidi.hUBiDiDirection
ubidi_getBaseDirection(const UChar\*, int32_t)None
Draft
4.6 ubidi.henum UBiDiDirection::UBIDI_NEUTRALNone
Draft
4.6 uchar.henum UBlockCode::UBLOCK_ALCHEMICAL_SYMBOLSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BAMUM_SUPPLEMENTNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BATAKNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_BRAHMINone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_DNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_EMOTICONSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_ETHIOPIC_EXTENDED_ANone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_KANA_SUPPLEMENTNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MANDAICNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_PLAYING_CARDSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UBlockCode::UBLOCK_TRANSPORT_AND_MAP_SYMBOLSNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UJoiningGroup::U_JG_TEH_MARBUTA_GOALNone
Stable
4.6We agreed to have Unicode constants "born stable"uchar.henum
UProperty::UCHAR_OTHER_PROPERTY_LIMITNone
Draft
4.6Not "born stable" because Script_Extensions is a provisional Unicode
propertyuchar.henum UProperty::UCHAR_OTHER_PROPERTY_STARTNone
Draft
4.6Not "born stable" because Script_Extensions is a provisional Unicode
propertyuchar.henum UProperty::UCHAR_SCRIPT_EXTENSIONSNone
Draft
4.6Not "born stable" because Script_Extensions is a provisional Unicode
propertyucol.henum UColReorderCode {}None
Internal
**(untagged)**TODO(Stuart): tag **Internal**ucol.henum
UColReorderCode::UCOL_REORDERCODE_CURRENCYNone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_DIGITNone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_FIRSTNone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_IGNORENone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_LIMITNone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_PUNCTUATIONNone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_SPACENone
Internal
. ucol.henum UColReorderCode::UCOL_REORDERCODE_SYMBOLNone
Internal
. ucol.hint32_t ucol_getScriptOrder(const UCollator\*, int32_t\*, const int32_t,
UErrorCode\*)None
Internal
.Will be renamed, remain internalucol.hvoid ucol_setScriptOrder(UCollator\*,
const int32_t\*, const int32_t)None
Internal
.Will be renamed, remain internaluidna.h#define UIDNA_INFO_INITIALIZERNone
Draft
4.6 uidna.hUIDNA\* uidna_openUTS46(uint32_t, UErrorCode\*)None
Draft
4.6 uidna.henum (anonymous)::UIDNA_ALLOW_UNASSIGNEDNone
Stable
2.6Not new, just changed from #define to enumuidna.henum
(anonymous)::UIDNA_CHECK_BIDINone
Draft
4.6 uidna.henum (anonymous)::UIDNA_CHECK_CONTEXTJNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_DEFAULTNone
Stable
2.6Not new, just changed from #define to enumuidna.henum
(anonymous)::UIDNA_ERROR_BIDINone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_CONTEXTJNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_DISALLOWEDNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_DOMAIN_NAME_TOO_LONGNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_EMPTY_LABELNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_HYPHEN_3_4None
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_INVALID_ACE_LABELNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_LABEL_HAS_DOTNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_LABEL_TOO_LONGNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_LEADING_COMBINING_MARKNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_LEADING_HYPHENNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_PUNYCODENone
Draft
4.6 uidna.henum (anonymous)::UIDNA_ERROR_TRAILING_HYPHENNone
Draft
4.6 uidna.henum (anonymous)::UIDNA_NONTRANSITIONAL_TO_ASCIINone
Draft
4.6 uidna.henum (anonymous)::UIDNA_NONTRANSITIONAL_TO_UNICODENone
Draft
4.6 uidna.henum (anonymous)::UIDNA_USE_STD3_RULESNone
Stable
2.6 Not new, just changed from #define to enumuidna.hint32_t
uidna_labelToASCII(const UIDNA\*, const UChar\*, int32_t, UChar\*, int32_t,
UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_labelToASCII_UTF8(const UIDNA\*, const char\*, int32_t,
char\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_labelToUnicode(const UIDNA\*, const UChar\*, int32_t,
UChar\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_labelToUnicodeUTF8(const UIDNA\*, const char\*,
int32_t, char\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_nameToASCII(const UIDNA\*, const UChar\*, int32_t,
UChar\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_nameToASCII_UTF8(const UIDNA\*, const char\*, int32_t,
char\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_nameToUnicode(const UIDNA\*, const UChar\*, int32_t,
UChar\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hint32_t uidna_nameToUnicodeUTF8(const UIDNA\*, const char\*, int32_t,
char\*, int32_t, UIDNAInfo\*, UErrorCode\*)None
Draft
4.6 uidna.hvoid uidna_close(UIDNA\*)None
Draft
4.6 uloc.h#define ULOC_KEYWORD_ASSIGN_UNICODENone
Draft
4.6 uloc.h#define ULOC_KEYWORD_ITEM_SEPARATOR_UNICODENone
Draft
4.6 uloc.h#define ULOC_KEYWORD_SEPARATOR_UNICODENone
Draft
4.6 unorm2.hint32_t unorm2_getDecomposition(const UNormalizer2\*, UChar32,
UChar\*, int32_t, UErrorCode\*)None
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_EIGHT_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_FIVE_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_FOUR_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_NINE_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_ONE_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_SEVEN_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_SIX_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_THREE_DIGIT_SYMBOLNone
Draft
4.6 unum.henum UNumberFormatSymbol::UNUM_TWO_DIGIT_SYMBOLNone
Draft
4.6 uobject.h#define UOBJECT_DEFINE_NO_RTTI_IMPLEMENTATIONNone
Internal
. uregex.hUBool uregex_find64(URegularExpression\*, int64_t, UErrorCode\*)None
Draft
4.6 uregex.hUBool uregex_lookingAt64(URegularExpression\*, int64_t,
UErrorCode\*)None
Draft
4.6 uregex.hUBool uregex_matches64(URegularExpression\*, int64_t,
UErrorCode\*)None
Draft
4.6 uregex.hUText\* uregex_appendTailUText(URegularExpression\*, UText\*,
UErrorCode\*)None
Draft
4.6 uregex.hUText\* uregex_groupUText(URegularExpression\*, int32_t, UText\*,
int64_t\*, UErrorCode\*)None
Draft
4.6 uregex.hUText\* uregex_groupUTextDeep(URegularExpression\*, int32_t,
UText\*, UErrorCode\*)None
Internal
4.4 technology previewDoes this want to be draft 4.6?
Thought this was the controversial one - srluregex.hint64_t
uregex_end64(URegularExpression\*, int32_t, UErrorCode\*)None
Draft
4.6 uregex.hint64_t uregex_regionEnd64(const URegularExpression\*,
UErrorCode\*)None
Draft
4.6 uregex.hint64_t uregex_regionStart64(const URegularExpression\*,
UErrorCode\*)None
Draft
4.6 uregex.hint64_t uregex_start64(URegularExpression\*, int32_t,
UErrorCode\*)None
Draft
4.6 uregex.hvoid uregex_getFindProgressCallback(const URegularExpression\*,
URegexFindProgressCallback\*\*, const void\*\*, UErrorCode\*)None
Draft
4.6 uregex.hvoid uregex_reset64(URegularExpression\*, int64_t, UErrorCode\*)None
Draft
4.6 uregex.hvoid uregex_setFindProgressCallback(URegularExpression\*,
URegexFindProgressCallback\*, const void\*, UErrorCode\*)None
Draft
4.6 uregex.hvoid uregex_setRegion64(URegularExpression\*, int64_t, int64_t,
UErrorCode\*)None
Draft
4.6 uregex.hvoid uregex_setRegionAndStart(URegularExpression\*, int64_t,
int64_t, int64_t, UErrorCode\*)None
Draft
4.6 uscript.hUBool uscript_hasScript(UChar32, UScriptCode)None
Draft
4.6 uscript.henum UScriptCode::USCRIPT_BASSA_VAHNone
Stable
2.2Huh? I see @stable ICU 4.6 for this and ff - Peteruscript.henum
UScriptCode::USCRIPT_DUPLOYAN_SHORTANDNone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_ELBASANNone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_GRANTHANone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_KPELLENone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_LOMANone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_MANDAICNone
Stable
4.6 uscript.henum UScriptCode::USCRIPT_MENDENone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_MEROITIC_CURSIVENone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_MEROITIC_HIEROGLYPHSNone
Stable
4.6 uscript.henum UScriptCode::USCRIPT_NABATAEANNone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_OLD_NORTH_ARABIANNone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_PALMYRENENone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_SINDHINone
Stable
2.2 uscript.henum UScriptCode::USCRIPT_WARANG_CITINone
Stable
2.2 uscript.hint32_t uscript_getScriptExtensions(UChar32, UScriptCode\*,
int32_t, UErrorCode\*)None
Draft
4.6 vtzone.hstatic VTimeZone\* VTimeZone::createVTimeZoneFromBasicTimeZone(const
BasicTimeZone&, UErrorCode&)None
Draft
4.6

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---

## Other existing drafts in 4.6

**File API 4.4.2 4.6 comments**calendar.hUBool Calendar::isWeekend()constDraft
4.4 added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stablecalendar.hUBool Calendar::isWeekend(UDate, UErrorCode&)
constDraft
4.4 added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stablecalendar.hUCalendarWeekdayType
Calendar::getDayOfWeekType(UCalendarDaysOfWeek, UErrorCode&) constDraft
4.4 added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stablecalendar.hint32_t
Calendar::getWeekendTransition(UCalendarDaysOfWeek, UErrorCode&) constDraft
4.4 added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stabledatefmt.hUnicodeString& DateFormat::format(Calendar&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 datefmt.hUnicodeString& DateFormat::format(UDate, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 datefmt.hUnicodeString& DateFormat::format(const Formattable&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 dcfmtsym.hconst UnicodeString&
DecimalFormatSymbols::getPatternForCurrencySpacing(ECurrencySpacing, UBool,
UErrorCode&) constDraft
4.2do not promote to stable, see
http://bugs.icu-project.org/trac/ticket/7385dcfmtsym.henum
DecimalFormatSymbols::ECurrencySpacing {}Draft
4.2ditto
dcfmtsym.henum DecimalFormatSymbols::ECurrencySpacing::kCurrencyMatchDraft
4.2dittodcfmtsym.henum
DecimalFormatSymbols::ECurrencySpacing::kCurrencySpacingCountDraft
4.2dittodcfmtsym.henum DecimalFormatSymbols::ECurrencySpacing::kInsertDraft
4.2dittodcfmtsym.henum
DecimalFormatSymbols::ECurrencySpacing::kSurroundingMatchDraft
4.2dittodcfmtsym.hvoid
DecimalFormatSymbols::setPatternForCurrencySpacing(ECurrencySpacing, UBool,
const UnicodeString&)Draft
4.2dittodecimfmt.hUnicodeString& DecimalFormat::format(const StringPiece&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 decimfmt.hUnicodeString& DecimalFormat::format(double, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 decimfmt.hUnicodeString& DecimalFormat::format(int32_t, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 decimfmt.hUnicodeString& DecimalFormat::format(int64_t, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 dtptngen.hUnicodeString DateTimePatternGenerator::getBestPattern(const
UnicodeString&, UDateTimePatternMatchOptions, UErrorCode&)Draft
4.4 added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stabledtptngen.hUnicodeString
DateTimePatternGenerator::replaceFieldTypes(const UnicodeString&, const
UnicodeString&, UDateTimePatternMatchOptions, UErrorCode&)Draft
4.4 added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableerrorcode.hconst char\* ErrorCode::errorName() constDraft
4.4 errorcode.hvoid ErrorCode::assertSuccess() constDraft
4.4 fmtable.hFormattable::Formattable(const StringPiece&, UErrorCode&)Draft
4.4 fmtable.hStringPiece Formattable::getDecimalNumber(UErrorCode&)Draft
4.4 fmtable.hvoid Formattable::setDecimalNumber(const StringPiece&,
UErrorCode&)Draft
4.4 format.hUnicodeString& Format::format(const Formattable&, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 fpositer.hFieldPositionIterator::FieldPositionIterator()Draft
4.4 fpositer.hFieldPositionIterator::FieldPositionIterator(const
FieldPositionIterator&)Draft
4.4 fpositer.hFieldPositionIterator::~FieldPositionIterator()Draft
4.4 fpositer.hUBool FieldPositionIterator::next(FieldPosition&)Draft
4.4 fpositer.hUBool FieldPositionIterator::operator!=(const
FieldPositionIterator&) constDraft
4.4 fpositer.hUBool FieldPositionIterator::operator==(const
FieldPositionIterator&) constDraft
4.4 locdspnm.hLocaleDisplayNames::~LocaleDisplayNames()Draft
4.4 We agreed to promote to stable when we discussed about the API in J side.
So, it should be same in C - Promote to stable. - Yoshito
locdspnm.hUDialectHandling LocaleDisplayNames::getDialectHandling() const
=0Draft
4.4 Same as above - Yoshito
locdspnm.hUnicodeString& LocaleDisplayNames::keyDisplayName(const char\*,
UnicodeString&) const =0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::keyValueDisplayName(const char\*, const char\*,
UnicodeString&) const =0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::languageDisplayName(const char\*, UnicodeString&) const
=0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::localeDisplayName(const Locale&, UnicodeString&) const
=0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::localeDisplayName(const char\*, UnicodeString&) const
=0Draft
4.4Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::regionDisplayName(const char\*, UnicodeString&) const
=0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::scriptDisplayName(UScriptCode, UnicodeString&) const =0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::scriptDisplayName(const char\*, UnicodeString&) const
=0Draft
4.4 Same as above - Yoshitolocdspnm.hUnicodeString&
LocaleDisplayNames::variantDisplayName(const char\*, UnicodeString&) const
=0Draft
4.4 Same as above - Yoshitolocdspnm.hconst Locale&
LocaleDisplayNames::getLocale() const =0Draft
4.4 Same as above - Yoshitolocdspnm.hstatic LocaleDisplayNames\*
LocaleDisplayNames::createInstance(const Locale&)Draft
4.4 Same as above - Yoshitolocdspnm.hstatic LocaleDisplayNames\*
LocaleDisplayNames::createInstance(const Locale&, UDialectHandling)Draft
4.4 Same as above - Yoshitolocid.hstatic const Locale& Locale::getRoot()Draft
4.4 normalizer2.hFilteredNormalizer2::FilteredNormalizer2(const Normalizer2&,
const UnicodeSet&)Draft
4.4 normalizer2.hUBool FilteredNormalizer2::hasBoundaryAfter(UChar32) constDraft
4.4 normalizer2.hUBool FilteredNormalizer2::hasBoundaryBefore(UChar32)
constDraft
4.4 normalizer2.hUBool FilteredNormalizer2::isInert(UChar32) constDraft
4.4 normalizer2.hUBool FilteredNormalizer2::isNormalized(const UnicodeString&,
UErrorCode&) constDraft
4.4 normalizer2.hUBool Normalizer2::hasBoundaryAfter(UChar32) const =0Draft
4.4 normalizer2.hUBool Normalizer2::hasBoundaryBefore(UChar32) const =0Draft
4.4 normalizer2.hUBool Normalizer2::isInert(UChar32) const =0Draft
4.4 normalizer2.hUBool Normalizer2::isNormalized(const UnicodeString&,
UErrorCode&) const =0Draft
4.4 normalizer2.hUNormalizationCheckResult FilteredNormalizer2::quickCheck(const
UnicodeString&, UErrorCode&) constDraft
4.4 normalizer2.hUNormalizationCheckResult Normalizer2::quickCheck(const
UnicodeString&, UErrorCode&) const =0Draft
4.4 normalizer2.hUnicodeString Normalizer2::normalize(const UnicodeString&,
UErrorCode&) constDraft
4.4 normalizer2.hUnicodeString& FilteredNormalizer2::append(UnicodeString&,
const UnicodeString&, UErrorCode&) constDraft
4.4 normalizer2.hUnicodeString& FilteredNormalizer2::normalize(const
UnicodeString&, UnicodeString&, UErrorCode&) constDraft
4.4 normalizer2.hUnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString&, const
UnicodeString&, UErrorCode&) constDraft
4.4 normalizer2.hUnicodeString& Normalizer2::append(UnicodeString&, const
UnicodeString&, UErrorCode&) const =0Draft
4.4 normalizer2.hUnicodeString& Normalizer2::normalize(const UnicodeString&,
UnicodeString&, UErrorCode&) const =0Draft
4.4 normalizer2.hUnicodeString&
Normalizer2::normalizeSecondAndAppend(UnicodeString&, const UnicodeString&,
UErrorCode&) const =0Draft
4.4 normalizer2.hint32_t FilteredNormalizer2::spanQuickCheckYes(const
UnicodeString&, UErrorCode&) constDraft
4.4 normalizer2.hint32_t Normalizer2::spanQuickCheckYes(const UnicodeString&,
UErrorCode&) const =0Draft
4.4 normalizer2.hstatic const Normalizer2\* Normalizer2::getInstance(const
char\*, const char\*, UNormalization2Mode, UErrorCode&)Draft
4.4 numfmt.hUnicodeString& NumberFormat::format(const Formattable&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 numfmt.hUnicodeString& NumberFormat::format(const StringPiece&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 numfmt.hUnicodeString& NumberFormat::format(double, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 numfmt.hUnicodeString& NumberFormat::format(int32_t, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 numfmt.hUnicodeString& NumberFormat::format(int64_t, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 numfmt.henum NumberFormat::EStyles {}Draft
4.2do not promote to stable, see http://bugs.icu-project.org/trac/ticket/7386
numfmt.henum NumberFormat::EStyles::kCurrencyStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kIsoCurrencyStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kNumberStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kPercentStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kPluralCurrencyStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kScientificStyleDraft
4.2dittonumfmt.henum NumberFormat::EStyles::kStyleCountDraft
4.2dittonumfmt.hstatic NumberFormat\* NumberFormat::createInstance(const
Locale&, EStyles, UErrorCode&)Draft
4.2dittoplatform.h#define UCLN_NO_AUTO_CLEANUPDraft
4.4decided to make @internalplatform.h#define U_CHECK_DYLOADDraft
4.4decided to make @internalplatform.h#define U_ENABLE_DYLOADDraft
4.4decided to make @internalplatform.h (also in std_string.h)#define
U_HAVE_STD_STRINGDraft
4.2decided to make @internalregex.hRegexMatcher&
RegexMatcher::appendReplacement(UText\*, UText\*, UErrorCode&)Internal
4.4 technology previewDraft
4.6Should these be draft 4.6? (yes -- Markus)
Yes. srl
These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.regex.hRegexMatcher& RegexMatcher::reset(UText\*)Internal
4.4 technology previewDraft
4.6 dittoregex.hRegexMatcher\* RegexPattern::matcher(UText\*,
PatternIsUTextFlag, UErrorCode&) constInternal
4.4 technology previewDraft
4.6dittoregex.hRegexMatcher::RegexMatcher(UText\*, UText\*, uint32_t,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hRegexMatcher::RegexMatcher(UText\*, uint32_t,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hUText\* RegexMatcher::inputText() constInternal
4.4 technology previewDraft
4.6dittoregex.hUText\* RegexMatcher::replaceAll(UText\*, UText\*,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hUText\* RegexMatcher::replaceFirst(UText\*, UText\*,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.henum RegexPattern::PatternIsUTextFlag {}Internal
.Draft
4.6dittoregex.henum RegexPattern::PatternIsUTextFlag::PATTERN_IS_UTEXTInternal
.Draft
4.6dittoregex.hint32_t RegexMatcher::split(UText\*, UText\*dest\[\], int32_t,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hint32_t RegexPattern::split(UText\*, UText\*dest\[\], int32_t,
UErrorCode&) constInternal
4.4 technology previewDraft
4.6dittoregex.hstatic RegexPattern\* RegexPattern::compile(UText\*,
UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hstatic RegexPattern\* RegexPattern::compile(UText\*, uint32_t,
UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hstatic RegexPattern\* RegexPattern::compile(UText\*, uint32_t,
UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoregex.hstatic UBool RegexPattern::matches(UText\*, UText\*,
UParseError&, UErrorCode&)Internal
4.4 technology previewDraft
4.6dittoselfmt.hFormat\* SelectFormat::clone()constDraft
4.4 selfmt.hSelectFormat& SelectFormat::operator=(const SelectFormat&)Draft
4.4 selfmt.hSelectFormat::SelectFormat(const SelectFormat&)Draft
4.4 selfmt.hSelectFormat::SelectFormat(const UnicodeString&, UErrorCode&)Draft
4.4 selfmt.hSelectFormat::~SelectFormat()Draft
4.4 selfmt.hUBool SelectFormat::operator!=(const Format&) constDraft
4.4 selfmt.hUBool SelectFormat::operator==(const Format&) constDraft
4.4 selfmt.hUClassID SelectFormat::getDynamicClassID() constDraft
4.4 selfmt.hUnicodeString& SelectFormat::format(const Formattable&,
UnicodeString&, FieldPosition&, UErrorCode&) constDraft
4.4 selfmt.hUnicodeString& SelectFormat::format(const UnicodeString&,
UnicodeString&, FieldPosition&, UErrorCode&) constDraft
4.4 selfmt.hUnicodeString& SelectFormat::toPattern(UnicodeString&)Draft
4.4 selfmt.hstatic UClassID SelectFormat::getStaticClassID()Draft
4.4 selfmt.hvoid SelectFormat::applyPattern(const UnicodeString&,
UErrorCode&)Draft
4.4 selfmt.hvoid SelectFormat::parseObject(const UnicodeString&, Formattable&,
ParsePosition&) constDraft
4.4 smpdtfmt.hUnicodeString& SimpleDateFormat::format(Calendar&, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 smpdtfmt.hUnicodeString& SimpleDateFormat::format(UDate, UnicodeString&,
FieldPositionIterator\*, UErrorCode&) constDraft
4.4 smpdtfmt.hUnicodeString& SimpleDateFormat::format(const Formattable&,
UnicodeString&, FieldPositionIterator\*, UErrorCode&) constDraft
4.4 std_string.h (also in platform.h)#define U_HAVE_STD_STRINGDraft
4.2decided to make @internaltimezone.henum
TimeZone::EDisplayType::GENERIC_LOCATIONDraft
4.4
timezone.henum TimeZone::EDisplayType::LONG_GENERICDraft
4.4
timezone.henum TimeZone::EDisplayType::LONG_GMTDraft
4.4
timezone.henum TimeZone::EDisplayType::SHORT_COMMONLY_USEDDraft
4.4
timezone.henum TimeZone::EDisplayType::SHORT_GENERICDraft
4.4
timezone.henum TimeZone::EDisplayType::SHORT_GMTDraft
4.4
tmutfmt.hTimeUnitFormat::TimeUnitFormat(const Locale&, EStyle, UErrorCode&)Draft
4.2do not promote to stable, see
http://bugs.icu-project.org/trac/ticket/7387tmutfmt.henum TimeUnitFormat::EStyle
{}Draft
4.2ditto
tmutfmt.henum TimeUnitFormat::EStyle::kAbbreviateDraft
4.2ditto
tmutfmt.henum TimeUnitFormat::EStyle::kFullDraft
4.2ditto
tmutfmt.henum TimeUnitFormat::EStyle::kTotalDraft
4.2ditto
ucal.hUBool ucal_isWeekend(const UCalendar\*, UDate, UErrorCode\*)Draft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.hUCalendarWeekdayType ucal_getDayOfWeekType(const
UCalendar\*, UCalendarDaysOfWeek, UErrorCode\*)Draft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.henum UCalendarType::UCAL_DEFAULTDraft
4.2should become @stable -pedbergucal.henum UCalendarWeekdayType {}Draft
needs an ICU version => 4.4
added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg; should
become @stableucal.henum UCalendarWeekdayType::UCAL_WEEKDAYDraft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.henum UCalendarWeekdayType::UCAL_WEEKEND_CEASEDraft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.henum UCalendarWeekdayType::UCAL_WEEKEND_ONSETDraft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.henum UCalendarWeekdayType::UCAL_WEEKENDDraft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableucal.hint32_t ucal_getWeekendTransition(const UCalendar\*,
UCalendarDaysOfWeek, UErrorCode\*)Draft
4.4added per [#2223](http://bugs.icu-project.org/trac/ticket/2223) pedberg;
should become @stableudat.hUCalendarDateFields
udat_toCalendarDateField(UDateFormatField)Draft
4.4 udatpg.henum UDateTimePatternMatchOptions {}Draft
needs an ICU version => 4.4
added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg; should
become @stableudatpg.henum
UDateTimePatternMatchOptions::UDATPG_MATCH_ALL_FIELDS_LENGTHDraft
4.4added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableudatpg.henum
UDateTimePatternMatchOptions::UDATPG_MATCH_HOUR_FIELD_LENGTHDraft
4.4added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableudatpg.henum
UDateTimePatternMatchOptions::UDATPG_MATCH_NO_OPTIONSDraft
4.4added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableudatpg.hint32_t
udatpg_getBestPatternWithOptions(UDateTimePatternGenerator\*, const UChar\*,
int32_t, UDateTimePatternMatchOptions, UChar\*, int32_t, UErrorCode\*)Draft
4.4added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableudatpg.hint32_t
udatpg_replaceFieldTypesWithOptions(UDateTimePatternGenerator\*, const UChar\*,
int32_t, const UChar\*, int32_t, UDateTimePatternMatchOptions, UChar\*, int32_t,
UErrorCode\*)Draft
4.4added per [#7180](http://bugs.icu-project.org/trac/ticket/7180) pedberg;
should become @stableuldnames.hUDialectHandling uldn_getDialectHandling(const
ULocaleDisplayNames\*)Draft
4.4 uldnames.hULocaleDisplayNames\* uldn_open(const char\*, UDialectHandling,
UErrorCode\*)Draft
4.4 uldnames.hconst char\* uldn_getLocale(const ULocaleDisplayNames\*)Draft
4.4 uldnames.henum UDialectHandling {}Draft
needs a version number, 4.4?uldnames.henum
UDialectHandling::ULDN_DIALECT_NAMESDraft
4.4 uldnames.henum UDialectHandling::ULDN_STANDARD_NAMESDraft
4.4 uldnames.hint32_t uldn_keyDisplayName(const ULocaleDisplayNames\*, const
char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_keyValueDisplayName(const ULocaleDisplayNames\*,
const char\*, const char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_languageDisplayName(const ULocaleDisplayNames\*,
const char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_localeDisplayName(const ULocaleDisplayNames\*, const
char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_regionDisplayName(const ULocaleDisplayNames\*, const
char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_scriptCodeDisplayName(const ULocaleDisplayNames\*,
UScriptCode, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_scriptDisplayName(const ULocaleDisplayNames\*, const
char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hint32_t uldn_variantDisplayName(const ULocaleDisplayNames\*, const
char\*, UChar\*, int32_t, UErrorCode\*)Draft
4.4 uldnames.hvoid uldn_close(ULocaleDisplayNames\*)Draft
4.4 uloc.hint32_t uloc_forLanguageTag(const char\*, char\*, int32_t, int32_t\*,
UErrorCode\*)Draft
4.2keep draft until Java 7 adds these??? or do we not care in C?
Already in Java7. Promote. srl
Well... I keep the corresponding J API draft for now. It's in and I do not
expect any changes, but not yet released. If you guys want to make this stable,
I'd make the J one too.
uloc.hint32_t uloc_toLanguageTag(const char\*, char\*, int32_t, UBool,
UErrorCode\*)Draft
4.2ditto
Ditto srl
Same as above.
uniset.hint32_t UnicodeSet::span(const UnicodeString&, int32_t,
USetSpanCondition) constDraft
4.4 uniset.hint32_t UnicodeSet::spanBack(const UnicodeString&, int32_t,
USetSpanCondition) constDraft
4.4 unistr.hUnicodeString UnicodeString::tempSubString(int32_t start=, int32_t
length=) constDraft
4.4 unistr.hUnicodeString UnicodeString::tempSubStringBetween(int32_t, int32_t
limit=) constDraft
4.4 unistr.hUnicodeString& UnicodeString::retainBetween(int32_t, int32_t
limit=)Draft
4.4 unorm2.hUBool unorm2_hasBoundaryAfter(const UNormalizer2\*, UChar32)Draft
4.4 unorm2.hUBool unorm2_hasBoundaryBefore(const UNormalizer2\*, UChar32)Draft
4.4 unorm2.hUBool unorm2_isInert(const UNormalizer2\*, UChar32)Draft
4.4 unorm2.hUBool unorm2_isNormalized(const UNormalizer2\*, const UChar\*,
int32_t, UErrorCode\*)Draft
4.4 unorm2.hUNormalizationCheckResult unorm2_quickCheck(const UNormalizer2\*,
const UChar\*, int32_t, UErrorCode\*)Draft
4.4 unorm2.hUNormalizer2\* unorm2_openFiltered(const UNormalizer2\*, const
USet\*, UErrorCode\*)Draft
4.4 unorm2.hconst UNormalizer2\* unorm2_getInstance(const char\*, const char\*,
UNormalization2Mode, UErrorCode\*)Draft
4.4 unorm2.henum UNormalization2Mode {}Draft
needs a version numberunorm2.henum
UNormalization2Mode::UNORM2_COMPOSE_CONTIGUOUSDraft
4.4 unorm2.henum UNormalization2Mode::UNORM2_COMPOSEDraft
4.4 unorm2.henum UNormalization2Mode::UNORM2_DECOMPOSEDraft
4.4 unorm2.henum UNormalization2Mode::UNORM2_FCDDraft
4.4 unorm2.hint32_t unorm2_append(const UNormalizer2\*, UChar\*, int32_t,
int32_t, const UChar\*, int32_t, UErrorCode\*)Draft
4.4 unorm2.hint32_t unorm2_normalize(const UNormalizer2\*, const UChar\*,
int32_t, UChar\*, int32_t, UErrorCode\*)Draft
4.4 unorm2.hint32_t unorm2_normalizeSecondAndAppend(const UNormalizer2\*,
UChar\*, int32_t, int32_t, const UChar\*, int32_t, UErrorCode\*)Draft
4.4 unorm2.hint32_t unorm2_spanQuickCheckYes(const UNormalizer2\*, const
UChar\*, int32_t, UErrorCode\*)Draft
4.4 unorm2.hvoid unorm2_close(UNormalizer2\*)Draft
4.4 unum.henum UNumberFormatStyle::UNUM_NUMBERING_SYSTEMDraft
4.2 unum.hint32_t unum_formatDecimal(const UNumberFormat\*, const char\*,
int32_t, UChar\*, int32_t, UFieldPosition\*, UErrorCode\*)Draft
4.4 unum.hint32_t unum_parseDecimal(const UNumberFormat\*, const UChar\*,
int32_t, int32_t\*, char\*, int32_t, UErrorCode\*)Draft
4.4 uregex.hURegularExpression\* uregex_openUText(UText\*, uint32_t,
UParseError\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.6@draft ICU 4.6
These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hUText\* uregex_getUText(URegularExpression\*, UText\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.6These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hUText\* uregex_patternUText(const URegularExpression\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.6These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hUText\* uregex_replaceAllUText(URegularExpression\*,
UText\*, UText\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.6These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hUText\* uregex_replaceFirstUText(URegularExpression\*,
UText\*, UText\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.6These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.henum URegexpFlag::UREGEX_CANON_EQDraft
2.4Should be 4.2?? srl
No, this is indeed old. It says "Not implemented yet, just a placeholder, hence
draft." See
<http://bugs.icu-project.org/trac/browser/icu/trunk/source/i18n/unicode/uregex.h?annotate=blame>uregex.hint32_t
uregex_splitUText(URegularExpression\*, UText\*destFields\[\], int32_t,
UErrorCode\*)Internal
4.4 technology previewDraft
4.4These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hvoid uregex_appendReplacementUText(URegularExpression\*,
UText\*, UText\*, UErrorCode\*)Internal
4.4 technology previewDraft
4.4These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.uregex.hvoid uregex_setUText(URegularExpression\*, UText\*,
UErrorCode\*)Internal
4.4 technology previewDraft
4.6These were \*already\* made draft 4.6 per
[#8102](http://bugs.icu-project.org/trac/ticket/8102) - pedberg. Fixing this doc
to so indicate.usearch.henum USearchAttribute::USEARCH_ELEMENT_COMPARISONDraft
4.4added per [#7093](http://bugs.icu-project.org/trac/ticket/7093) pedberg -
should become @stableusearch.henum
USearchAttributeValue::USEARCH_ANY_BASE_WEIGHT_IS_WILDCARDDraft
4.4added per [#7093](http://bugs.icu-project.org/trac/ticket/7093) pedberg -
should become @stableusearch.henum
USearchAttributeValue::USEARCH_PATTERN_BASE_WEIGHT_IS_WILDCARDDraft
4.4added per [#7093](http://bugs.icu-project.org/trac/ticket/7093) pedberg -
should become @stableusearch.henum
USearchAttributeValue::USEARCH_STANDARD_ELEMENT_COMPARISONDraft
4.4added per [#7093](http://bugs.icu-project.org/trac/ticket/7093) pedberg -
should become @stableushape.h#define SHAPE_TAIL_NEW_UNICODEDraft
4.2 Do not promote- should be U_ - srlushape.h#define SHAPE_TAIL_TYPE_MASKDraft
4.2 Do not promote- should be U_ - srlushape.h#define U_SHAPE_LAMALEF_AUTODraft
4.2 ushape.h#define U_SHAPE_LAMALEF_BEGINDraft
4.2 ushape.h#define U_SHAPE_LAMALEF_ENDDraft
4.2 ushape.h#define U_SHAPE_LAMALEF_MASKDraft
4.2 ushape.h#define U_SHAPE_LAMALEF_NEARDraft
4.2 ushape.h#define U_SHAPE_LAMALEF_RESIZEDraft
4.2 ushape.h#define U_SHAPE_SEEN_MASKDraft
4.2 ushape.h#define U_SHAPE_SEEN_TWOCELL_NEARDraft
4.2 ushape.h#define U_SHAPE_SPACES_RELATIVE_TO_TEXT_BEGIN_ENDDraft
4.2 ushape.h#define U_SHAPE_SPACES_RELATIVE_TO_TEXT_MASKDraft
4.2 ushape.h#define U_SHAPE_TASHKEEL_BEGINDraft
4.2 ushape.h#define U_SHAPE_TASHKEEL_ENDDraft
4.2 ushape.h#define U_SHAPE_TASHKEEL_MASKDraft
4.2 ushape.h#define U_SHAPE_TASHKEEL_REPLACE_BY_TATWEELDraft
4.2 ushape.h#define U_SHAPE_TASHKEEL_RESIZEDraft
4.2 ushape.h#define U_SHAPE_TEXT_DIRECTION_VISUAL_RTLDraft
4.2 ushape.h#define U_SHAPE_YEHHAMZA_MASKDraft
4.2 ushape.h#define U_SHAPE_YEHHAMZA_TWOCELL_NEARDraft
4.2 ustdio.hUFILE\* u_fadopt(FILE\*, const char\*, const char\*)Draft
4.4 ustring.hUChar\* u_strFromJavaModifiedUTF8WithSub(UChar\*, int32_t,
int32_t\*, const char\*, int32_t, UChar32, int32_t\*, UErrorCode\*)Draft
4.4 ustring.hchar\* u_strToJavaModifiedUTF8(char\*, int32_t, int32_t\*, const
UChar\*, int32_t, UErrorCode\*)Draft
4.4 utf.h#define U_IS_SURROGATE_TRAILDraft
4.2 utf16.h#define U16_IS_SURROGATE_TRAILDraft
4.2 utypes.h#define U_CHARSET_IS_UTF8Draft
4.2 uvernum.h#define U_ICU_ENTRY_POINT_RENAMEDraft
4.2

[Jump to top](icu4c-api-comparison-4-4-2-with-4-6.md)

---
