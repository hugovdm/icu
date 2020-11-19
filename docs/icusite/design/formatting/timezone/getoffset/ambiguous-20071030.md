# Discussion 20071030: getOffset for ambiguous time range

**getOffset for ambiguous time range**
3 messages

---

**yoshito_umaoka@us.ibm.com <yoshito_umaoka@us.ibm.com>Tue, Oct 30, 2007 at 7:16
AM**

To: Markus Scherer <markus.icu@gmail.com>

Cc: srloomis@us.ibm.com, emmo@us.ibm.com, Mark Davis
<mark.davis@icu-project.org>

Hi Markus,
In ICU4J, I added an API - getOffsetFromLocale in BasicTimeZone (I'm still
thinkin where the method resides - may move it up to TimeZone) in my own
branch.
<http://bugs.icu-project.org/trac/changeset/22807/icu4j/branches/yoshito/dfmtparse/src/com/ibm/icu/util/BasicTimeZone.java>
I initially thought that we just need one flag to control the behavior -
whether earlier iteration or later iteration. Then I found a very odd
example - Daylight -> Standard, but positive offset change (because base
offset is changed at the same time). For DateFormat parsing, we may
already know the local wall time is STD or DST. In this case, we want to
use one of them even DST -> STD is a positive offset change. To make the
API to cover all the cases, I had to implement two levels of flags - DST
or STD / former or latter.
When LOCAL_STD is set and the transition is either STD->DST or DST->STD,
it always interpret time as standard time, no matter
LOCAL_FORMER/LOCAL_LATTER is set.
When LOCAL_FORMER is set and the transition is STD->STD or DST->DST, then
it always interpret time as former one.
I'm not sure the way of specifying these options are OK for public API..
-Yoshito

---

**Markus Scherer <markus.icu@gmail.com>Wed, Nov 14, 2007 at 10:41 AM**

To: yoshito_umaoka@us.ibm.com

Cc: srloomis@us.ibm.com, emmo@us.ibm.com, Mark Davis
<mark.davis@icu-project.org>

Hi Yoshito,
Sorry for the very late reply!
This sounds like it might be simpler to have an "enum" (simple "final
int"s in Java) with 4 values rather than two boolean or dual-choice
parameters.
markus

\[Quoted text hidden\]

---

**yoshito_umaoka@us.ibm.com <yoshito_umaoka@us.ibm.com>Wed, Nov 28, 2007 at 2:56
PM**

To: Markus Scherer <markus.icu@gmail.com>

Cc: emmo@us.ibm.com, Mark Davis <mark.davis@icu-project.org>,
srloomis@us.ibm.com

Hi Markus,
I got your point and I agree that we may want to simplify the options.
For this time, I already implemented like that way and had no time to
improve the options, I will go for the current implementation.
The API is marked as @internal because we cannot add any public APIs in
maintenance releases. However, after once 3.8.1 is shipped, I will
revisit this and clean up the options. For 4.0, I will post the updated
API proposal in the design list and make the API public (@draft 4.0).
-Yoshito
