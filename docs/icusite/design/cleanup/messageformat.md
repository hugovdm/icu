# MessageFormat Redesign

As a part of the cleanup of MessageFormat, here's what we have planned.

1.  Have two interfaces that UFormat inherits from: FormatOnly and ParseOnly (if
    anyone has better names, let me know!)
2.  Replace API parameter types with these where we can (where it is an
    acceptable broadening)
3.  Create a MessageFormatOnly that only does formatting, thus only implements
    FormatOnly
    1.  It only does placeholder substitution for strings, and parse of strings.
    2.  It has a registry of FormatOnlys that it uses to format based on
        placeholder-keys / types, but it is unpopulated.
    3.  MessageFormat inherits from it, and populates the registry.

Note: longer term, I think we might want to have a more generalized interface:
an object that can match at a given point in a string, and (logically) return
back a set of results and how far through the string each represented, plus
flagged partial-unique results.
That is, with the contents {Jan => M1, January => M1, June => M6, July => M7},
we'd have the following results (pos, string => output-matches)

0, "JanuaryX" => <M1,3>, <M1,7>
0, "JanX" => <M1, 3>
0, "JaX" => <M1,2,partial>
0, "JX" => none
0, "Ju" => none
0, "Jul" => <M7,2,partial>
0, "July" => <M7,2>
