# Backwards era-0 years and add,roll

(Peter Edberg, 2012-05-23; updated 2012-06-07, 2012-06-13)

This is for [#9226](http://bugs.icu-project.org/trac/ticket/9226), Calendar::add
year direction for era 0 depends on calendar
(and note, all behavior described here is the same in ICU4C and ICU4J)

There are three ICU calendars in which era-0 years go backwards (era-0 year
values increase as one goes backwards in time); the current era is 1 for all of
these:

*   gregorian
*   roc (based on gregorian)
*   coptic

This is not generally documented (although some references to gregorian behavior
indicate this), and it not necessarily expected for all of these cases, since in
other similar calendars era-0 years go forward:

*   japanese (based on gregorian), current era is 235
*   ethiopic (very similar to coptic), current era is 1
*   chinese lunar, current era is 78

Of course, in calendars with only one era, for which the current era is 0, years
go forward in time, for example buddhist (based on gregorian), hebrew, islamic.

> *Note, as a separate issue, I think we need APIs that provide meta information
> about calendars, such as whether the era-0 years go backwards or forwards.
> That will be the subject of a separate bug...*

Calendars in which era-0 years go backwards should never have a negative value
in the year field. Calendars in which era-0 years go forwards use negative (or
0) values in the year field to represent years before the beginning of era-1 (as
explicitly noted for the hebrew calendar in the Java class documentation for
Calendar).

For calendars in which era-0 years go backwards, the add/roll methods for years
in era-0 (and the ucal_add/ucal_roll functions) have behavior that can seem
strange. And in addition, before we even address that:

*   Calling roll/ucal_roll for year can modify the era, contrary to the
    documented behavior of not modifying fields more significant than the one it
    is called to modify.   For example, set a gregorian calendar to era 1 year
    4, then call roll(YEAR, -5); the result is era 0 year 2 (i.e. 2 BCE).

But now on to the era-0 issues for gregorian, roc, coptic:

*   With the calendar set to era-0, calling add or roll for YEAR with a positive
    value does increase the value of the YEAR field, but that moves backwards in
    time.         This is generally consistent with documentation for add and
    roll that says, e.g. -
    *   “Add a specified signed amount to a particular field” (ucal_add,
        ucal_roll)
    *   “Adds the specified (signed) amount of time to the given time field,
        based on the calendar's rules” (Calendar::add)
    *   “add(f, delta) adds delta to field f” (ICU4J Calendar class
        documentation of rules for field manipulation methods)
*   However, it explicitly contradicts the documented behavior for ICU4C
    Calendar::roll -
    *   “Rolling by a positive value always means rolling forward in time; e.g.,
        rolling the year by +1 on '100 BC' will result in '99 BC', for Gregorian
        calendar”
*   Clients are also likely to assume that adding a positive year value moves
    forward in time (as with all other calendar fields besides year). In fact
    this assumption is the cause of the following bug in getFieldDifference -
    *   [#9227](http://bugs.icu-project.org/trac/ticket/9227) Per
        Calendar::fieldDifference, 1911 to 1913 is 5879430 yrs (for roc)

*   This behavior can also cause infinite loops with a series of add functions.
    Set a gregorian calendar to era 1 year 2, then make the following series of
    calls:
    *   add(YEAR, -3); the result is era 0 year 2 (2 CE)
    *   add(YEAR, -3); the result is era 1 year 2 (2 BCE)
    *   add(YEAR, -3); the result is era 0 year 2 (2 CE)
    *   ...

For comparison, here is the JDK (1.6.0_31) behavior for Gregorian:

*   If the calendar is set to era 1 year 4, then:
    *   add(year, -7) does cross into era 0 as expected                 (and as
        in ICU), resulting in era 0, year 4.
    *   roll(year, -7) actually wraps around within era 1               to the
        just under the maximum value for year, resulting in era 1, year
        292278991. This is actually consistent with the          documentation,
        and with how roll behaves for other fields.
*   If the calendar is set to era 0, then add of a positive         value moves
    forward in time (unlike current ICU behavior), crossing to era 1 if
    necessary, and add of a negative value moves backward   in time. However, in
    era 0, the behavior of roll is inconsistent and strange; roll with a
    positive value moves backwards in time, and roll      across the era
    boundary changes eras, with strange results. If the calendar is set to era 0
    year 4 month January, then:
    *   add(year, 7) results in era 1, year 4
    *   roll(year, 7) results in era 0, year 11
    *   roll(year, -7) results in era 1, year 292269057, month July
*   For era 0 the behavior of roll just seems erroneous and should not influence
    ICU behavior.

Proposal: add or roll for YEAR with a positive value should always move forward
in time (as already explicitly documented for ICU Calendar::roll, and as add
already behaves in the JDK), and with a negative value should always move
backward in time (except when roll results in a wrap). Improve the
documentation, e.g. to explain that the delta is not always just added to the
value of the calendar field.

Additionally, when roll is called with a year value that would cross era
boundaries: Wrap the year if the era has definite bounds at both ends, otherwise
pin the year. This will change the ICU behavior.
