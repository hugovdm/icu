# Flat CLDR Locale Data

Among the semantic and API issues with ICU resource bundles is the fallback of
nested (non-top-level) resource items.

ICU resource bundles mostly behave like Java and Windows resource bundles: They
perform fallback on the top level, and, like Windows resource bundles, they
support structured data. (In the case of ICU, the structure is recursive and
arbitrarily nestable, unlike Windows which provides fixed types like dialog box
resources.) The idea is that you don't want the x coordinate of a dialog box to
fallback, but only let the lookup for the whole dialog box fall back.

Fallback of nested items is bolted on top of the regular logic for the purpose
of implementing CLDR locale data fallbacks while more or less retaining the
nested structure of LDML.

One way to deal with this is to generalize and make public API for optional
nested fallback.

*Another way to deal with this is to flatten the locale data in ICU locale
resource bundles, just like the CLDR tools code internally flattens the data
using XPaths.*

We could flatten the paths to single keys for every LDML item that is an atomic
unit and is subject to fallbacks. Aside from most simple LDML items, this
includes whole collation tailorings: For example, we would have a flat key for
collations/phonebook which would access a Table with Sequence, Version etc.
because there is no nested fallback for those. CLDR tools already have data and
logic for distinguishing paths and atomic units.

With flattened locale data, there is no need to supported nested-item fallbacks,
and we can remove the code, internal APIs, and semantic confusions about them.

Looking at this another way, it would keep the resource bundle mechanism simple,
and shift the complexity of dealing with CLDR locale data out to CLDR-specific
code.

## Paths Flattened to Keys

Logically, we would use top-level key strings that are LDML distinguishing
XPaths.

TODO: Give examples of actual LDML XPaths.

The path keys would be fairly long. They would be shared via the pool bundle,
but the pool bundle would probably be significantly larger than the current
15kB.

TODO(markus): Measure the size of a pool bundle that has every resource path in
ICU turned into a single key.

The LDML2ICUConverter could generate compact path keys. The runtime code could
either use such compact keys directly, or map from readable keys to compact
ones.

We need to figure out whether it needs to be possible and easy to parse compact
keys and map them back to full paths.

*Keys could be compacted by turning most path elements into one or two
characters each.* Path elements from large sets could be left untouched (e.g.,
language and region codes) or mapped to short-ish strings if there is already an
established mapping (e.g., 5-letter UN codes for time zones). The mapping for
each path element needs to be unique only at the level at which it occurs
because it will be prefixed by higher-level path elements.

For keeping keys parseable and easily truncatable, if we don't use path element
separator punctuation, then we may need another way to indicate the start or end
of a path element, for example by titlecasing the short form of each path
element.

For example:

*   date/calendar/gregorian/months/format/narrow/1 → DCGrMFN1
*   languages/gsw → LGsw

In code, we might have an LDMLKey class that maps from a readable form to the
compact form. We might have an LDMLElements enumeration and use it like

> myKey = LDMLKey.make(LDMLElements.date, LDMLElements.calendar,
> LDMLElements.gregorian, ...);
> value = de_AT_bundle.getString(myKey + "1"); // get the de_AT month name for
> January

Alternatively, *keys could be compacted by calculating 18-bit hash codes for
them and turning each hash code into a 3-character base64 string.* If there are
collisions, we may need to modify the hash function. This would probably be
fastest and simplest, but make keys completely unreadable and unparseable. (In
other words, while this might be the most compact key string storage, it may not
lend itself to the kind of processing we need to do for CLDR data.)

One consideration is at what nodes in the LDML structure we need to be able to
enumerate the available items. At such nodes we probably need keys that we can
tokenize into path elements, and calculate the parent path key as a key prefix.
