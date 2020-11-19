# Regular Expressions, UText based operation

[TOC]

This is a proposal made by Apple intern Jordan Rose in summer 2008 to change the
ICU4C regular expression engine to operate on UTexts. There are four sections to
this: Changes to i18n/unicode/regex.h, changes to i18n/unicode/uregex.h, and
changes to common/unicode/utext.h to add a UTEXT_CURRENT32 macro and to add
comparison functions.

Although it is often better to deal with UnicodeStrings or UChar strings, the
associated memory and performance cost of extracting characters can sometimes be
quite large. A regular expression may not have to deal with the entire string to
get a useful answer, and having multiple copies of the same data in memory is
never a great thing.

The performance cost of using UText rather than UnicodeString is certainly an
issue to consider here. However, by accessing the chunk buffer directly when
possible, the costs of the change are minimized. The new implementation remains,
on average, a bit slower, mostly because it is working with 64-bit native
indexes as required by UText, rather than 32-bit indexes as it has in the past.
(All externally-visible indexes are still 32-bit UTF-16 indexes into the texts
for backwards compatibility.)

The benefits, however, are clear: the regular expression engine can now be used
with external string formats, while still working perfectly well with
UnicodeStrings.

Changes to the C++ regex API are listed in the first section below; these are
new methods that take UText, paralleling existing methods that take
UnicodeString, plus generalizations of the behavior of a few existing methods.
Changes to the C uregex API are listed in the second section; these are also
mostly new function that take UText, paralleling existing functions that take
UnicodeString, plus generalizations of the behavior of a few existing functions.
Changes to the C utext API are listed in the other sections, adding utext
comparison functions and a new macro. Methods and constructors that did not
change are not included, nor any local variables. All of the additions / changes
are marked in yellow.

## Changes to regex.h (C++ API)

`/**`

` * Class <code>RegexPattern</code> represents a compiled regular expression. It
includes`

` * factory methods for creating a RegexPattern object from the source (string)
form`

` * of a regular expression, methods for creating RegexMatchers that allow the
pattern`

` * to be applied to input text, and a few convenience methods for simple
common`

` * uses of regular expressions.`

` *`

` * <p>Class RegexPattern is not intended to be subclassed.</p>`

` *`

` * @stable ICU 2.4`

` */`

`class U_I18N_API RegexPattern: public UObject {`

`public:`

` /**`

` * Compiles the regular expression in string form into a RegexPattern`

` * object. These compile methods, rather than the constructors, are the usual`

` * way that RegexPattern objects are created.`

` *`

` * <p>Note that RegexPattern objects must not be deleted while RegexMatcher`

` * objects created from the pattern are active. RegexMatchers keep a pointer`

` * back to their pattern, so premature deletion of the pattern is a`

` * catastrophic error.</p>`

` *`

` * <p>All pattern match mode flags are set to their default values.</p>`

` *`

` * <p>Note that it is often more convenient to construct a RegexMatcher
directly`

` * from a pattern string rather than separately compiling the pattern and`

` * then creating a RegexMatcher object from the pattern.</p>`

` *`

` * @param regex The regular expression to be compiled.`

` * @param pe Receives the position (line and column nubers) of any error`

