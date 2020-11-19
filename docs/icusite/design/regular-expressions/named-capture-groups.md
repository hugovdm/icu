# Named Capture Groups

This proposal for supporting named capture groups in ICU closely follows Java,
which introduced them with Java 7.

Different regular expression implementations vary considerably in their syntax
for named capture. This page gives a nice overview:
<http://www.regular-expressions.info/named.html>

Java seems to have chosen a reasonable common denominator among the options that
are out there.

Proposed for ICU:

**Pattern Syntax**:

> (?<*name*> ... )

Notes:

*   The angle brackets are literal - they appear in the pattern.
*   Names are ASCII letters and numbers only, beginning with a letter. No dashes
    or underscores.
*   Two occurrences of the same name in a pattern is an error. (Perl and .NET
    allow this; Java & Python do not)
*   Named capture groups may also be referenced by number, in the traditional
    capture group way.

**Back Reference in Pattern Syntax**:

> \\k<*name*>

**Replacement String Syntax**:

> ${*name*}

**ICU API Additions**:

File utypes.h, in enum UErrorCode, add

U_REGEX_INVALID_CAPTURE_GROUP_NAME /\*\*< Invalid capture group name. @draft ICU
55 \*/

This error will be set both when parsing patterns with syntactically invalid
names, and by API functions taking a name parameter when the specified name does
not appear in the pattern.

I propose a minimalist API, only adding functions to obtain the capture group
number for a name. Given the corresponding number, the existing API relating to
capture groups can then be used.

I am not proposing to provide overloads to the existing group(), start() and
end() functions that take a group name instead of a group number. Their are 13
existing capture group functions, between the plain C and C++ APIs. Group names
could plausibly be provided as either invariant (char \*) strings, as
UnicodeString or as (UChar \*). To provide all the plausible variants would be a
substantial API bloat. The extra user code to get a number from a name is pretty
minimal.

We could always add a few convenience overloads later, if there is demand.

**C++ API, file regex.h**

class RegexPattern {

/\*\*

\* Get the group number corresponding to a named capture group.

\* The returned number can be used with any function that access

\* capture groups by number.

\*

\* The function returns an error status if the specified name does not

\* appear in the pattern.

\*

\* @param groupName The capture group name.

\* @param status A UErrorCode to receive any errors.

\*

\* @draft ICU 55

\*/

virtual int32_t groupNumberFromName(const UnicodeString &groupName, UErrorCode
&status) const;

/\*\*

\* Get the group number corresponding to a named capture group.

\* The returned number can be used with any function that access

\* capture groups by number.

\*

\* The function returns an error status if the specified name does not

\* appear in the pattern.

\*

\* @param groupName The capture group name,

\* platform invariant characters only.

\* @param nameLength The length of the name, or -1 if the name is

\* nul-terminated.

\* @param status A UErrorCode to receive any errors.

\*

\* @draft ICU 55

\*/

virtual int32_t groupNumberFromName(const char \*groupName, int32_t nameLength,
UErrorCode &status) const;

**Plain C API, file uregex.h**

/\*\*

\* Get the group number corresponding to a named capture group.

\* The returned number can be used with any function that access

\* capture groups by number.

\*

\* The function returns an error status if the specified name does not

\* appear in the pattern.

\*

\* @param regexp The compiled regular expression.

\* @param groupName The capture group name.

\* @param nameLength The length of the name, or -1 if the name is a

\* nul-terminated string.

\* @param status A pointer to a UErrorCode to receive any errors.

\*

\* @draft ICU 55

\*/

U_DRAFT int32_t U_EXPORT2

uregex_groupNumberFromName(URegularExpression \*regexp,

const UChar \*groupName,

int32_t nameLength,

UErrorCode \*status);

/\*\*

\* Get the group number corresponding to a named capture group.

\* The returned number can be used with any function that access

\* capture groups by number.

\*

\* The function returns an error status if the specified name does not

\* appear in the pattern.

\*

\* @param regexp The compiled regular expression.

\* @param groupName The capture group name,

\* platform invariant characters only.

\* @param nameLength The length of the name, or -1 if the name is

\* nul-terminated.

\* @param status A pointer to a UErrorCode to receive any errors.

\*

\* @draft ICU 55

\*/

U_DRAFT int32_t U_EXPORT2

uregex_groupNumberFromCName(URegularExpression \*regexp,

const char \*groupName,

int32_t nameLength,

UErrorCode \*status);
