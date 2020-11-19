# Chat about edge cases 20070719

**Yoshito Umaoka <y.umaoka@gmail.com>Thu, Jul 19, 2007 at 3:26 PM**

To: markus.icu@gmail.com

3:11 PM Yoshito: Markus?

3:13 PM me: hi

Yoshito: Not sure I'm understanding what you are talking about - getOffset()

My point is - time overlap could happen standard -> standard

3:14 PM or

dayligh -> daylight

me: hm, i had *thought* that we were talking about the same thing...

3:15 PM Yoshito: ok.

so..

me: let's assume DST ends on november 5. so on nov05 until 02:00 we have DST,
but then we "fall back" to 01:00 std time.

3:16 PM Yoshito: I understand the case.

I'm talking about -

standard offset change -

me: now with getOffset(UDate=nov05 01:30, local) i want to specify whether it's
the earlier (DST) or the later (std) point in time

Yoshito: both before/after are standard time.

3:17 PM me: yes, before nov05 01:00 it's DST, and after nov05 02:00 it's std
time

Yoshito: this is rare case.. but I think there are some examples.

me: unambiguously

Yoshito: no no... I meant

3:18 PM for example, a country decided to change standard time from UTC+4:00 to
UTC+5:00

no DST is involved

but - overlap happens.

3:19 PM oops.. my example is wrong.. I meant UTC+5:00 to UTC+4:00

me: ah

3:20 PM or like the counties in Indiana which are changing between Central and
Eastern time several times in a row

you are right, there could be ambiguities there, too

Yoshito: so.. what you want to specify is either time before transition is used
or time after the transition is used when ambigious case.

1st or 2nd iteration

3:21 PM usually DST -> STD

but, STD -> STD

DST -> DST

me: now i get it. same parameter, but with expanded semantics

and named to express those

Yoshito: ok.. now we are on the smae page :)

3:23 PM Anyway, the idea is great - we need a way to allow user to controll the
behavior of ambigious local time.

me: would you like to reply again and clarify, or would you like me to explain
to the list?

Yoshito: At least for ICU4J, I believe the API is public and stable.. so need a
new API.

3:24 PM I will send my note for clarifying this.

me: absolutely. in icu4c as well, getOffset() is @stable ICU 2.8

i wasn't intending to propose to *change* an existing function

3:26 PM it could be a new overload, or more explicitly
getOffsetFromLocal(UDate date, UBool earlierTimeIfOverlap, ...)

Yoshito: yeah, something like that.

3:27 PM me: thanks for being persistent in clarifying! :-)

Yoshito: I think we do not need to worry about more than 2 iteration..

I believe it have never happened in actual time zones.

3:28 PM me: right. that would require a time zone changing offsets more than
once a day.

more than once within an hour, actually
