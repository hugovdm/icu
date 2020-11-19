# ICU4J Number Parsing Problems

I was looking at ICU
[ticket#7128](http://bugs.icu-project.org/trac/ticket/7128). I decided to write
down this document for discussing what we should do.

### Background

In ICU4J 4.0, Mark Davis implemented lenient symbol parsing feature in
DecimalFormat under [ticket#5909](http://bugs.icu-project.org/trac/ticket/5909).
This code partially implemented items discussed in [this design
document](index.md). The implementation is actually beyond the description
available in 5909. I'm going to describe what was done in more detail in this
document. This change introduced several UnicodeSet static objects in
DecimalFormat class -
private static final UnicodeSet dotEquivalents =(UnicodeSet) new UnicodeSet(
"\[.\\u2024\\u3002\\uFE12\\uFE52\\uFF0E\\uFF61\]").freeze();
private static final UnicodeSet commaEquivalents = (UnicodeSet) new UnicodeSet(
"\[,\\u060C\\u066B\\u3001\\uFE10\\uFE11\\uFE50\\uFE51\\uFF0C\\uFF64\]").freeze();
private static final UnicodeSet otherGroupingSeparators = (UnicodeSet) new
UnicodeSet(
"\[\\\\
'\\u00A0\\u066C\\u2000-\\u200A\\u2018\\u2019\\u202F\\u205F\\u3000\\uFF07\]").freeze();
private static final UnicodeSet strictDotEquivalents =(UnicodeSet) new
UnicodeSet(
"\[.\\u2024\\uFE52\\uFF0E\\uFF61\]").freeze();
private static final UnicodeSet strictCommaEquivalents = (UnicodeSet) new
UnicodeSet(
"\[,\\u066B\\uFE10\\uFE50\\uFF0C\]").freeze();
private static final UnicodeSet strictOtherGroupingSeparators = (UnicodeSet) new
UnicodeSet(
"\[\\\\
'\\u00A0\\u066C\\u2000-\\u200A\\u2018\\u2019\\u202F\\u205F\\u3000\\uFF07\]").freeze();
private static final UnicodeSet defaultGroupingSeparators = (UnicodeSet) new
UnicodeSet(
dotEquivalents).addAll(commaEquivalents).addAll(otherGroupingSeparators).freeze();
private static final UnicodeSet strictDefaultGroupingSeparators = (UnicodeSet)
new UnicodeSet(
strictDotEquivalents).addAll(strictCommaEquivalents).addAll(strictOtherGroupingSeparators).freeze();
During number parsing, following lines create a couple of UnicodeSet instances -
UnicodeSet decimalSet = new UnicodeSet(getSimilarDecimals(decimal,
strictParse));
UnicodeSet groupingSet = new UnicodeSet(strictParse ?
strictDefaultGroupingSeparators : defaultGroupingSeparators)
.add(grouping).removeAll(decimalSet);
The implementation of getSimilarDecimals is below -
/\*\*
\* Return characters that are used where this decimal is used.
\* @param decimal
\* @param strictParse
\* @return
\*/
private UnicodeSet getSimilarDecimals(char decimal, boolean strictParse) {
if (dotEquivalents.contains(decimal)) {
return strictParse ? strictDotEquivalents : dotEquivalents;
}
if (commaEquivalents.contains(decimal)) {
return strictParse ? strictCommaEquivalents : commaEquivalents;
}
// if there is no match, return the character itself
return new UnicodeSet().add(decimal);
}
Before this change was made, the parse code simply compared a character with
decimalSeparator/groupingSeparator in DecimalFormatSymbols. UnicodeSet
decimalSet can be one of followings -
1. if DecimalFormatSymbols.decimalSeparator is a variant of "dot", use either
dotEquivalents (or strictDotEquivalents when NumberFormat#setParseStrict(true))
2. if DecimalFormatSymbols.decimalSeparator is a variant of "comma", use either
commaEquivalents (or strictCommaEquivalents when
NumberFormat#setParseString(true))
3. otherwise, use a UnicodeSet only containing
DecimalFormatSymbols.decimalSeparator itself.
*Note: I think there is a bug in this code. xxxEquivalents is a super set of strictXxxEquivalents. If DecimalFormatSymbols.decimalSeparator is a character included in xxxEquivalents, but not in strictXxxEquivalents, then the parser will miss DecimalFormatSymbols.decimalSeparator itself.*
The set of grouping separator character is defaultGroupingSeparators (or
strictDefaultGroupingSeparator when NumberFormat#setParseString(true)), plus
DecimalFormatSymbols.groupingSeparator, minus the set of decimal separator
characters produced by the logic above.
*Note: Again, I think there is a bug in this code too. If
DecimalFormatSymbols.groupingSeparator matches a character included in
decimalSet, the parser will miss DecimalFormatSymbols.groupingSeparator itself.*
During parse, check for decimal separator is done prior to grouping separator.
Once the parser found a character matching one of decimalSet, the parser
memorize the detected decimal separator character and does not interpret all
other characters in decimalSet as decimal separators. The same logic is used for
grouping separator.

### Issues

The [ticket#7128](http://bugs.icu-project.org/trac/ticket/7128) was reported by
Apache Harmony development team. Apache Harmony project wants to use ICU4J
NumberFormat/DecimalFormat as their JRE class library implementation. This
lenient symbol parsing is not implemented in JDK. Because this logic is
hardcoded in ICU4J DecimalFormat implementation, they cannot provide JDK
compatible behavior with ICU4J implementation.
This is also an undocumented feature. ICU4J JavaDoc does not explain how
decimal/grouping separators are parsed, and it also does not explain
NumberFormat#setParseString(boolean) affects the behavior.
So this behavior is totally undocumented/uncontrollable.

### **Goals**

The lenient symbol parsing behavior is probably useful in common use cases. But
it is sometimes harmful, mainly because it is not controllable. For example,
ticket#7128 complains that space (U+0020) characters are eaten by the parser.
Space characters might be used as separator for multiple numbers, rather than
grouping separator. However, the current implementation always interprets space
as grouping separator no matter how users configure DecimalFormatSymbols. So, I
think our goals are -
1. Lenient decimal/grouping separator parsing must be controllable via public
APIs - also it must be easily disabled (strictly use separators in
DecimalFormatSymbols)
2. The default behavior should use the current logic
3. API documentation should explain the default behavior (what characters are
interpreted as decimal/grouping separator) and effect of setParseStrict (if we
still want to use smaller set in the strict parsing mode - but I personally
think controlling the behavior from multiple APIs is not a good thing, because
it easily introduce inconsistencies)

### Design Options

**\[Option 1\] Configuring set of decimal/grouping separators used for parsing
in DecimalFormatSymbols**

Initially, I proposed to add public void setParseStrictSymbols(boolean) in
DecimalFormat for just skipping the lenient decimal/grouping separator match.
This is good enough to satisfy the JDK compatibility requirement. Mark Davis
countered that the behavior should be driven by new APIs in DateFormatSymbols
which allow ICU4J users to control the set of possible decimal/grouping
separator characters. With this approach, we would need new APIs like below -
In DateFormatSymbols
public void setParsingDecimalSeparators(UnicodeSet decimalSeparators)
public void setParsingGroupingSeparators(UnicodeSet groupingSeparators)
public UnicodeSet getParsingDecimalSeparators()
public UnicodeSet getParsingGroupingSeparators()
DateFormatSymbols also has decimal/grouping separators for monetary numbers,
thus, we would also need -
public void setParsingMonetaryDecimalSeparators(UnicodeSet decimalSeparators)
public void setParsingMonetaryGroupingSeparators(UnicodeSet groupingSeparators)
public UnicodeSet getParsingMonetaryDecimalSeparators()
public UnicodeSet getParsingMonetaryGroupingSeparators()
In addition to this, our current implementation use different set for strict
mode (NumberFormst#setParseStrict(true)). So, we may need strict variants -
public void setStrictParsingDecimalSeparators(UnicodeSet decimalSeparators)
public void setStrictParsingGroupingSeparators(UnicodeSet groupingSeparators)
public UnicodeSet getStrictParsingDecimalSeparators()
public UnicodeSet getStrictParsingGroupingSeparators()
public void setStrictParsingMonetaryDecimalSeparators(UnicodeSet
decimalSeparators)
public void setStrictParsingMonetaryGroupingSeparators(UnicodeSet
groupingSeparators)
public UnicodeSet getStrictParsingMonetaryDecimalSeparators()
public UnicodeSet getStrictParsingMonetaryGroupingSeparators()
This is obviously too much. In addition to this, the current implementation
switches the decimal separator set by the primary decimal separator character
(DecimalFormatSymbols.getDecimalSeparator()). Thus, calling
DecimalFormatSymbols.setDecimalSeparator(char) easily introduce inconsistency
problem between decimal separator used for formatting and separator variants
used for parsing. For these reasons, I think this is not a good solution.
**\[Option 2\] Simply expose dotEquivalents, commaEquivalents and defaultGroupingSeparators**
It's possible to make these UnicodeSets currently used by DecimalFormat
configurable, either via DecimalFormat APIs or DecimalFormatSymbols APIs. By
doing so, you can avoid the inconsistency problem between
DateFormatSymbols.setDecimalSeparator and
DateFormatSymbols.setParsingDecimalSeparator. With this solution, actual set
used for parsing decimal separator is populated with the logic currently used by
DecimalFormat. For example -
In DecimalFormat or DecimalFormatSymbols -
public void setDotEquivalents(UnicodeSet dotEquivalents)
public void setCommaEquivalents(UnicodeSet commaEquivalents)
public UnicodeSet getDotEquivalents()
public UnicodeSet getCommaEquivalents()
public void setAlternativeGroupingSeparators(UnicodeSet grpSeparators)
public UnicodeSet getAlternativeGroupingSeparators()
But, again, if we want to preserve current strict behavior, we need the same set
of APIs for strict
public void setStrictDotEquivalents(UnicodeSet dotEquivalents)
public void setStrictCommaEquivalents(UnicodeSet commaEquivalents)
public UnicodeSet getStrictDotEquivalents()
public UnicodeSet getStrictCommaEquivalents()
public void setStrictAlternativeGroupingSeparators(UnicodeSet grpSeparators)
public UnicodeSet getStrictAlternativeGroupingSeparators()
Actually, we could merge these two sets of APIs into one set by adding boolean
arg like below -
public void setDotEquivalents(UnicodeSet dotEquivalents, boolean strict)
public void setCommaEquivalents(UnicodeSet commaEquivalents, boolean strict)
public UnicodeSet getDotEquivalents(boolean strict)
public UnicodeSet getCommaEquivalents(boolean strict)
public void setAlternativeGroupingSeparators(UnicodeSet grpSeparators, boolean
strict)
public UnicodeSet getAlternativeGroupingSeparators(boolean strict)
This solution should work OK, but I do not like this because it's really
difficult to understand the relationship between decimal separator and
dotEquivalents/commaEquivalents. This just exposes what we're doing internally,
based on the assumption - decimal separator belongs to either dotEquivalents or
commaEquivalents in most case.
**\[Option 3\] Customizable matcher function**
Supporting lenient match through multiple UnicodeSets like Option 1 and 2
require a bunch of different sets and sometimes difficult to understand. Another
approach is to make the matching logic by customizable functions, which can hide
manipulation of multiple sets behind the API.
public class AlternativeDecimalSymbolMatcher implement Serializable {
public enum SymbolType {
DECIMAL_SEPARATOR,
GROUPING_SEPARATOR,
}
/\*\*
\* Return the length of matching symbol at the given position.
\*
\* @param text input text
\* @param idx position in the text
\* @param type Decimal symbol type
\* @param primarySymbol The primary symbol character for the type
\* @param narrowMatch true if matching is done more strictly (When
NumerFormat#setParseStrict(true),
\* DecimalFormat sets true to narrowMatch)
\* @return When the text segment at the given index matches an
\* alternative symbol for the symbol type, return the length of
\* matching symbol. Otherwise, return 0.
\*/
public int match(CharSequence text, int idx, SymbolType type, char
primarySymbol, boolean narrowMatch) {
....
....
}
}
With this option, DecimalFormat checks if a character at a certain position
matches the primary decimal separator (DecimalFormatSymbols.decimalSeparator, or
monetaryDecimalSeparator for currency number) or not. If not, then check if a
character matches the primary grouping separator
(DecimalFormatSymbols.groupingSeparator, or monetaryDecimalSeparator for
currency number) or not. When the character matches neither of them, then calls
DecimalSymbolMatcher.match with DECIMAL_SEPARATOR, then GROUPING_SEPARATOR in
this order until match is found. The default implementation uses existing logic
(dotEquivalent, commaEquivalent, etc..) We can create a static instance of
DecimalSymbolMatcher subclass doing nothing in this method for supporting JDK
compatible behavior. For example,
public class NoopDecimalSymbolMatcher extends DecimalSymbolMatcher {
public static final NoopDecimalSymbolMatcher INCETANCE = new
NoopDecimalSymbolMatcher();
public int match(CharSequence text, int idx, SymbolType type, char symbol) {
return 0;
}
}
In DecimalFormat, add new APIs -
public void setAlternativeDecimalySymbolMatcher(AlternativeDecimalSymbolMatcher
altSymMatcher)
public AlternativeDecimalSymbolMatcher getAlternativeSymbolMatcher()
This design allow more flexible customization without exploding API signature.
Also we could support more symbol types discussed in [this document](index.md)
in future.
