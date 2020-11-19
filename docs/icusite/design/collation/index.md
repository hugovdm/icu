# Collation

## Add collation tailoring syntax for compact lists of characters with same level strength difference

Adding the following operators to the collation tailoring syntax:

"&a<b<c<d"

can be expressed as

"&a<\*bcd"

assuming the elements are not contractions. This is analogous to CLDR's <pc>
tag, one of the Abbreviating Ordering Specifications in the [LDML
spec](http://www.unicode.org/reports/tr35/#Collation_Elements). "<\*", "<<\*",
"<<<\*", and "=\*" will all be supported, corresponding to <pc>, <tc>, <sc> and
<ic> respectively.

See [ICU ticket #7015](http://bugs.icu-project.org/trac/ticket/7015).

This is related to the [ideas for reducing the size of collation resource
bundles](../size/collation.md).

## Add \[import\] option to collation tailoring syntax

Allow collation rule strings to import other collation rule strings via an
\[import\] option. This will be useful for maintenance and size purposes,
especially for the European Ordering Rules and Unihan. For all practical
purposes, the imported string would directly replace the \[import\] in the
original rules string.

\[import de-u-co-phonebk\] could be used to import a specific collation, or just
\[import de\] to import the standard one for the language.

See [ICU ticket #7023](http://bugs.icu-project.org/trac/ticket/7023).

See also [CLDR bug
2268](http://www.unicode.org/cldr/bugs/locale-bugs?findid=2268) and the [CLDR
design doc for proposed collation
additions](https://sites.google.com/site/cldr/development/development-process/design-proposals/collation-additions).

This is related to the [ideas for reducing the size of collation resource
bundles](../size/collation.md).

## Script reordering

***Moved to*** [***Script Reordering***](script-reordering/index.md)***.***
