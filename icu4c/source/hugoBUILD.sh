TESTS=""

# Common testdata
TESTS="$TESTS utility/LocaleMatcherTest/testDataDriven"

# For MaybeStackArray status plumbing:
TESTS="$TESTS utility/MultithreadTest/TestBreakTranslit"

# # Basic Measures, usage-less mixed
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitMeasure"

# Usage: Mixed
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitMixedMeasure"
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitCompoundMeasure"
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitSkeletons"
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/microPropsInternals"

# Usage: Precision
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitUsage"
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitUsageErrorCodes"
# TESTS="$TESTS format/NumberTest/NumberFormatterApiTest/unitUsageSkeletons"

TESTS="$TESTS format/NumberTest/NumberFormatterApiTest"
TESTS="$TESTS format/NumberTest/NumberSkeletonTest"
# TESTS="$TESTS format"

# NumberRangeFormatterTest/testBasic - I forgot why:
# TESTS="$TESTS format/NumberTest/NumberRangeFormatterTest/testBasic"

# Morpheus internals tests:
TESTS="$TESTS format/UnitsRouterTest"
TESTS="$TESTS format/UnitsTest"
TESTS="$TESTS format/UnitsDataTest"

TESTS="$TESTS format/MeasureFormatTest"

# Focused testing overrides
# TESTS="format/UnitsTest/testComplexUnitsConverter"

# TESTS="$TESTS FIXME/cmemory"
# TESTS="$TESTS format/NumberFormatTest/TestUsageInternals"

# TESTS="format/MeasureFormatTest/TestSerializeSingleAssertFailure"