` * within the regular expression.)`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A regexPattern object for the compiled pattern.`

` */`

` static RegexPattern * U_EXPORT2 compile( UText *regex,`

` UParseError &pe,`

` UErrorCode &status);`

` /**`

` * Compiles the regular expression in string form into a RegexPattern`

` * object using the specified match mode flags. These compile methods,`

` * rather than the constructors, are the usual way that RegexPattern objects`

` * are created.`

` *`

` * <p>Note that RegexPattern objects must not be deleted while RegexMatcher`

` * objects created from the pattern are active. RegexMatchers keep a pointer`

` * back to their pattern, so premature deletion of the pattern is a`

` * catastrophic error.</p>`

` *`

` * <p>Note that it is often more convenient to construct a RegexMatcher
directly`

` * from a pattern string instead of than separately compiling the pattern and`

` * then creating a RegexMatcher object from the pattern.</p>`

` *`

` * @param regex The regular expression to be compiled.`

` * @param flags The match mode flags to be used.`

` * @param pe Receives the position (line and column nubers) of any error`

` * within the regular expression.)`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A regexPattern object for the compiled pattern.`

` */`

` static RegexPattern * U_EXPORT2 compile( UText *regex,`

` uint32_t flags,`

` UParseError &pe,`

` UErrorCode &status);`

` /**`

` * Compiles the regular expression in string form into a RegexPattern`

` * object using the specified match mode flags. These compile methods,`

` * rather than the constructors, are the usual way that RegexPattern objects`

` * are created.`

` *`

` * <p>Note that RegexPattern objects must not be deleted while RegexMatcher`

` * objects created from the pattern are active. RegexMatchers keep a pointer`

` * back to their pattern, so premature deletion of the pattern is a`

` * catastrophic error.</p>`

` *`

` * <p>Note that it is often more convenient to construct a RegexMatcher
directly`

` * from a pattern string instead of than separately compiling the pattern and`

` * then creating a RegexMatcher object from the pattern.</p>`

` *`

` * @param regex The regular expression to be compiled.`

` * @param flags The match mode flags to be used.`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A regexPattern object for the compiled pattern.`

` */`

` static RegexPattern * U_EXPORT2 compile( UText *regex,`

` uint32_t flags,`

` UErrorCode &status);`

` /**`

` * Creates a RegexMatcher that will match the given input against this pattern.
The`

` * RegexMatcher can then be used to perform match, find or replace operations`

` * on the input. Note that a RegexPattern object must not be deleted while`

` * RegexMatchers created from it still exist and might possibly be used again.`

` * <p>`

` * The matcher will make a shallow clone of the supplied input text, and all
regexp`

` * pattern matching operations happen on this clone. While read-only operations
on`

` * the supplied text are permitted, it is critical that the underlying string
not be`

` * altered or deleted before use by the regular expression operations is
complete.`

` *`

` * @param input The input text to which the regular expression will be
applied.`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A RegexMatcher object for this pattern and input.`

` */`

` virtual RegexMatcher *matcher(UText *input,`

` UErrorCode &status) const;`

` /**`

` * Test whether a string matches a regular expression. This convenience
function`

` * both compiles the reguluar expression and applies it in a single operation.`

` * Note that if the same pattern needs to be applied repeatedly, this method
will be`

` * less efficient than creating and reusing a RegexMatcher object.`

` *`

` * @param regex The regular expression`

` * @param input The string data to be matched`

` * @param pe Receives the position of any syntax errors within the regular
expression`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return True if the regular expression exactly matches the full input
string.`

` *`

` * @stable ICU 2.4`

` */`

` static UBool U_EXPORT2 matches(UText *regex,`

` UText *input,`

` UParseError &pe,`

` UErrorCode &status);`

` /**`

` * Returns the regular expression from which this pattern was compiled. This
method will work`

` * even if the pattern was compiled from a UText.`

` *`

` * Note: If the pattern was originally compiled from a UText, and that UText
was modified,`

` * the returned string may no longer reflect the RegexPattern object.`

` * @stable ICU 2.4`

` */`

` virtual UnicodeString pattern() const;`

` /**`

` * Returns the regular expression from which this pattern was compiled. This
method will work`

` * even if the pattern was compiled from a UnicodeString.`

` *`

` * Note: This is the original input, not a clone. If the pattern was originally
compiled from a`

` * UText, and that UText was modified, the returned UText may no longer reflect
the RegexPattern`

` * object.`

` */`

` virtual UText *patternText() const;`

` /**`

` * Split a string into fields. Somewhat like split() from Perl.`

` * The pattern matches identify delimiters that separate the input`

` * into fields. The input data between the matches becomes the`

` * fields themselves.`

` * <p>`

` * For the best performance on split() operations,`

` * <code>RegexMatcher::split</code> is perferable to this function`

` *`

` * @param input The string to be split into fields. The field delimiters`

` * match the pattern (in the "this" object)`

` * @param dest An array of mutable UText structs to receive the results of the
split.`

` * If a field is NULL, a new UText is allocated to contain the results for`

` * that field. This new UText is not guaranteed to be mutable.`

` * @param destCapacity The number of elements in the destination array.`

` * If the number of fields found is less than destCapacity, the`

` * extra strings in the destination array are not altered.`

` * If the number of destination strings is less than the number`

` * of fields, the trailing part of the input string, including any`

` * field delimiters, is placed in the last destination string.`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return The number of fields into which the input string was split.`

` */`

` virtual int32_t split(UText *input,`

` UText *dest[],`

` int32_t destCapacity,`

` UErrorCode &status) const;`

`};`

`/**`

` * class RegexMatcher bundles together a reular expression pattern and`

` * input text to which the expression can be applied. It includes methods`

` * for testing for matches, and for find and replace operations.`

` *`

` * <p>Class RegexMatcher is not intended to be subclassed.</p>`

` *`

` * @stable ICU 2.4`

` */`

`class U_I18N_API RegexMatcher: public UObject {`

`public:`

` /**`

` * Construct a RegexMatcher for a regular expression.`

` * This is a convenience method that avoids the need to explicitly create`

` * a RegexPattern object. Note that if several RegexMatchers need to be`

` * created for the same expression, it will be more efficient to`

` * separately create and cache a RegexPattern object, and use`

` * its matcher() method to create the RegexMatcher objects.`

` *`

` * @param regexp The regular expression to be compiled.`

` * @param flags Regular expression options, such as case insensitive matching.`

` * @see UREGEX_CASE_INSENSITIVE`

` * @param status Any errors are reported by setting this UErrorCode variable.`

` */`

` RegexMatcher(UText *regexp, uint32_t flags, UErrorCode &status);`

` /**`

` * Construct a RegexMatcher for a regular expression.`

` * This is a convenience method that avoids the need to explicitly create`

` * a RegexPattern object. Note that if several RegexMatchers need to be`

` * created for the same expression, it will be more efficient to`

` * separately create and cache a RegexPattern object, and use`

` * its matcher() method to create the RegexMatcher objects.`

` * <p>`

` * The matcher will make a shallow clone of the supplied input text, and all
regexp`

` * pattern matching operations happen on this clone. While read-only operations
on`

` * the supplied text are permitted, it is critical that the underlying string
not be`

` * altered or deleted before use by the regular expression operations is
complete.`

` *`

` * @param regexp The Regular Expression to be compiled.`

` * @param input The string to match. The matcher retains a shallow clone of the
text.`

` * @param flags Regular expression options, such as case insensitive matching.`

` * @see UREGEX_CASE_INSENSITIVE`

` * @param status Any errors are reported by setting this UErrorCode variable.`

` */`

` RegexMatcher(UText *regexp, UText *input,`

` uint32_t flags, UErrorCode &status);`

` /**`

` * Returns a string containing the text matched by the previous match.`

` * If the pattern can match an empty string, an empty string may be returned.`

` * @param dest A mutable UText in which the matching text is placed.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status A reference to a UErrorCode to receive any errors.`

` * Possible errors are U_REGEX_INVALID_STATE if no match`

` * has been attempted or the last match failed.`

` * @return A string containing the matched input text. If a pre-allocated
UText`

` * was provided, it will always be used and returned.`

` */`

` virtual UText *group(UText *dest, UErrorCode &status) const;`

` /**`

` * Returns a string containing the text captured by the given group`

` * during the previous match operation. Group(0) is the entire match.`

` *`

` * @param groupNum the capture group number`

` * @param dest A mutable UText in which the matching text is placed.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status A reference to a UErrorCode to receive any errors.`

` * Possible errors are U_REGEX_INVALID_STATE if no match`

` * has been attempted or the last match failed.`

` * @return A string containing the matched input text. If a pre-allocated
UText`

` * was provided, it will always be used and returned.`

` */`

` virtual UText *group(int32_t groupNum, UText *dest, UErrorCode &status)
const;`

` /**`

` * Resets this matcher with a new input string. This allows instances of
RegexMatcher`

` * to be reused, which is more efficient than creating a new RegexMatcher for`

` * each input string to be processed.`

` * @param input The new string on which subsequent pattern matches will
operate.`

` * The matcher makes a shallow clone of the given text; ownership of the`

` * original string remains with the caller. Because no deep copy of the`

` * text is made, it is essential that the caller not modify the string`

` * until after regexp operations on it are done.`

` * @return this RegexMatcher.`

` */`

` virtual RegexMatcher &reset(UText *input);`

` /**`

` * Returns the input string being matched. Ownership of the string belongs to`

` * the matcher; it should not be altered or deleted. This method will work even
if the input`

` * was originally supplied as a UText.`

` * @return the input string`

` * @stable ICU 2.4`

` */`

` virtual const UnicodeString &input() const;`

` /**`

` * Returns the input string being matched. This is the live input text; it
should not be`

` * altered or deleted. This method will work even if the input was originally
supplied as`

` * a UnicodeString.`

` * @return the input text`

` */`

` virtual UText *inputText() const;`

` /**`

` * Returns the input string being matched, either by copying it into the
provided`

` * UText parameter or by returning a shallow clone of the live input. Note that
copying`

` * the entire input may cause significant performance and memory issues.`

` * @param dest The UText into which the input should be copied, or NULL to
create a new UText`

` * @return dest if non-NULL, a shallow copy of the input text otherwise`

` */`

` virtual UText *getInput(UText *dest) const;`

` /**`

` * Replaces every substring of the input that matches the pattern`

` * with the given replacement string. This is a convenience function that`

` * provides a complete find-and-replace-all operation.`

` *`

` * This method first resets this matcher. It then scans the input string`

` * looking for matches of the pattern. Input that is not part of any`

` * match is left unchanged; each match is replaced in the result by the`

` * replacement string. The replacement string may contain references to`

` * capture groups.`

` *`

` * @param replacement a string containing the replacement text.`

` * @param dest a mutable UText in which the results are placed.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status a reference to a UErrorCode to receive any errors.`

` * @return a string containing the results of the find and replace.`

` * If a pre-allocated UText was provided, it will always be used and returned.`

` */`

` virtual UText *replaceAll(UText *replacement, UText *dest, UErrorCode
&status);`

` /**`

` * Replaces the first substring of the input that matches`

` * the pattern with the replacement string. This is a convenience`

` * function that provides a complete find-and-replace operation.`

` *`

` * <p>This function first resets this RegexMatcher. It then scans the input
string`

` * looking for a match of the pattern. Input that is not part`

` * of the match is appended directly to the result string; the match is
replaced`

` * in the result by the replacement string. The replacement string may contain`

` * references to captured groups.</p>`

` *`

` * <p>The state of the matcher (the position at which a subsequent find()`

` * would begin) after completing a replaceFirst() is not specified. The`

` * RegexMatcher should be reset before doing additional find() operations.</p>`

` *`

` * @param replacement a string containing the replacement text.`

` * @param dest a mutable UText in which the results are placed.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status a reference to a UErrorCode to receive any errors.`

` * @return a string containing the results of the find and replace.`

` * If a pre-allocated UText was provided, it will always be used and returned.`

` */`

` virtual UText *replaceFirst(UText *replacement, UText *dest, UErrorCode
&status);`

` /**`

` * Implements a replace operation intended to be used as part of an`

` * incremental find-and-replace.`

` *`

` * <p>The input string, starting from the end of the previous replacement and
ending at`

` * the start of the current match, is appended to the destination string. Then
the`

` * replacement string is appended to the output string,`

` * including handling any substitutions of captured text.</p>`

` *`

` * <p>For simple, prepackaged, non-incremental find-and-replace`

` * operations, see replaceFirst() or replaceAll().</p>`

` *`

` * @param dest A mutable UText to which the results of the find-and-replace are
appended.`

` * Must not be NULL.`

` * @param replacement A UText that provides the text to be substituted for`

` * the input text that matched the regexp pattern. The replacement`

` * text may contain references to captured text from the input.`

` * @param status A reference to a UErrorCode to receive any errors. Possible`

` * errors are U_REGEX_INVALID_STATE if no match has been`

` * attempted or the last match failed, and U_INDEX_OUTOFBOUNDS_ERROR`

` * if the replacement text specifies a capture group that`

` * does not exist in the pattern.`

` *`

` * @return this RegexMatcher`

` */`

` virtual RegexMatcher &appendReplacement(UText *dest,`

` UText *replacement, UErrorCode &status);`

` /**`

` * As the final step in a find-and-replace operation, append the remainder`

` * of the input string, starting at the position following the last
appendReplacement(),`

` * to the destination string. <code>appendTail()</code> is intended to be
invoked after one`

` * or more invocations of the <code>RegexMatcher::appendReplacement()</code>.`

` *`

` * @param dest A mutable UText to which the results of the find-and-replace are
appended.`

` * Must not be NULL.`

` * @return the destination string.`

` */`

` virtual UText *appendTail(UText *dest);`

` /**`

` * Split a string into fields. Somewhat like split() from Perl.`

` * The pattern matches identify delimiters that separate the input`

` * into fields. The input data between the matches becomes the`

` * fields themselves.`

` *`

` * @param input The string to be split into fields. The field delimiters`

` * match the pattern (in the "this" object). This matcher`

` * will be reset to this input string.`

` * @param dest An array of mutable UText structs to receive the results of the
split.`

` * If a field is NULL, a new UText is allocated to contain the results for`

` * that field. This new UText is not guaranteed to be mutable.`

` * @param destCapacity The number of elements in the destination array.`

` * If the number of fields found is less than destCapacity, the`

` * extra strings in the destination array are not altered.`

` * If the number of destination strings is less than the number`

` * of fields, the trailing part of the input string, including any`

` * field delimiters, is placed in the last destination string.`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return The number of fields into which the input string was split.`

` */`

` virtual int32_t split(UText *input,`

` UText *dest[],`

` int32_t destCapacity,`

` UErrorCode &status);`

`private:`

` // Constructors and other object boilerplate are private.`

` // Instances of RegexMatcher can not be assigned, copied, cloned, etc.`

` void init2(UText *t, UErrorCode &e); // Common initialization, part 2. (now
takes a UText)`

`private:`

` // Several methods below now take native indexes.`

` //`

` // MatchAt This is the internal interface to the match engine itself.`

` // Match status comes back in matcher member variables.`

` //`

` void MatchAt(int64_t startIdx, UBool toEnd, UErrorCode &status);`

` inline void backTrack(int64_t &inputIdx, int32_t &patIdx);`

` UBool isWordBoundary(int64_t pos); // perform Perl-like \b test`

` UBool isUWordBoundary(int64_t pos); // perform RBBI based \b test`

` REStackFrame *resetStack();`

` inline REStackFrame *StateSave(REStackFrame *fp, int32_t savePatIdx,
UErrorCode &status);`

` void IncrementTime(UErrorCode &status);`

` int64_t appendGroup(int32_t groupNum, UText *dest, UErrorCode &status) const;`

` // These methods are used when we know the entire input is in the UText chunk
buffer.`

` // They are more efficient because they directly access it, rather than using
the UText API.`

` UBool findUsingChunk();`

` void MatchChunkAt(int32_t startIdx, UBool toEnd, UErrorCode &status);`

` UBool isChunkWordBoundary(int32_t pos);`

`};`

## Changes to uregex.h (C API)

`/**`

` * Open (compile) an ICU regular expression. Compiles the regular expression
in`

` * string form into an internal representation using the specified match mode
flags.`

` * The resulting regular expression handle can then be used to perform various`

` * matching operations.`

` * <p>`

` * The contents of the pattern UText will be extracted and saved. Ownership of
the`

` * UText struct itself remains with the caller. This is to match the behavior
of`

` * uregex_open().`

` *`

` * @param pattern The Regular Expression pattern to be compiled. `

` * @param flags Flags that alter the default matching behavior for`

` * the regular expression, UREGEX_CASE_INSENSITIVE, for`

` * example. For default behavior, set this parameter to zero.`

` * See <code>enum URegexpFlag</code>. All desired flags`

` * are bitwise-ORed together.`

` * @param pe Receives the position (line and column nubers) of any syntax`

` * error within the source regular expression string. If this`

` * information is not wanted, pass NULL for this parameter.`

` * @param status Receives error detected by this function.`

` *`

` */`

`U_DRAFT URegularExpression * U_EXPORT2`

`uregex_openUText(UText *pattern,`

` uint32_t flags,`

` UParseError *pe,`

` UErrorCode *status);`

`/**`

` * Returns a pointer to the source form of the pattern for this regular
expression.`

` * This function will work even if the pattern was originally specified as a
UText.`

` *`

` * @param regexp The compiled regular expression.`

` * @param patLength This output parameter will be set to the length of the`

` * pattern string. A NULL pointer may be used here if the`

` * pattern length is not needed, as would be the case if`

` * the pattern is known in advance to be a NUL terminated`

` * string.`

` * @param status Receives errors detected by this function.`

` * @return a pointer to the pattern string. The storage for the string is`

` * owned by the regular expression object, and must not be`

` * altered or deleted by the application. The returned string`

` * will remain valid until the regular expression is closed.`

` * @stable ICU 3.0`

` */`

`U_STABLE const UChar * U_EXPORT2 `

`uregex_pattern(const URegularExpression *regexp,`

` int32_t *patLength,`

` UErrorCode *status);`

`/**`

` * Returns the source text of the pattern for this regular expression.`

` * This function will work even if the pattern was originally specified as a
UChar string.`

` *`

` * @param regexp The compiled regular expression.`

` * @param status Receives errors detected by this function.`

` * @return the pattern text. The storage for the text is owned by the regular
expression`

` * object, and must not be altered or deleted.`

` */`

`U_STABLE UText * U_EXPORT2 `

`uregex_patternUText(const URegularExpression *regexp,`

` UErrorCode *status);`

`/**`

` * Set the subject text string upon which the regular expression will look for
matches.`

` * This function may be called any number of times, allowing the regular`

` * expression pattern to be applied to different strings.`

` * <p>`

` * Regular expression matching operations work directly on the application's`

` * string data; only a shallow clone is made. The subject string data must not
be`

` * altered after calling this function until after all regular expression`

` * operations involving this string data are completed. `

` *`

` * @param regexp The compiled regular expression.`

` * @param text The subject text string.`

` * @param status Receives errors detected by this function.`

` */`

`U_DRAFT void U_EXPORT2 `

`uregex_setUText(URegularExpression *regexp,`

` UText *text,`

` UErrorCode *status);`

`/**`

` * Get the subject text that is currently associated with this `

` * regular expression object. If the input was supplied using
uregex_setText(),`

` * that pointer will be returned. Otherwise, the characters in the input will`

` * be extracted to a buffer and returned. In either case, ownership remains`

` * with the regular expression object.`

` *`

` * This function will work even if the input was originally specified as a
UText.`

` *`

` * @param regexp The compiled regular expression.`

` * @param textLength The length of the string is returned in this output
parameter. `

` * A NULL pointer may be used here if the`

` * text length is not needed, as would be the case if`

` * the text is known in advance to be a NUL terminated`

` * string.`

` * @param status Receives errors detected by this function.`

` * @return Pointer to the subject text string currently associated with`

` * this regular expression.`

` * @stable ICU 3.0`

` */`

`U_STABLE const UChar * U_EXPORT2 `

`uregex_getText(URegularExpression *regexp,`

` int32_t *textLength,`

` UErrorCode *status);`

`/**`

` * Get the subject text that is currently associated with this `

` * regular expression object.`

` *`

` * This function will work even if the input was originally specified as a
UChar string.`

` *`

` * @param regexp The compiled regular expression.`

` * @param dest A mutable UText in which to store the current input.`

` * If NULL, a new UText will be created as an immutable shallow clone`

` * of the actual input string.`

` * @param status Receives errors detected by this function.`

` * @return The subject text currently associated with this regular expression.`

` * If a pre-allocated UText was provided, it will always be used and returned.`

` */`

`U_DRAFT UText * U_EXPORT2 `

`uregex_getUText(URegularExpression *regexp,`

` UText *dest,`

` UErrorCode *status);`

`/** Extract the string for the specified matching expression or subexpression.`

` * Group #0 is the complete string of matched text.`

` * Group #1 is the text matched by the first set of capturing parentheses.`

` *`

` * @param regexp The compiled regular expression.`

` * @param groupNum The capture group to extract. Group 0 is the complete`

` * match. The value of this parameter must be`

` * less than or equal to the number of capture groups in`

` * the pattern.`

` * @param dest Mutable UText to receive the matching string data.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return The matching string data. If a pre-allocated UText was provided,`

` * it will always be used and returned.`

` */`

`U_DRAFT UText * U_EXPORT2 `

`uregex_groupUText(URegularExpression *regexp,`

` int32_t groupNum,`

` UText *dest,`

` UErrorCode *status);`

`/**`

` * Replaces every substring of the input that matches the pattern`

` * with the given replacement string. This is a convenience function that`

` * provides a complete find-and-replace-all operation.`

` *`

` * This method scans the input string looking for matches of the pattern. `

` * Input that is not part of any match is copied unchanged to the`

` * destination buffer. Matched regions are replaced in the output`

` * buffer by the replacement string. The replacement string may contain`

` * references to capture groups; these take the form of $1, $2, etc.`

` *`

` * @param regexp The compiled regular expression.`

` * @param replacement A string containing the replacement text.`

` * @param dest A mutable UText that will receive the result.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A UText containing the results of the find and replace.`

` * If a pre-allocated UText was provided, it will always be used and returned.`

` */`

`U_DRAFT UText * U_EXPORT2 `

`uregex_replaceAllUText(URegularExpression *regexp,`

` UText *replacement,`

` UText *dest,`

` UErrorCode *status);`

`/**`

` * Replaces the first substring of the input that matches the pattern`

` * with the given replacement string. This is a convenience function that`

` * provides a complete find-and-replace operation.`

` *`

` * This method scans the input string looking for a match of the pattern. `

` * All input that is not part of the match is copied unchanged to the`

` * destination buffer. The matched region is replaced in the output`

` * buffer by the replacement string. The replacement string may contain`

` * references to capture groups; these take the form of $1, $2, etc.`

` *`

` * @param regexp The compiled regular expression.`

` * @param replacement A string containing the replacement text.`

` * @param dest A mutable UText that will receive the result.`

` * If NULL, a new UText will be created (which may not be mutable).`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return A UText containing the results of the find and replace.`

` * If a pre-allocated UText was provided, it will always be used and returned.`

` */`

`U_DRAFT UText * U_EXPORT2 `

`uregex_replaceFirstUText(URegularExpression *regexp,`

` UText *replacement,`

` UText *dest,`

` UErrorCode *status);`

`/**`

` * Implements a replace operation intended to be used as part of an`

` * incremental find-and-replace.`

` *`

` * <p>The input string, starting from the end of the previous match and ending
at`

` * the start of the current match, is appended to the destination string. Then
the`

` * replacement string is appended to the output string,`

` * including handling any substitutions of captured text.</p>`

` *`

` * <p>For simple, prepackaged, non-incremental find-and-replace`

` * operations, see replaceFirst() or replaceAll().</p>`

` *`

` * @param regexp The regular expression object. `

` * @param replacementText The string that will replace the matched portion of
the`

` * input string as it is copied to the destination buffer.`

` * The replacement text may contain references ($1, for`

` * example) to capture groups from the match.`

` * @param dest A mutable UText that will receive the result. Must not be NULL.`

` * @param status A reference to a UErrorCode to receive any errors. `

` */`

`U_DRAFT void U_EXPORT2 `

`uregex_appendReplacementUText(URegularExpression *regexp,`

` UText *replacementText,`

` UText *dest,`

` UErrorCode *status);`

`/**`

` * As the final step in a find-and-replace operation, append the remainder`

` * of the input string, starting at the position following the last match,`

` * to the destination string. <code>uregex_appendTailUText()</code> is intended
`

` * to be invoked after one or more invocations of the`

` * <code>uregex_appendReplacementUText()</code> function.`

` *`

` * @param regexp The regular expression object. This is needed to `

` * obtain the input string and with the position`

` * of the last match within it.`

` * @param dest A mutable UText that will receive the result. Must not be NULL.`

` * @param status A reference to a UErrorCode to receive any errors. `

` * @return The destination UText.`

` *`

` * @stable ICU 3.0`

` */`

`U_DRAFT UText * U_EXPORT2 `

`uregex_appendTailUText(URegularExpression *regexp,`

` UText *dest);`

` /**`

` * Split a string into fields. Somewhat like split() from Perl.`

` * The pattern matches identify delimiters that separate the input`

` * into fields. The input data between the matches becomes the`

` * fields themselves.`

` * <p>`

` * The behavior of this function is not very closely aligned with
uregex_split();`

` * instead, it is based on (and implemented directly on top of) the C++ split
method.`

` *`

` * @param regexp The compiled regular expression.`

` * @param dest An array of mutable UText structs to receive the results of the
split.`

` * If a field is NULL, a new UText is allocated to contain the results for`

` * that field. This new UText is not guaranteed to be mutable.`

` * @param destCapacity The number of elements in the destination array.`

` * If the number of fields found is less than destCapacity, the`

` * extra strings in the destination array are not altered.`

` * If the number of destination strings is less than the number`

` * of fields, the trailing part of the input string, including any`

` * field delimiters, is placed in the last destination string.`

` * This behavior mimics that of Perl. It is not an error condition, and no`

` * error status is returned when all destField positions are used.`

` * @param status A reference to a UErrorCode to receive any errors.`

` * @return The number of fields into which the input string was split.`

` */`

`U_DRAFT int32_t U_EXPORT2 `

`uregex_splitUText(URegularExpression *regexp,`

` UText *destFields[],`

` int32_t destFieldsCapacity,`

` UErrorCode *status);`

## Changes to common/unicode/utext.h to add a UTEXT_CURRENT32 macro

This is a simple proposal to add a UTEXT_CURRENT32 macro to the UText API.
Although it is not commonly needed, it is useful enough to deserve a place
alongside UTEXT_NEXT32 and UTEXT_PREVIOUS32.

`/**`

` * inline version of utext_current32(), for performance-critical situations.`

` *`

` * Get the code point at the current iteration position of the UText.`

` * Returns U_SENTINEL (-1) if the position is at the end of the`

` * text.`

` */`

`#define UTEXT_CURRENT32(ut) \`

` ((ut)->chunkOffset < (ut)->chunkLength &&
((ut)->chunkContents)[(ut)->chunkOffset]<0xd800 ? \`

` ((ut)->chunkContents)[((ut)->chunkOffset)] : utext_current32(ut))`

## Changes to common/unicode/utext.h to add comparison functions

Although it is often better to deal with UnicodeStrings or UChar strings, the
associated memory and performance cost of extracting characters can sometimes be
quite large. A comparison may often only need to look at the first few
characters of a string to give a useful result.

Because a comparison starts at each UText's current iteration position,
comparing a text to itself at a different offset must be done with a clone.

The native-limit versions of these functions are needed for the above proposal
to shift the regular expression engine over to use UText natively, but could be
retained as internal-only if the proposal is rejected.

`/**`

` * Compare two UTexts (binary order). The comparison begins at each source
text's`

` * iteration position. The iteration position of each UText will be left
following`

` * the last character compared.`

` *`

` * The comparison is done in code point order; unlike u_strCompare, you`

` * cannot choose to use code unit order. This is because the characters`

` * in a UText are accessed one code point at a time, and may not be from a
UTF-16`

` * context.`

` *`

` * This functions works with strings of different explicitly specified lengths`

` * unlike the ANSI C-like u_strcmp() and u_memcmp() etc.`

` * A length argument of -1 signifies that as much of the string should be used
as`

` * is necessary to compare with the other string. If both length arguments are
-1,`

` * the entire remaining portionss of both strings are used.`

` *`

` * @param s1 First source string.`

` * @param length1 Length of first source string in UTF-32 code points.`

` *`

` * @param s2 Second source string.`

` * @param length2 Length of second source string in UTF-32 code points.`

` *`

` * @return <0 or 0 or >0 as usual for string comparisons`

` */`

`U_DRAFT int32_t U_EXPORT2`

`utext_compare(UText *s1, int32_t length1,`

` UText *s2, int32_t length2); `

`/**`

` * Compare two UTexts (binary order). The comparison begins at each source
text's`

` * iteration position. The iteration position of each UText will be left
following`

` * the last character compared. This method differs from utext_compare in that`

` * it accepts native limits rather than lengths for each string.`

` *`

` * The comparison is done in code point order; unlike u_strCompare, you`

` * cannot choose to use code unit order. This is because the characters`

` * in a UText are accessed one code point at a time, and may not be from a
UTF-16`

` * context.`

` *`

` * This functions works with strings of different explicitly specified lengths`

` * unlike the ANSI C-like u_strcmp() and u_memcmp() etc.`

` * A limit argument of -1 signifies that as much of the string should be used
as`

` * is necessary to compare with the other string. If both limit arguments are
-1,`

` * the entire remaining portionss of both strings are used.`

` *`

` * @param s1 First source string.`

` * @param limit1 Native index of the last character in the first source string
to be considered.`

` *`

` * @param s2 Second source string.`

` * @param limit2 Native index of the last character in the second source string
to be considered.`

` *`

` * @return <0 or 0 or >0 as usual for string comparisons`

` */`

`U_DRAFT int32_t U_EXPORT2`

`utext_compareNativeLimit(UText *s1, int64_t limit1,`

` UText *s2, int64_t limit2); `

`/**`

` * Compare two UTexts case-insensitively using full case folding. The
comparison`

` * begins at each source text's iteration position. The iteration position of
each`

` * UText will be left following the last character compared.`

` *`

` * The comparison is done in code point order; this is because the characters`

` * in a UText are accessed one code point at a time, and may not be from a
UTF-16`

` * context.`

` *`

` * This functions works with strings of different explicitly specified lengths`

` * unlike the ANSI C-like u_strcmp() and u_memcmp() etc.`

` * A length argument of -1 signifies that as much of the string should be used
as`

` * is necessary to compare with the other string. If both length arguments are
-1,`

` * the entire remaining portionss of both strings are used.`

` *`

` * @param s1 First source string.`

` * @param length1 Length of first source string in UTF-32 code points.`

` *`

` * @param s2 Second source string.`

` * @param length2 Length of second source string in UTF-32 code points.`

` *`

` * @param options A bit set of options:`

` * - U_FOLD_CASE_DEFAULT or 0 is used for default options:`

` * Comparison in code point order with default case folding.`

` *`

` * - U_FOLD_CASE_EXCLUDE_SPECIAL_I`

` *`

` * @param pErrorCode Must be a valid pointer to an error code value,`

` * which must not indicate a failure before the function call.`

` *`

` * @return <0 or 0 or >0 as usual for string comparisons`

` */`

`U_DRAFT int32_t U_EXPORT2`

`utext_caseCompare(UText *s1, int32_t length1,`

` UText *s2, int32_t length2,`

` uint32_t options, UErrorCode *pErrorCode); `

`/**`

` * Compare two UTexts case-insensitively using full case folding. The
comparison`

` * begins at each source text's iteration position. The iteration position of
each`

` * UText will be left following the last character compared. This method
differs from`

` * utext_caseCompare in that it accepts native limits rather than lengths for
each`

` * string.`

` *`

` * The comparison is done in code point order; this is because the characters`

` * in a UText are accessed one code point at a time, and may not be from a
UTF-16`

` * context.`

` *`

` * This functions works with strings of different explicitly specified lengths`

` * unlike the ANSI C-like u_strcmp() and u_memcmp() etc.`

` * A limit argument of -1 signifies that as much of the string should be used
as`

` * is necessary to compare with the other string. If both length arguments are
-1,`

` * the entire remaining portionss of both strings are used.`

` *`

` * @param s1 First source string.`

` * @param limit1 Native index of the last character in the first source string
to be considered.`

` *`

` * @param s2 Second source string.`

` * @param limit2 Native index of the last character in the second source string
to be considered.`

` *`

` * @param options A bit set of options:`

` * - U_FOLD_CASE_DEFAULT or 0 is used for default options:`

` * Comparison in code point order with default case folding.`

` *`

` * - U_FOLD_CASE_EXCLUDE_SPECIAL_I`

` *`

` * @param pErrorCode Must be a valid pointer to an error code value,`

` * which must not indicate a failure before the function call.`

` *`

` * @return <0 or 0 or >0 as usual for string comparisons`

` */`

`U_DRAFT int32_t U_EXPORT2`

`utext_caseCompareNativeLimit(UText *s1, int64_t limit1,`

` UText *s2, int64_t limit2,`

` uint32_t options, UErrorCode *pErrorCode); `
