# RBBI Rule Enhancements

Motivations

*   The RBBI rules as of ICU 4.6 are unable to express the UAX-14 line breaking
    behavior of Unicode 6.0. Some extensions are needed. The problem is with the
    reverse direction rule for UAX rule LB8.
*   A number of other rules could be expressed more easily if there were more
    fine grained control over rule chaining. It is currently either on or off
    for a complete set of rules.
*   Some of the existing rule syntax is extremely error prone.
*   Plain old bugs.

ICU Tickets

*   2783, #comments in rules fail with multi-line sets. May not make sense, in
    which case return the bug.
*   3058, Empty unicode set should not be an error. It turns out that there are
    uses for this. The contents of the set may come from a $Variable defined
    elsewhere, and, depending on options or whatever, a set may be empty.
*   #3640, \\p{unicode property} syntax is not recognized in rules, only in
    sets.
*   #3769, make rule chaining optional per rule set. (This will be subsumed by
    #4441)
*   #4441, Rule Chaining Enhancements
    *   Replache !!LBCMNoChain with something more general.
    *   !!LookAheadHardBreak, remove this as an option, make it default.
        (Look-ahead breaks without this option are never used, behavior is not
        well defined and completely untested. They exist in a half-way attempt
        to maintain compatibility with the original Rich Gilliam engine.
*   #????, Look-ahead breaks, allow more than one to be in-flight at once.
    Needed for the UAX14 fixes. Requires changes to engine and to state tables.
    Probably a vector of length = number of states, vec\[state\] = input
    position when at a state corresponding to a '/', and side table for
    accepting states that complete a look-ahead, indicating which vector
    position(s) (states) have the break position.
*   #4444, Bugs with look-ahead breaks. Already fixed? Invesitigate.
*   #5451, 64 bit text indexes. UText does them.
*   *Many More.*
