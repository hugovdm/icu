# UnicodeSet Redesign

Here are my thoughts on UnicodeSet redesign.

## Problems

*   Slow parsing of patterns
*   Customizable, but not easily
*   Doesn't match latest recommended syntax (&&, ||, ...)
*   Parsing drags in a lot of other code.

## Proposal

1.  Create a UnicodeSetBuilder class, implementing Parser. Takes a pattern and
    produces a UnicodeSet. It can be in a different module than icu4j-core.
2.  Wire those UnicodeSet methods (and constructor) that take a pattern to use a
    static UnicodeSetBuilder field, which is set by reflection. If a
    UnicodeSetBuilder cannot be created, return UnsupportedOperationException

UnicodeSetBuilder has optional settings. These can be set with setters, and also
with the constuctors.

*   a keepPattern boolean (default false)
*   a list of VariableHandlers (you can add one, get the current list, or clear)
*   a list of PropertyHander (you can add one, get the current list, or clear)

**VariableHandler** is called whenever $ is hit. It is responsible for
determining whether there is a match, returning a substitute string, and telling
how far it parsed. Returns false if there is no match. By default, there is no
VariableHandler.
**PropertyHandler** is called whenever \[: or \\p{ or \\P{ is hit. It is
responsible for determining whether there is a match, returning a UnicodeSet,
and telling how far it parsed (including the terminating :\] or }. Returns false
if there is no match. By default, there is a PropertyHandler with the current
behavior, calling the UCharacter / UProperty methods with patterns like
\[:gc=Cn:\]. However, the implementation of the PropertyHandler will avoid using
exceptions (which the current implementation overuses).
Both are chained, with the latest addition called first. If any handler returns
true, its values are used and the rest are skipped.
A UnicodeSetBuilder internally keeps a UnicodeSet that it is building, and has
public methods for adding, retaining, removing, etc. These are public, so that
people using it directly can do the following:
UnicodeSet foo = new
UnicodeSetBuilder().add("\[\[:gc=L:\]\[:sc=Grek:\]\]").add(0xB7).build();
Or a UnicodeSetBuilder can use the normal parse methods, allowing people to find
out how far it gets.
UnicodeSet foo = new UnicodeSetBuilder().parse(myString, myParsePosition);
Option. If the keepPattern is true, then we could add to any pattern when we do
an add/retain/remove, something that the current implementation doesn't allow. I
don't think this is worth doing now, but maybe in the future.
