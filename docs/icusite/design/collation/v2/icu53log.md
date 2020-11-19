# ICU 53 collation v2 work log

The "collation v2" reimplementation went into ICU 53 (both C++ & Java). The
following is a raw copy of the relevant parts of a "Collation v2 TODO" doc with
tasks, questions, notes, and decisions.

After ICU 53, the collation code is again maintained as usual, with further
incremental enhancements and bug fixes.

---

UTS #10=UCA spec: Read the latest draft at
<http://www.unicode.org/draft/reports/tr10/tr10.html>

UTS #10 bugs: Submit report at <http://www.unicode.org/reporting.html>

UTS #35=LDML collation: Read the latest draft at
<http://unicode.org/repos/cldr/trunk/specs/ldml/tr35-collation.html>

[ICU collation
tickets](http://bugs.icu-project.org/trac/query?status=accepted&status=design&status=new&component=collation&group=type&max=300&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&col=weeks&col=keywords&order=id)

    [ICU tickets with keyword
    “fixed-collv2”](http://bugs.icu-project.org/trac/query?keywords=~fixed-collv2&group=type&col=id&col=summary&col=status&col=owner&col=priority&col=milestone&order=priority)

[CLDR collation
tickets](http://unicode.org/cldr/trac/query?status=accepted&status=assigned&status=new&status=reopened&component=data-collation&component=uca&or&status=accepted&status=assigned&status=new&status=reopened&keywords=~collation&group=priority&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&order=priority)

“Collation subcommittee”: Mark Davis, Yoshito Umaoka, Peter Edberg, Vladimir
Weinstein

mark, yoshito, pedberg, weiv

## Code

### icu/branches/markus/collv2

    This branch is closed! No more commits here!

    <http://bugs.icu-project.org/trac/build/testing-icu4c-win>

    <http://bugs.icu-project.org/trac/build/testing-icu4c>

    [change
    log](http://bugs.icu-project.org/trac/log/icu/branches/markus/collv2)

    created as copy of icu/trunk@31337

    became initial branch revision 31342

    last branch change revision: 35221 (plus a couple of revisions until 35225
    with mergeinfo changes)

    [diff from last-merged trunk revision to collv2
    HEAD](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2&old=34462%40icu%2Ftrunk)

        [diff only
        source/common](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/common&old=34462%40icu%2Ftrunk/source/common)

        [diff only
        source/i18n](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/i18n&old=34462%40icu%2Ftrunk/source/i18n)

        [diff only
        source/i18n/unicode](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/i18n/unicode&old=34462%40icu%2Ftrunk/source/i18n/unicode)
        (public header files)

        [diff only
        source/test/intltest](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/test/intltest&old=34462%40icu%2Ftrunk/source/test/intltest)

        [diff only
        source/test/cintltst](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/test/cintltst&old=34462%40icu%2Ftrunk/source/test/cintltst)

        new
        [source/test/testdata/collationtest.txt](http://bugs.icu-project.org/trac/browser/icu/branches/markus/collv2/source/test/testdata/collationtest.txt)

        [diff only
        source/tools](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/tools&old=34462%40icu%2Ftrunk/source/tools)

    last merge:

        svn merge -r 33424:34462 ^/icu/trunk (ICU 52rc)

        → r34474

    previous merge:

        svn merge -r 32800:33424 ^/icu/trunk (nearly ICU 51)

        <http://bugs.icu-project.org/trac/changeset/33436>

    merge to trunk:

        trunk: svn update to revision 35221.

        Normal merge did not work because the branch did not seem to have a
        clean history of merges from the trunk.

        Problem: Some previous merge seems to have gone wrong. svn thinks that
        only [trunk revisions
        31338-32216,32511-34462](http://bugs.icu-project.org/trac/log/icu/trunk?revs=31338-32216%2C32511-34462)
        have been merged to the branch. It thinks that [trunk revisions
        32224-32498](http://bugs.icu-project.org/trac/log/icu/trunk?revs=32224%2C32236%2C32242%2C32247%2C32252-32254%2C32259%2C32262-32263%2C32268%2C32271%2C32275%2C32279-32282%2C32284-32285%2C32289-32290%2C32293%2C32298-32300%2C32305%2C32307%2C32313%2C32315%2C32330-32331%2C32334%2C32338-32340%2C32348%2C32358%2C32363-32365%2C32367-32368%2C32372%2C32376%2C32378%2C32388%2C32393%2C32397-32400%2C32405-32406%2C32408-32410%2C32412%2C32416%2C32424%2C32427%2C32429-32431%2C32435-32436%2C32439-32441%2C32445%2C32447%2C32449%2C32459%2C32461%2C32468%2C32470%2C32474%2C32483%2C32489%2C32491-32492%2C32494-32495%2C32497-32498)
        are still “eligible”.

        Checked out trunk@34462, diff to branch looks good, no unexpected
        changes except some $Date$ and $Revision$ in data .xml files.

        Fix the branch:

            svn merge ^/icu/trunk . -r 31338:34462 --record-only

            –> r35223

            (undo –> r35224, confirm merge does not work, redo –> r35225)

        svn merge --reintegrate --accept p ^/icu/branches/markus/collv2

            Shows expected changes and conflicts, with a small number of
            unrelated to revert.

        use new source/common/sharedobject.h, use its clearPtr()

        svn commit -m 'ticket:9101: merge branches/markus/collv2@35225 into the
        trunk'

        <http://bugs.icu-project.org/trac/changeset/35227>

### tools/trunk/unicode/c/genuca

    [diff
    tools/trunk/unicode/c/genuca](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40tools%2Ftrunk%2Funicode%2Fc%2Fgenuca&old=33557%40tools%2Ftrunk%2Funicode%2Fc%2Fgenuca)

    Was temporarily developed in
    [icu/branches/markus/collv2/source/tools/genuca2](http://bugs.icu-project.org/trac/log/icu/branches/markus/collv2/source/tools/genuca2?rev=35220)
    until copied to the tools tree in changeset 35220 and deleted from the
    collv2 branch in changeset 35221.

### icu4j/branches/markus/collv2

    This branch is closed! No more commits here!

    [change
    log](http://bugs.icu-project.org/trac/log/icu4j/branches/markus/collv2)

    created as copy of icu4j/trunk@34922

    became initial branch revision 34930

    last branch change revision: 35302

    [diff from last-merged trunk revision to collv2
    HEAD](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu4j%2Fbranches%2Fmarkus%2Fcollv2&old=34930%40icu4j%2Fbranches%2Fmarkus%2Fcollv2)

        new
        [main/classes/collate/src/com/ibm/icu/impl/coll](http://bugs.icu-project.org/trac/browser/icu4j/branches/markus/collv2/main/classes/collate/src/com/ibm/icu/impl/coll)

            BOCSU.java is not new, moved here from core BOCU.java

        [diff only
        main/classes/collate/src/com/ibm/icu/text](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu4j%2Fbranches%2Fmarkus%2Fcollv2%2Fmain%2Fclasses%2Fcollate%2Fsrc%2Fcom%2Fibm%2Ficu%2Ftext&old=34930%40icu4j%2Fbranches%2Fmarkus%2Fcollv2%2Fmain%2Fclasses%2Fcollate%2Fsrc%2Fcom%2Fibm%2Ficu%2Ftext)

        [diff only
        main/tests/collate/src/com/ibm/icu/dev/test](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu4j%2Fbranches%2Fmarkus%2Fcollv2%2Fmain%2Ftests/collate/src/com/ibm/icu/dev/test&old=34930%40icu4j%2Fbranches%2Fmarkus%2Fcollv2%2Fmain%2Ftests/collate/src/com/ibm/icu/dev/test)

        new collationtest.txt & updated Collation_\*.txt files in
        [main/tests/collate/src/com/ibm/icu/dev/data/](http://bugs.icu-project.org/trac/browser/icu4j/branches/markus/collv2/main/tests/collate/src/com/ibm/icu/dev/data/)

    merge to trunk:

        trunk: svn switch to revision 35304

        svn merge --reintegrate --accept p ^/icu4j/branches/markus/collv2

        revert icudata.jar, update its collation files from ICU4C trunk

        svn commit -m 'ticket:9101: merge branches/markus/collv2@35302 into the
        trunk'

        <http://bugs.icu-project.org/trac/changeset/35305>

## Benchmarks

Yoshito

Q: Is v2 fast enough?

Q: Do we need a compare() Latin-1 fastpath like in v1?

    Done 2013aug: Apparently needed; Yoshito’s initial benchmarking suggested
    60% slower Latin-1 without the fastpath. Added new fastpath with small
    memory-mapped data, for both UTF-16 and UTF-8, and supporting more
    characters and more collator attributes.

    If so, for both UTF-16 and UTF-8?

        Yoshito: UTF-8 is higher priority in IBM

    Performance vs. code bloat & complexity & heap memory (over 3kB per Collator
    instance) & slower first use and setAttribute()

        Might be able to reduce the size and store it with the binary data and
        not modify it in setAttribute(), might extend it to Latin-A for all of
        French and some more languages, see details later in this doc

Q: Potential for other performance improvements in the all-Unicode compare()
functions??

    Maybe even enough to not need a Latin-1 fastpath?

    See details later in this doc

### Compare v1 vs. v2 performance

<http://icu-project.org/trac/ticket/10549> “Collation performance tests for
ICU4C”

Q: Take code from existing collperf test?

User operations to test:

    sort list of strings

    search for a string in a sorted list

        Yoshito: My understanding is that the scenario used in the existing test
        case is not matching our main customer’s interest. Especially, sorting
        performance test with qsort is far from the customer’s usage scenario. -
        They use a variation of radix sort based on incremental access of
        collation key value - therefore, their implementation always produce
        necessary length of collation key for each, not depending on
        ucol_strcoll. It can still measure ucol_strcoll’s performance, but with
        unnecessary noise (qsort or binary search algorithm part).

Collator API to use:

    compare(UTF-16 UnicodeString (with length))

    compare(UTF-8 with length)

    Q: Do we need to benchmark nextSortkeyPart(UTF-8 UCharIterator) with a
    [radix sort](http://en.wikipedia.org/wiki/Radix_sort)?

        Yoshito: I don’t think we need radix sort implementation. We just need
        to understand how nextSortKeyPart is invoked and measure “pure”
        peformance of the function.

Languages: (some may have the same collation order, but the test strings should
mostly be in the tested language, giving a different mix of ASCII vs. other
Latin etc.)

    en · de · ru · th · ja · zh-u-co-pinyin

Collator settings to use (several settings, to get a better idea for common and
desirable use cases, and to help justify the reimplementation):

    language defaults

    ignore case: strength=2

    ignore accents: strength=1 & caseLevel=on

    ignore punctuation: strength=4 & alternate=shifted & normalization=on

        normalization guarantees correct results, as in JavaScript i18n, and
        “ignore punctuation” is a desirable use case

Proposed combinations, rather than the cross-product of language x settings.
(Still should test both UTF-8 & UTF-16.)

    locale=en, default settings; English strings

    locale=de@collation=phonebook, ignore case: strength=2; German strings

    locale=fr, ignore accents: strength=1 & caseLevel=on; French strings

    locale=ru, ignore punctuation: strength=4 & alternate=shifted; Russian
    strings

    locale=th, normalization=on & alternate=shifted; Thai strings

    locale=th, normalization=on & alternate=shifted; English strings

    locale=ja, strength=5 (full JIS X 4061); Japanese strings

    locale=ja, strength=5 (full JIS X 4061); English strings

    locale=zh@collation=pinyin; Chinese strings

    locale=zh@collation=pinyin; English strings

Markus 2013nov08: I did implement a new Latin fastpath \[...\]. It covers a
larger set of Latin characters than before, so it should bail out to the normal
path less often. We could include an eastern European language like Polish to
see if there is a noticeable difference.

July 23, 2013 update: Yoshito ran the existing collation performance regression
test suites with a simple hack - setting an instance of root collator created by
CollationRoot::createCollator(status) to Collator\* UCollator.delegate.
UCollator.delegate is designed for collation plug-in, but it would be sufficient
enough to see the performance trend with the new code. Many test cases are
within +/- 5% range, except following two cases:

    Key Gen (ucol_getSortKey performance) dropped about 20%-40%

    Binary Search (ucol_strcoll) dropped about 60% for Latin data (equal or
    better for other scripts)

The latter issue found in Latin is likely caused by Latin 1 optimization and I
think this is already in plan. However, key generation performance drop might
need to be investigated.

## Test

Yoshito & Markus

Obsolete 2013oct: So far, v2 code is tested via

    intltest collate/UCAConformanceTest (currently tests only v2, with UCA
    6.3/CLDR 24 root data; the v1 code still uses UCA 6.2 which would fail with
    the new conformance test files)

    intltest collate/CollationTest, v2-specific and mostly data-driven

Done 2013oct: Eventually, all existing tests will use the new code.

Obsolete 2013oct (replaced v1 code with v2 code, fixing tests) – Q: How, and how
early, can/should we use v2 code in more of the tests?

    Idea: When the Collator runtime code is complete, return a v2 Collator
    instance whenever the test code does not build or load a rule-tailored
    collator (root, de-u-co-standard, etc. -> getTailoredSet()->isEmpty()).

    Probably best to do this under an #ifdef because there will be some
    failures.

    Probably need to make some test code changes to cope with implementation
    differences. For example, casting to RuleBasedCollator will fail (or produce
    a bad pointer) as long as v2 is implemented as RBC2. Use more
    virtual/internal Collator base class methods, maybe use dynamic_cast and
    check for NULL. Maybe move some tests from C to C++ to make such changes
    workable.

    Idea: Return new Collator when building from rules.

Add tests (data-driven or otherwise) to confirm v1 bugs and to verify that they
are fixed in v2.

    See [this query of ICU collation
    tickets](http://bugs.icu-project.org/trac/query?status=accepted&status=design&status=new&status=reviewing&component=collation&group=type&max=300&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&col=weeks&order=priority).

    Done 2013jun: Use the Trac ticket keyword “fixed-collv2” to indicate that
    the collv2 branch has a test for the bug (and it passes).

Q: There are many bug tickets for v1. What is a good way to verify that v1
indeed fails them and v2 passes?

    Idea: Does it make sense to run the data-driven CollationTest on v1 collator
    instances as well? Print warnings for failures with v1??

    Or else write specific test code to let v1 fail and later verify that v2
    passes as we switch the implementation? It would be nice to be able to use
    the data-driven test framework.

    Maybe some temporary syntax in the data-driven test data file that indicates
    using a v1 Collator??

    Idea: Write test code using the regular public API, use #ifdef from previous
    “Q” to switch between v1 & v2. Affected tests should fail with v1 and pass
    with v2.

    Better idea: Set environment variable (getenv()) COLLTESTV1=1 to switch the
    data-driven tests to the existing public (v1) Collator API (as much as
    possible). Add test cases, with the v1-bug-ticket numbers in comments. Test
    that these cases fail with v1 & work with v2.

        Note: Some test code cannot work with v1; for example, v1
        nextSortKeyPart() will return different sort keys than getSortKey(), by
        design.

        FYI: When we eventually merge the collv2 work into the trunk, the code
        for testing the existing Collator API will remain, and the one using the
        v2-specific class names will be removed.

    Simpler: Verify bugs with the online collation demo, add tests in v2
    data-driven test code.

Obsolete 2013oct: Note: collate/CollationTest will eventually need to use the
public API anyway. An #ifdef could help do that early.

Obsolete 2013oct: Idea: Temporarily, as long as v2 is missing some
functionality, a v2 instance could also have a pointer to a v1 version. For
missing functionality it would call the v1 function.

Done 2013nov20: r34675 “delete low-ROI tests that depended on the old rule
parser and other deleted internals: cmsccoll/TestCollations,
cmsccoll/RamsRulesTest, cmsccoll/TestRedundantRules (collationtest.txt),
cmsccoll/TestExpansionSyntax (collationtest.txt), much of
cmsccoll/TestVariableTopSetting (replaced by spot checks), citertst/TestCEs” see
“[collv2 test
review](https://docs.google.com/document/d/1ddMIfnm0_5JFdMqmuk21Xm0sdK2pj6kB1_INoUvqd64/edit#)”

try to simplify collate/CollationTest

    Postponed: try to share code with existing framework; for example,
    prettify(CollationKey)

    Done 2012nov26: try to work with a root collator rather than building a new
    base

    Done 2012nov26: try to replace rawbase tests with raw tailorings or even
    rules

    Postponed: move base data builder to toolutil?

    Postponed: consider: DataDrivenCollatorTest also did this:

        tested offsets (via IntlTestCollator?)

        on each new Collator, did cloneBinary() and RBC(binary) and used both
        the original and the clone for testing

Done 2012nov23: test cloneBinary(): after construct(bin) cloneBinary() again,
check same binary, construct(bin2), check equal collator (equal also to
original), usable without crash

Done 2012nov22: collationtest.txt: reorder Zzzz Grek –> Han<Omega<FFFD<FFFF

Done 2012nov26: CollationTest: test string comparisons with NFD versions of
strings, too

Done 2013dec20: valgrind and/or memory sanitizer

## Collator runtime

Markus

Done 2012nov15: Finish compare(UTF-8)

Done 2013mar01: Implement getTailoredSet, getContractions() etc.

    Iterate data structure, not re-parse rules

    Done 2012nov16: Read all related API docs

    Done 2012nov16: Q: Results for root/no tailoring rules? Fallbacks to root?
    Prefixes as contractions?!

        2012nov16: experimented with v1, sent email to icu-core “Collator
        questions: contractions & expansions”

        Avoid v1 bugs: <http://bugs.icu-project.org/trac/ticket/9755>

Done: attributes: variableTop and/or maxVariable

    Done 2012nov16: sent email to Peter “ICU collation: variableTop,
    maxVariable, modifier letters” asking about usage & concerns

    Done 2012nov29: Peter replied that he has no concerns about replacing
    variableTop with maxVariable and pinning the variableTop values.

Done 2013sep24: internalGetShortDefinitionString()

Done 2013jul11: getMaxExpansion()

    Done 2012nov16: sent email to icu-core “can we deprecate
    Collator::getMaxExpansion()?”

    Done 2012nov28: <http://bugs.icu-project.org/trac/ticket/9758> deprecate
    RuleBasedCollator::getMaxExpansion()

    Done 2013jul11: Implement in RBC, with mutex & lazy init for the data.
    Probably simplest to use a UHashtable. Pass that UHashtable into the
    CollationElementIterator constructor so that its getMaxExpansion() need not
    synchronize.

Done 2013feb: clone()

Obsolete 2013jul22: Normalization=on (paper note 20130212)

    Decompose U+0344 as well;
    <http://bugs.icu-project.org/trac/ticket/8052#comment:7> and
    <http://unicode.org/cldr/trac/ticket/5667#comment:1>

        Maybe not; see unicode list discussion 2013apr02 “FCD and Collation”,
        and <http://unicode.org/cldr/trac/ticket/5866>

        Thoughts 2013apr12:

            If we do not add overlap contractions, then we have the
            ICU-documented limitation of decomposition mappings vs.
            contractions, for U+0344 as well as other composites (e.g., Danish
            aa vs. ä).

            If we do add overlap contractions, then FCD text sorts like NFD
            text, but we change the sorting behavior of NFD text in some cases.

            There is a theoretically unlimited number of overlap contractions
            for the Tibetan composite vowels, so we cannot “fix” them completely
            without decomposing them.

            Decision 2013jul22: Document as in
            <http://unicode.org/cldr/trac/ticket/5866> and in the email
            2013apr02 “FCD and Collation” discussion

    No need to decompose the problematic characters unless they are adjacent to
    ccc!=0

        Decision 2013jul22: Untrue. Need to decompose Tibetan composite vowels
        even if not adjacent to ccc!=0 so that contractions with one of their
        pieces work. For example, for contraction x+0F72 we need to see the
        decomposition of <x, 0F73>.

Done 2013may: Put from-rules constructor into the parser file, to reduce
dependencies

Postponed: Revisit root collator unsafeBackwardSet: Serialize whole set
(consistent with UCA version, larger data, bound to UCD version, genuca
bootstrapping would want FCD data from preparseucd.py) vs. only contractions
(consistent with CollationFCD & normalization, slow runtime enumeration of
FCD-unsafe characters, maybe fastpath in Normalizer2Impl?) (paper note 20121228)

    Done 2013dec11: optimize runtime enumeration of FCD-unsafe chars

Done 2013sep20: digits unsafe because of numeric collation?!

    Seems unnecessary because the result should still be the same after an
    identical prefix with some of the digits.

    Confirmed: However, we could get different results if there are more than
    254 digits with some in the identical prefix.

        Simpler: Test “11b” < “101a”

    If needed: omit from binary, like normalization-unsafe chars.

    Done: Consider treating them as unsafe only in code, not in the UnicodeSet,
    and only test if numeric=on.

        inline CollationData.isUnsafeBackward(UChar32 c, UBool numeric /\* spell
        like in CollationIterator \*/) const

        if(unsafeBackwardSet->contains(c)) { return TRUE; }

        if(!numeric) { return FALSE; }

        if(c < 0x660) { return 0x30 <= c && c <= 0x39; }

        return hasSpecialTag(getCE32(c), DIGIT_TAG);

Done 2013dec15: Implement “ICU API proposal: Collator variableTop ->
maxVariable”

    <http://bugs.icu-project.org/trac/ticket/8032#comment:7>

    Done 2014feb21: <http://unicode.org/cldr/trac/ticket/5016>

    callcoll/TestVariableTop

    “digit” withdrawn, only four valid values space, punct, symbol, currency
    (+default)

    allow many CEs for variableTop as long as there is exactly one primary CE

    consider changing the proposed API to adding a new UColAttribute and 4 new
    UColAttributeValue constants.

        pro:

            use setAttribute() etc.

            avoid adding another setting that does not use setAttribute()
            \[currently variableTop & reorder code\] –> setting from locale
            keywords

            if & when we want to write a BCP 47 settings string, we can easily
            use the explicitlySetAttributes

            does not even look like all UColReorderCode values can be set (but
            most of them can)

        con:

            hardly better than current proposal?

            adds 1+4=5 constants rather than 2 methods

            harder to add digits if & when we might want to

            adds to UColAttributeValue enum with few valid values per attribute,
            shares only “off”

            there is no setAttribute() in Java, would have to invent something
            different there

Postponed: add RuleBasedCollator constructor with UParseError

    propose to icu-design

    <http://bugs.icu-project.org/trac/ticket/10158>

    maybe also write error reason to nullable ByteSink or UnicodeString

    implemented 2013nov26, @internal

Done 2013sep20: operator==()

Done 2013sep22: hashCode()

Done 2013sep23: getVersion()

Done 2013sep23: getLocale()

Done 2013sep23: getRules()

Done 2013sep22: cloneRuleData()

Done 2013sep22: cloneBinary()

Done 2013sep25: merge ^/icu/trunk into branch

    done r34474: merge, and resolve conflicts

    done r34484: make it work with new UInitOnce

    done r34486: update for UCD 6.3 & UCA 6.3 (new FractionalUCA.txt?)

    done r34485: adjust RBC for tblcoll.h

Done 2013oct30: propose const cloneBinary() etc., ticket 10428, done on collv2

    sent proposal 2013oct07, approved:2013-10-30

Done 2013oct07: propose that the loader does not auto-build from rules any more
when the binary is missing or has the wrong UCA version

    old code set U_DIFFERENT_UCA_VERSION when appropriate (weakened from
    U_COLLATOR_VERSION_MISMATCH), with fallback to building from rules

    dependencies!

    someone trying to reduce data file sizes should omit the rule strings, not
    the binaries

    sent to icu-core on 2013oct07, no replies

    done on collv2, reminder TODOs still in ucol_res.cpp

Done 2013dec20: should the rule UnicodeString be bogus/NULL if it was omitted
from the data?

    Done 2013dec20: test in v1 (either write test program, or temporarily change
    “Sequence” in ucol_open_internal())

    v1 ucol_getRules() has special code to return “” instead of NULL

    A bogus Locale returns “” as well.

    Keep returning “”, to avoid breaking callers.

    Postponed: We should document that “” might mean that the rules are not
    available.

can we delete code in #if BOYER_MOORE ? (maybe just leave it but don’t try to
make it work?)

Done 2013oct11: can we delete code in #ifdef U_USE_COLLATION_OBSOLETE_2_6 ?
Collator::createInstance(locale, version, UErrorCode)

    sent proposal 2013oct07 – <http://bugs.icu-project.org/trac/ticket/10476>

Done 2013oct11: can we delete various ucol.h @internal API?

    ucol_equals() – moved to internal header, to avoid more work rewriting test
    code

    ucol_getUnsafeSet() – keep

    ucol_forgetUCA() – deleted

    ucol_prepareShortStringOpen() – keep

    sent proposal 2013oct07 – <http://bugs.icu-project.org/trac/ticket/10476>

Done 2013oct11: other tblcoll.h internals

    setUCollator(locale): ignore; construct Tailoring outside, then construct
    RBC with that

    getUCollator(): return cast(this)

    virtual void setLocales(const Locale& requestedLocale, const Locale&
    validLocale, const Locale& actualLocale)

        and ucol_setReqValidLocales()

        It seems like the Collator (or its tailoring) should have an immutable
        actualLocale.

        At most we should want to set the validLocale.

        Asked 20130926 if we can drop support for the ULOC_REQUESTED_LOCALE

            ures_getLocaleByType() always returns NULL, others set error

            propose to set error; alternative maybe return valid locale?

            Peter says ok to returning valid locale in ICU 53

            For later: <http://bugs.icu-project.org/trac/ticket/10477>

Done 2013oct11: move the validLocale from the tailoring to rbc; add
requestedLocale??

Done 2013oct20: rethink collation fast latin builder vs. runtime vs. u-umlaut
contraction vs. finding difference at ‘u’ and returning rather than bailing out
for the contraction (easy: just write an empty contraction list so that the
runtime inspects the next character)

Done 2013nov08: clean up ucaconf.cpp test, try to resolve its pre-existing TODOs
about v1 bugs

Done 2013dec19: (do this last) clean up warnings in test code, especially unused
functions

Done 2013nov22: swap the new data structure (needed for Java port)

    maybe move the swapper to toolutils?

Done 2013dec12: addContractions(cp, set) @internal (paper note 20130305)

    Adds contractions that start with cp to the set.

    Ignores prefixes. Returns the number of items added.

Postponed:

    Check tailorings for whether they set canonically equivalent strings to the
    same CEs. Japanese?

    Maybe build this into the Builder, maybe with callbacks for informational
    output. Active in genrb, ignored at runtime.

    Check tailorings for whether they make differences that are already the
    case, e.g., &c<<<C.

Done 2013sep26: increment UCOL_RUNTIME_VERSION and UCOL_BUILDER_VERSION

Done 2013dec20: check that all new code is inside #if !UCONFIG_NO_COLLATION

Done 2013dec20: adjust file comments saying back to C++ and replacing what old
files

Done 2013nov09: remove any register-cleanup enum constants? UCLN_I18N_UCOL_RES?
UCLN_I18N_UCOL_BLD?

Postponed: propose as public API the C<->C++ converters toUCollator(),
fromUCollator(), toUCollationElements(), ...

Done 2013dec19: review tools/dumpce

    Deleted 2013dec19.

    Sent email to icu-core 20131011 asking whether we can remove it; Vladimir &
    Peter say yes.

    No one seems to be using it.

    It was not built via the tools/Makefile, nor via the MSVC allinone.sln. It
    had .dsp and .dsw files from a very old Visual Studio version.

    Would need work:

        Several of its core functions use the old implementation's internal rule
        parser, which is gone.

        They would take some thought and work to replace; it would be better to
        work with getTailoredSet() and such. (In particular, one of the more
        useful genrb options removes the collation rule strings, and a tool like
        this should be able to work with the binary data.)

        It would be better to get and print 64-bit CEs.

        The tool does not handle multiple tailorings, or collation
        keywords/default etc.

Done 2013nov26: clean up genuca2.cpp

Done 2014feb24: after port to Java (when we do not need to generate data for the
old version any more), move genuca2.cpp to tools/genuca

Done 2014feb25: after port to Java, merge into trunk; note data sizes before &
after

Yoshito?

Nothing to do (just keep as is in C API): Add UTRACE_... calls back into the
runtime code.

    Is this still used? -- 2013-sep: Yoshito says yes

    Only in C API? – yes

### Ideas for Fast Latin

    Done 2013aug30 -- also included 2000..203F, FFFE, FFFF: See v2 design doc:
    Might extend the fastpath to U+017F, extend it to quaternary weights and
    handle alternate=shifted, and reduce it to 2 bytes per level. (paper notes
    20120909 & 20130110 & 20130112)

    Done 2013aug30 -- made it 16b per char, based on some of the ideas here,
    with code complications for decoding (two types of expansions, two lengths
    of primary bit fields/two mini CE formats)

        32b per char? (about 1.6kB for up to U+017F)

        Recode primaries to 7b each, maybe bail out if first CE is pri ignorable

            26 a-z, 48 Z/P/S, few more? (there are more even in Latin-1/A)

            bail out if primary outside special groups+Latn (that is, >=Grek)

            Bail out = 1

        Maybe don’t go into fast-Latin if specials+Latn reordered relative to
        each other

        Recode secondaries to few bits (keep WF2), few per primary, then sec CEs

            First CE: completely ignorable or primary CE; Danish: up to 4 after
            common (0..4 if 0=common) → 3b?

            Second CE can be secondary CE:

                Latin-1: 88, 8A, 8E, 92, 96, 9A, 9E, A0

                Latin-A: 8C, 90, 98, 9C, A2, A4, B2, FB 4C

                5b: 0=ignorable, 1=common, 2..5 for primary CEs, 6..31 for
                secondary CEs

                Or 7b for both CEs, 5 values \* 25 values, use tiny table

        Case: 2x2b, or combined with tertiary

        Tertiary: Bail out of fast Latn if no difference after secondary? Bail
        out if ter CE; else few per pri+sec (2 or 3b)

            Maybe 3 case values & 5 ter values = 4b (case \* 3 + ter),
            0xf=uppercase with ter=5, use tiny table; Swedish: up to 4 after
            common (0..5 if 0=ignorable, 5 for uppercase AA)

            Might not need case bits for second CE (except for &T<<<Þ/H), maybe
            just 2b ter

            Second CE: Use value 0 for common; ter ignorable only if sec
            ignorable (all-zero fast-Latin value)

        Simple, no tiny tables, always use 0=ignorable:

            pri 7+7

            sec 3+5

            case 2+2

            ter 3+3

        Variable: Dynamic, bail out if two pri weights are not both var/non-var

        Might bail out for sec/ter weights below common; might bail out of only
        sec/ter level, so that we can still compare on primary level.

        Contraction (use high or low pri values): Top byte zero, then 5b a-z 5b
        A-Z or even assume uppercase of first; index to triple of
        default/lower/upper (need lists of up to 20 contraction suffix
        characters)

        Might store in binary except “search”

        Flag in data for fast Latin data same as in root, don’t duplicate it

### Ideas for regular compare()

-- without CE buffers (hopefully speeds up primary level), with fast “equals”
detection and level skipping (complicates primary level but short-circuits lower
levels)

    Count how many comparisons are decided on which level; and how many in
    identical-prefix comparison (U_DEBUG)

    Withdrawn 2013jul02 (removing CE buffers) because it simplified the code
    significantly to have the CollationIterator have and build the whole CE
    buffer. -- For example, if 90+% of string comparisons are decided on a
    primary level, then we might remove the compare() method’s CE buffers. This
    might speed up primary-level comparison but we would have to re-fetch the
    CEs, either for every level, or just once (after level 1 compared equal) and
    buffer them then.

    Some way for “equal” to be found without comparing each level separately,
    especially if we don’t buffer CEs, or at least short-circuit some of the
    lower levels? Seems like comparisons are usually primary different or equal.

    Note: The identical-prefix code finds identical strings. It might make fast
    “equal” unnecessary, or vice versa.

    Maybe compare whole CEs first, then primaries, then lower32s masked for
    settings, note if primary equal but not all equal, don’t buffer CEs.

    Idea: Compare primaries and collect lower32Diffs=or-all(xor(pairs of
    lower32)). Maybe track leftVariable (init 0, set to 1 if multiple) and
    compareVariable (true if multiple, or right variable != leftVariable).

        We could do this with or without CE buffering.

        Most useful if good percentage of comparisons are decided on lower than
        secondary level, because we could short-circuit any work for
        intermediate levels.

        If “equal” gets fast enough, then maybe we could remove the
        identical-prefix code??

        Problem: Collecting/xor’ing of lower32 bits of primary ignorable CEs.
        Maybe set lower32Diffs=0xffffffff if any of those occur??

    If primary equal, then:

        If strength>=sec && lower32Diffs>>16 != 0: Iter & compare sec, must be
        !=equal. For backwards, go forward and collect in UnicodeString so that
        we don’t iter back into identical prefix; or maybe that’s harmless.

        If caseLevel==on && lower32Diffs&case: Iter & compare case, may be equal
        depending on when we ignore case bits.

        If str>=ter && lower32Diffs&terMask (can include case): Iter & compare
        ter, must be != equal.

        If str>=quat && (lower32Diffs&quat || anyVariable/compareVariable): Iter
        & compare quat

        Usually, iterates only once (primary diff) or twice. Does not inspect
        sec weights etc. at all if known no diffs there.

        TODO: Need a good way to reset the iterators, preserving the identical
        prefix offset. (iter.resetToOriginalPos())

### Done 2013jun25: Crazy idea: Redo specials

    Use impossible case bits as specials flag: isSpecialCE32(ce32) { (ce32 &
    0xc0) == 0xc0; } // slower?! -- might be at least as fast in Java where
    unassigned comparisons are not built in

    ce32 == 0xc0 for fallback: CASE_MASK, SPECIAL_CASE, MIN_SPECIAL_CE32

    all indexes could have the same number of bits, shared setter/getter

    more bits per special (20 → 26)

        might especially help digits with currently only a 16-bit index

        might put index into 20 upper bits, then tag, then 2 specials flags,
        then 6 bits for contraction flags, expansion lengths (no more escape for
        long expansions), digit values

    more tags

        use only 1 for Latin expansion, not 6

        use a separate tag for U+0000

    primaries can use byte FF

    Done 2013jun16: more named constants, for contraction optimization flags,
    for max contr/exp/prefix index

    measure perf of this specials test vs. <=0xff000000 (UCAConformanceTest did
    not get slower, looks very slightly faster)

## CollationElementIterator

Yoshito?

Done Q: [ticket #9104](http://bugs.icu-project.org/trac/ticket/9104) “does
CollationElementIterator (need to) support switching next/previous direction on
the fly?”

    Deprecated the support for switching direction without reset() etc. in ICU4J

Done 2013jul11: Reimplement CollationElementIterator API via (internal) v2
CollationIterator; some 64-bit CEs map to two 32-bit CEs (second is
continuation)

    2012nov16: sent email to icu-core “who uses CollationElementIterator?”

    can we deprecate it as public API??

        no, it’s public even in the JDK

## String search

Yoshito

Done 2012dec19: Retract Boyer-Moore string search code and its test cases
(ticket [#9573](http://bugs.icu-project.org/trac/ticket/9573))

    TODO: Confirm that this is the only use of getMaxExpansion()!

Postponed: Consider changing string search to using the internal v2
CollationIterator directly, or maybe with some wrapper code -- working with
64-bit CEs.

    This could be done after most of the other work

    See question above about deprecating the public CollationElementIterator API

    Alternative 1: [Ideas for new, more convenient iterator API (maybe not
    public API)](http://bugs.icu-project.org/trac/ticket/2485#comment:5)

    Alternative 2: Mark has ideas for a new version of string search that does
    an initial search in character space (get him to write them down...)

## Mark’s UCA tools & FractionalUCA.txt

Markus

UCA/LDML spec changes

    Done in UCA 6.3: U+FFFD with third-highest primary weight

Done 2012nov28: Add CLDR root collation data files into CLDR/collation/uca/
([CldrBug:5386](http://unicode.org/cldr/trac/ticket/5386))

Done 2012dec: Fix bugs in Mark’s code

    Fix/new contraction matching code & data structure & builder

Done 2013jan: Improve FractionalUCA.txt

    Compressible data: Dense ranges of primary order, optimized secondaries &
    tertiaries (merge secondaries into primary CEs & distribute per-primary
    secondaries/tertiaries), etc.

    Fix bugs: Add tailoring gaps for secondary & tertiary weights

    Complete data in the .txt file, avoid circular dependency with new UCD: Use
    case bits from UCA decomps.txt, list Unified_Ideograph, simpler script order
    data, etc.

Postponed: Make UCARules.txt work with builder that forbids tailoring
primary-after ignorables

    UCAConformanceTest::TestRulesNonIgnorable

    Try to reset to special reordering groups as appropriate, then after \[last
    regular\]

    <http://unicode.org/cldr/trac/ticket/6745>

    2013nov13: Sent email to “subcommittee” with subject “UCA_Rules.txt: remove?
    or fix how far?”

        Client seems to be using the UCA_Rules to find equivalences, not order.
        Probably does not care about passing the UCAConformanceTest.

Postponed: If we only cared about compare() speed, not sort key lengths, then we
would not have any 1-byte primary weights. We would free up dozens of lead
bytes. (paper note 20131110)

    We could just have a larger gap after each a-z for 2-byte tailored
    primaries.

    Consider changing Nd (0-9) to two-byte primary weights. That would free up
    about 18 lead bytes, for about 4500 2-byte primaries, for Han tailorings
    and/or for Unicode 7.0 additions.

    Groups below digits should not be compressible, even if they have a single
    lead byte. Verify that the tools code does that (currency symbols)!

        Digits will always have at least three lead bytes: first-primary,
        numeric-primary, possibly shared lead byte for all numbers.

## CLDR data

Markus (& Yoshito?)

Done 2012nov28: Remove und-co-ducet tailoring
([CldrBug:5464](http://unicode.org/cldr/trac/ticket/5464))

    MD: I think we want to retract that tailoring instead of spending time on
    it.

    cldr-dev email 2012nov14 “remove CLDR "ducet" tailoring?”

    (If we remove it, then we should probably change the LDML spec’s keyword
    fallback example to use a different keyword value.)

Obsolete: Or else fix und-co-ducet tailoring
([CldrBug:5208](http://unicode.org/cldr/trac/ticket/5208)), run through
UCAConformanceTest with DUCET test files

Obsolete because we removed und-co-ducet: Move modifier letters into currency
group

    MD: rather “handle modifier letters”. Having them shift with currencies
    would be unexpected.

    Q: Do we even care if the “ducet” tailoring is removed?

    This could be done later

## Collation builder / rule parser

Markus & Yoshito?

Done 2013apr01: Improve weight allocation: Use more byte values (allow 02 in
some weight bytes), different limits by level & by weight byte

    Consider modifying v1 weight allocator to use more byte values (so that it
    can work with an updated FractionalUCA.txt) and turning off sort key
    compression to minimize v1 code changes

Done 2013mar: Implement v2 “inverse UCA” to map from reset anchor to
next/previous CE for tailoring

Biggest change: From 32-bit CEs with continuation CEs to 64-bit CEs (no
continuations, less complexity for v2-only)

Assumption: Incremental development & testing & benchmarking require that we
keep v1 code passing all (or most) of its tests until v2 code is ready to fully
replace v1.

Q: Plan A or Plan B?

    Decision: Plan B: Reimplement because the builder code is also buggy, messy,
    hard to maintain

        Worse: Turns out v1 approach of building character/string-based
        tailoring graph caused problems. Replaced for v2 by building CE-based
        tailoring graph.

    Neither is trivial; new code vs. keeping it alive for both v1 & v2

    Current builder/parser has bugs too (not just runtime...), proved hard to
    extend (e.g., for import & for compact syntax)

    Plan A means making builder code work for building both v1 & v2 data
    structures

    I used to think plan A would save work but I am not so sure any more and am
    leaning towards plan B

Done 2013may: Consider important bugs (reset has expansion: tickets
[#9415](http://bugs.icu-project.org/trac/ticket/9415) &
[#9593](http://bugs.icu-project.org/trac/ticket/9593))

Postponed: Consider new features

    expansion string with its own prefix

    reset position with prefix

    context-after (see cldr-users discussion around 2013nov25 about Lao etc.:
    “Definition of Context-Sensitive Mappings in the CLDR Collation Algorithm”)

### Plan A: Refactor

    Keep rule parser

    Change current builder to 64-bit CEs, adapt to low-level v1 data structure
    builder continuing with 32-bit CEs

    Dynamically use either old or new “inverse UCA” map

    Dynamically use either old or new weight allocator

### Plan B: Reimplement

    Keep v1 mostly as-is, except (maybe!) to cope with new FractionalUCA.txt
    (see above)

    For v2 data structure building only: Implement new parser/token lists etc.,
    copy/modify code or use new design (see design doc)

### Builder TODOs

CollationRootElements

    Done 2013may: getPrimaryLimitsBefore(ce, isCompressible), sec, ter

CollationDataBuilder

    Done 2013may: copyFromBase(code point)

        copy all mappings for code point from base, including context

        enum context, add()

        share code with CollationSets? move to CollationData?

    Done 2013jul15: suppressContractions

        copy mapping for code point from base, excluding context

        see [Collation tailoring
        issues](https://docs.google.com/document/d/1XQIVPwyXVCu7mpfNNtp4FcPB_smgaVLNJFrXbcTUzvY/edit#heading=h.9042bh3g1d6d)

    Done 2013jul15: init 0-7F & C0-FF to explicit fallback, to allocate the
    UTrie2 blocks; Done 2013may: when building, copy-all from base for each such
    code point which did not get tailored

        Postponed: or maybe just ASCII (which is always preallocated), and leave
        other optimization up to the tailorings (measure data size change,
        propose, change builder, and update the tailorings)

    Done 2013jul15: Remove hack “tailoring”: Tertiary root CEs 3f30, 3f32
    (U+FFF1, U+FFF2)

        genuca2.cpp & collationtest.cpp & .txt

        Replace with appropriate @ rules & test tertiary CEs with uppercase bits

        Look through collationtest.cpp for U+FFF1 etc., clean up

        Also clean up .txt test of a-z after \[last secondary ignorable\]

    Postponed: Maybe look at sets of 4 code points where 1..3 have tailoring
    mappings, and optimize (copy from base) the other ones as long as they are
    just single CE32s (without expansions \[except Latin mini\] or context).

    Postponed: getJamoCE32s(): Try to prevent \[optimize \[Jamo\]\] from
    counting as anyJamoAssigned. (As of CLDR 24 \[2013\] the Korean tailoring
    does not optimize conjoining Jamo.)

CollationRuleParser

    Done 2013may: support v1 syntax except tailoring \[variable top\]

    Done 2013may: support quaternary relations

    Postponed: support \[last Grek\], \[last punct\] (paper note 20130305)

        <http://unicode.org/cldr/trac/ticket/5549>

        uint32_t scriptFirstPrimaries\[num scripts + num special groups\]

        bool haveScriptFirstPrimaries

        void initScriptFirstPrimaries()

            independent of script metadata changes (script metadata does not
            have special groups anyway)

            enum base FDD1 contractions; check gc --> groups; gc=L -->
            0<=script<num

    Done 2013may: const char \*errorReason

    Done 2013may: error in \[import\] --> overwrite ParseError index to start of
    \[import\]

    Done 2013oct06 (BundleImporter in collationbuilder.cpp): Write runtime
    \[import\]er

    Done 2013jul16: Make CollationTest % reorder parser use as much of the
    CollationRuleParser \[reorder\] parser as possible

    Done 2013jul16: parse maxVariable

    Done 2013may: Forbid tailoring U+FFFD, U+FFFE & U+FFFF

CollationBuilder

    Done 2013may: Tokens --> CEs

    Done 2013may: Forbid tailoring to/around unassigned code points

    Done 2013jul18: Adjust special reset positions according to previous rules

        <http://unicode.org/cldr/trac/ticket/6070#comment:6>

    Done 2013jul19: Review once more \[first variable\] and \[first regular\]
    and \[first trailing\]

        Initially implemented them as boundary CEs

        I think they should be real root CEs

            Able to tailor primary-before \[first variable\] and \[first
            trailing\]

                Unless a rule tailors weak-around FDD1+x

            Primary-before root \[first regular\] is still regular, not variable

            But primary-before the boundary contraction becomes variable

        verify that there is at least one rootElements entry with an
        unassigned-implicit primary

    Done 2013jul15: Case bits

        See [Collation tailoring
        issues](https://docs.google.com/document/d/1XQIVPwyXVCu7mpfNNtp4FcPB_smgaVLNJFrXbcTUzvY/edit#heading=h.enzf54i37goo)

    Done 2013jul16: Make Hangul work, without context

        Forbid conjoining Jamo in context-sensitive mappings (in the NFD
        prefix/str)

        Forbid conjoining Jamo with expansions — expansions used in search
        tailorings

        Build jamoCEs if any of the conjoining Jamo are tailored (done already)

        Test Hangul & Jamo

    Done 2013aug04: Support conjoining Jamo with expansions and maybe some
    context

        Done 2013jul28: Support expansions

            Store ce32s, not ces

            appendCEsFromCE32()

            copy from base/copy expansions

        Done 2013jul29: do not add mappings from original rule strings with
        Hangul syllables (ignoreString())

        Done 2013jul29: test Jamo expansions

        Done 2013aug02: probably support at least Jamo L in prefixes, and
        prefixes on Jamo L — even more: r34008

        Done 2013aug04: consider using a HANGUL_TAG bit to indicate that none of
        the Jamos has a special CE32; set it on all Hangul syllables, or for all
        L\* for certain L; inherit it to the tailoring, especially if that does
        not tailor any Jamos (copy all Hangul CE32s in initForTailoring()?)

        Postponed: We could probably support Jamo contractions as well as in v1
        by putting the V+T into the skipped buffer, recursing on the L, and
        looping/recursing over the rest of the skipped buffer. The data builder
        would have to do equivalent processing. The builder should then do at
        least Hangul tail closure. We would still have the usual problems with
        other overlap contractions.

        Done 2013aug03: Support Hangul in prefixes, and in the interior of
        contractions. The builder would have to do Hangul closure on prefixes.
        The data builder would have to support Hangul lookup for the closure
        process.

    Done 2013jul17: Japanese vs. \[hiraganaQ on\]

        <http://unicode.org/cldr/trac/ticket/5015> “deprecate the
        hiraganaQuaternary collation attribute”

        [\[:Kana:\]](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3AKana%3A%5D&g=)
        = \[ァ-ヺヽ-ヿㇰ-ㇿ㋐-㋾㌀-㍗ｦ-ｯｱ-ﾝ𛀀\]

        [\[:Hira:\]](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3AHira%3A%5D&g=)
        = \[ゝゞぁ-うゔぇ-おゕか-ぐゖけ-ぽ🈀ま-ゆ𛀁ょよゟら-ん\]

            BMP only: \[ゝゞぁ-うゔぇ-おゕか-ぐゖけ-ぽま-ゆょよゟら-ん\]

        Consider for now tracking context-sensitive mappings that include
        Katakana, and look at single-character (frozen) \[:Kana:\], fetch CEs
        for all such items, assert quaternary==0, set q=1 in last CE, change
        mappings → make it work without having to change the tailoring

        Alternatively, add <<<< to the v1 parser and just treat it like = there.
        Make the v2 parser ignore \[hiraganaQ on\]. Change the Japanese
        tailoring to use <<<< and also keep \[hiraganaQ on\] for now.

            Done 2014feb22: Change CLDR Japanese tailoring and re-enable the
            check in v2 CollationRuleParser forbidding \[hiraganaQ on\].

    Done 2013may: &\[last regular\]

    Propose to make this equivalent to &\[first Hani\] where that’s available.
    (Propose to CLDR & ICU) The actual “last regular” would be available only if
    the last script is known.

    Alternative would require changing all Han tailorings.

    Done 2013aug02: Canonical closure

        (Learn details of v1 closure process)

        v1 can do &x=a &u=â →

            sort with x: a, å, ...

            sort with u: â, ấ, ậ, ...

        Å U+212B Angstrom Sign

        [\[:toNFD=/a/:\]](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3AtoNFD%3D%2Fa%2F%3A%5D&g=)

        Closure on suppressed contextual mappings(?) — punted, see documentation
        TODOs

            add the set of suppressed base context strings to the canonical
            closure

            try to remove suppressed builder context strings

            the CollationDataBuilder could have a suppress(code point) method
            which returns the index to the ConditionalCE32, and iterator/getter
            methods to access the prefix and contraction strings + their ce32

        Closure exclude Hangul

            As long as we don’t support context with Jamo/across Hangul, all
            Hangul syllables should either fall back to the base, or else
            (especially if any of the conjoining Jamo are tailored) set the
            Hangul specials and set the jamoCEs.

            Agreed in discussion 20130617 with Mark: Don’t support context
            with/across Hangul, document limitation.

        Builder track UnicodeSet of tailored inputs — seems unnecessary, apply
        closure immediately

            str or prefix\*str where \*=FFFE or FFFF

            at least inputs that are not NFC-closure-inert

            exclude purely optimized items (known same as base)

        v1 ucol_elm.cpp:

            uprv_uca_canonicalClosure() first closes over composites (done in
            v2); _enumCategoryRangeClosureCategory(), called from there, uses
            ucol_strcoll() rather than comparing CEs, which might miss
            differences in case bits and Hiragana-ness. It also appears to
            attempt closure with prefixes but only copies one ce32, which is
            generally broken.

            Then it looks for the last base character and following ccc!=0
            combining mark in each prefix+string input, but it looks only for
            BMP characters (getFCD16(el.cPoints\[j\])). It checks for the tccc
            ((fcd & 0xff) == 0) rather than the lccc; this is probably ok,
            assuming that these are NFD strings.

            With the last (baseChar, firstCM) it calls
            uprv_uca_addTailCanonicalClosures(). That iterates over all
            combining marks with lower ccc than firstCM (right? and why not ones
            with higher ccc?), and if baseChar+mark NFC-composes to one BMP(!)
            composite (which misses some composites because of
            Composition_Exclusion), then it replaces the last occurrence in
            prefix+string of firstCM with the new mark, NFC, appends firstCM to
            both the modified input and the NFC version (so the strings might
            then not be normalized). Then, if a prefixLookup finds no prefix,
            adds a mapping from the NFC+firstCM string to the CEs as returned by
            a collation element iterator working on modifiedInput+firstCM,
            regardless of whether the new string already maps to those CEs.
            uprv_uca_setMapCE() always encodes an expansion, even if there is
            really only one CE; uprv_uca_finalizeAddition() just adds the
            mapping without further closure. This appears to add only mappings
            with trailing firstCM, rather than composites that include firstCM,
            which seems broken.

            Regardless of the prefixLookup, the function calls
            uprv_uca_addFCD4AccentedContractions() if the second-to-last
            UChar(!) of the NFC+firstCM string has lccc==0 (which is true if
            baseChar+mark composed?), and it calls
            uprv_uca_addMultiCMContractions() for all but the first iterated
            combining mark and its composite (what does that do?).

            Convoluted, not documented well, appears to try to add overlap
            contractions for contractions that end with ccc!=0 but not others,
            does not add composites that include firstCM, seems generally buggy.

        Experimentally disabled ucol_elm.cpp uprv_uca_addTailCanonicalClosures()

            Makes for 11 test failures in /tscoll/cmsccoll/TestVI5913

            <http://unicode.org/cldr/trac/ticket/1514>

            <http://bugs.icu-project.org/trac/ticket/5913> especially comment
            1/option 4

            <http://bugs.icu-project.org/trac/review/5913> (only C++!)

            does seem buggy though: if the string does have earlier characters
            (x+a+hat) then v1 does not find the x+a-dot-hat combination; the
            test probably passes because it tests with minimal contractions

            Characters with 3 combining marks:
            [\[:toNFD=/..../:\]](http://unicode.org/cldr/utility/list-unicodeset.jsp?a=%5B%3AtoNFD%3D%2F....%2F%3A%5D&g=)

        2013aug02: checked in new addTailComposites(nfdPrefix, nfdString) --
        called by addWithClosure()

    Done 2013aug07: genrb build every (CLDR) tailoring with the collv2 builder
    too

        Done 2013jul29: Ensure that it can at least handle all of the syntax and
        build something

        Done 2013aug13: zh.txt:2794: CollationBuilder failed at zhuyin/Sequence
        rule offset 49: U_UNSUPPORTED_ERROR tailoring primary after ignorables
        not supported

            <http://unicode.org/cldr/trac/ticket/6511> zhuyin tones tailored too
            strong?

                fixed in CLDR 24

            If the tailoring is as intended, then tailor the primary tones
            primary-before \[first variable\].

            Data patched for now, using secondary CEs:
            <http://bugs.icu-project.org/trac/changeset/33998>

        Done 2014feb21: ko.txt:205: CollationBuilder failed at searchjl/Sequence
        rule offset 395: U_UNSUPPORTED_ERROR contextual mappings with conjoining
        Jamo (hst=L/V/T) not supported

            email 20130728 “need to support Hangul Jamo contractions?”

            Might be able to turn these contractions into prefix rules: Make L|L
            a secondary CE, then make the compound LL expand to regular LL. The
            first L cannot be a trailing character of a decomposition mapping,
            and the second L should be able to handle the prefix via recursion.
            Permit this much context for Jamo but no more. (L can have a prefix
            but no contraction. L can be in a prefix but no V or T.)

            Data patched for now, removed Jamo L+L contractions:
            <http://bugs.icu-project.org/trac/changeset/33999>

            Done 2013aug05: CLDR ticket proposing data change; v1 & v2 should
            both be able to handle L|L prefix rules, except v1 cannot handle
            &L+L=L-L so changing the L-L tailoring would have to wait for v2;
            maybe make L-after-L equal to a single-secondary-byte combining mark
            so that we can tailor &mm=G|GG=GG|GG

                after merging to C trunk:
                <http://unicode.org/cldr/trac/ticket/6546> replace Jamo
                contractions in Korean searchjl collation

                    done 2014feb21 on CLDR trunk, will be integrated into ICU 53

                v1 apparently fails with &m=L|L too, and we already know it
                creates ill-formed secondary CEs for &\[before 2\]\[first
                primary ignorable\]...

        Done 2013aug07: Print size stats (toolutil? factored out of genuca2.cpp
        buildAndWriteBaseData())

        Done 2013aug07: [Compare sizes with
        v1](https://docs.google.com/spreadsheet/ccc?key=0Ag3w_MjvUEoRdFM2WEVoODY4M1ozVW56NDlESTRSRlE#gid=0)

        Done 2013aug07: CollationDataWriter (factored out of genuca2.cpp
        buildAndWriteBaseData())

    Postponed: The Korean tailoring probably makes thousands of secondary
    differences on each Jamo T. Is there any way to share CEs among tailorings
    that differ on at-least-same-level earlier in an expansion? (On the other
    hand, this is the tailoring that got smaller the most compared with v1.)

## Integrate with C plug-in interface & service registration

Markus Yoshito?

Done 2013oct: Done while replacing old code with new.

Old notes: Might be easier than with v1 because I believe the plug-in and
service registration code rely on C++ Collator dispatching. v1 UCollator has a
“delegate” C++ Collator, v2 should just go through C++ Collator anyway.

## Port to Java

Markus & Yoshito

Notes:

    Implementation package: com.ibm.icu.impl.coll

    Use long for 64-bit CE

    uint32_t primary? which is better/faster?

            long p = ce >>> 32, simple comparisons

            int p = (int)(ce >> 32), simple “==” but need to use “trick” for “<”

        long primary

        int ce32

        Pitfall:

            C++ cast from uint32_t to int64_t zero-extends, so (int64_t)p and
            (int64_t)(sec << 16) is fine and efficient.

            Java cast from int to long sign-extends, so we need to either mask
            off the high bits after the cast – (long)p & 0xffffffffL – or cast
            to long before shifting into the sign bit – ((long)sec << 16).

                Search for mask values: 0x\[89abcdef\]\[0-9a-f\]{7}\[^L\]

                Of course, this is irrelevant if we shift-left-by-32 the value
                after casting to long.

    CollationIterator.handleNextCE32() could return a long with the code point
    in the upper half (bits 52..32 or -1, use signed “>>”) and the ce32 in the
    lower half (bits 31..0)

        done, see helpers in the base CollationIterator:

            long makeCodePointAndCE32Pair(int c, int ce32)

            NO_CP_AND_CE32 which is the same as
            makeCodePointAndCE32Pair(U_SENTINEL=-1, Collation.FALLBACK_CE32)

    work on String or CharSequence?

        Keep the public API with String and use CharSequence in the
        implementation.

    Port C++ “public” to Java package-private visibility – until we find that
    something is indeed needed from another package.

    [CharsTrie](http://icu-project.org/apiref/icu4j/com/ibm/icu/util/CharsTrie.html)
    works on CharSequence, not on char\[\] – CharsTrie(CharSequence trieChars,
    int offset)

    Java: Port SharedObject or use some JDK reference counting?

        Search for “Java atomic increment” and “Java shared mutable” etc.

            <http://docs.oracle.com/javase/1.5.0/docs/api/java/util/concurrent/atomic/package-summary.html>

                ... extend the notion of volatile values ...

                ... get has the memory effects of reading a volatile variable.

                [AtomicInteger](http://docs.oracle.com/javase/1.5.0/docs/api/java/util/concurrent/atomic/AtomicInteger.html)

            <http://docs.oracle.com/javase/tutorial/essential/concurrency/atomic.html>

                Read/write of volatile fields implies memory barriers, but
                nothing says that increment and decrement operators would be
                atomic. Probably best to use class AtomicInteger.

        Java Collator does not get deleted, removeRef() won’t be called?!

            Write finalize() { settings.removeRef(); settings = null; } – seems
            no worse than usual Java memory management.

        Ported SharedObject, see usage comments there.

        Revisit; maybe just shallow-clone the settings each time we want to
        modify them, and remove SharedObject?

            That would always clone the CollationSettings itself, and its
            fastLatinPrimaries

    Markus messed up some global search&replace.

        Some function calls like switchToForward(errorCode) lost not just the
        “errorCode” but also the parentheses, so they look like variables. Just
        add “()”...

        Some spaces removed that I shouldn’t have, like in “extends SomeClass”.

    Check old code for Exception types and messages. We might want to try to
    match types, but we should be free to improve/clarify the messages.

Please

    use final and @Override

    before you start porting a file, put your name after the file name in the
    list below

If something needs to be changed in C++, note it here.

    TODO(Markus, after ICU 53): machine-generate CollationFCD.java initializers
    by icu/tools/unicode/c/genuca/genuca.cpp

Other changes to be made in ICU4J

    TODO(after ICU 53): create a common, public constant somewhere in ICU4J
    corresponding to C/C++ U_SENTINEL? (move Collation.SENTINEL_CP there)

        FYI: there is com.ibm.icu.text.UForwardCharacterIterator.DONE=-1 –
        document <0, and maybe make it equal to the new constant?

        there is also UnicodeSetIterator.IS_STRING=-1

        maybe create a new public class CodePoint with SENTINEL, static String
        toString(int), isSurrogate(int), ...?

        or UCharacter.SENTINEL?

    TODO(after ICU 53): propose widening CanonicalIterator to take CharSequence
    input -> for CollationBuilder, check other callers

    Done 2014mar04: use ICU-specific exceptions
    (<http://bugs.icu-project.org/trac/ticket/10683>), document them where
    appropriate

        Normalizer2Impl.load() sometimes throws an IOException which is
        immediately caught and then re-thrown as a RuntimeException.

    TODO(after ICU 53): ask Mark:
    AlphabeticIndexTest.TestFirstScriptCharacters() is now pretty much circular
    with the implementation code, but it’s harmless; keep or delete?

    TODO(after ICU 53): consider moving CollationKeyByteSink into
    RawCollationKey as nested class, with an instance cached in the key; and
    maybe similar in C++

    TODO(after ICU 53): we can probably simplify BOCSU.getNegDivMod() because
    Java specifies the div/mod behavior

### Runtime

    Collation.java - Markus - done

    CollationCompare.java - Yoshito - done

    CollationData.java - Markus - done

    CollationDataReader.java - Markus - done

    CollationFastLatin.java - Markus - done

    CollationFCD.java - Markus - done

    CollationIterator.java - Markus - done

    UTF16CollationIterator.java extends CollationIterator - Markus - done

        FCDUTF16CollationIterator.java - Markus - done

    IterCollationIterator.java extends CollationIterator - Markus - done

        Take & use ICU UCharacterIterator as before.

        FCDIterCollationIterator.java

    CollationKeys.java - Yoshito - done

    CollationLoader.java - Yoshito - done

        only CollationLoader class, ucol_getDisplayName, etc.. are not included

    CollationRootElements.java - Markus - done

    CollationRoot.java - Markus - done

    CollationSettings.java - Markus - done

    CollationTailoring.java - Markus - done

    ContractionsAndExpansions.java - Yoshito - done

        See Normalizer2Impl.addLcccChars(), addPropertyStarts(),
        addCanonIterPropertyStarts() for examples of how to iterate the newer
        type of Trie in Java (C++ UTrie2).

    SharedObject.java - Markus - done

    TailoredSet.java - Yoshito - done

        Somewhat similar to ContractionsAndExpansions.java, see comments above.

    icudata.jar - Markus - done

    delete impl.ImplicitCEGenerator.java - Markus - done

        check if the UnicodeTools still depend on it (after svn update of
        UnicodeTools and also CLDR code) - done

### API

Check i18n & i18n/unicode diffs, merge into existing Java API classes.

Make sure to preserve existing Java public/protected signatures, especially
argument & return types.

Note: Initially, I (Markus) copied all files into .impl.coll, to make it easier
to grab all files for “global” edits. Eventually, pieces of semi-ported code in
classes that go elsewhere need to be merged into those existing classes.

    Collator.java - Markus - done

    RuleBasedCollator.java - Markus - done

        using CollationBuffer for thread safety of frozen instances, as before

    CollationElementIterator.java - Yoshito -

        The old Java code seems to contain code for both the low-level collation
        algorithm (which in the new code is in CollationIterator.java) as well
        as for the utility CollationElementIterator API. \[In the old C++ code,
        the low-level code was in ucol.cpp.\]

        In C/C++, the old C++ coleitr.h/.cpp was only a shim over C
        ucoleitr.h/.cpp which contained the logic for the C UColElements API. In
        the new C++ code, the CollationElementIterator class has the API logic
        again, and uses an appropriate CollationIterator subclass, and the C
        code is back to just a wrapper.

        In C++, the C code still has special code for internal use by
        UStringSearch. This was more lightly modified than other collation code.
        Some of it was moved to other files for better layering.

        Nearly all of this C/C++ stuff should be collected in the
        impl.coll.CollationElementIterator.java file, except where some of it
        moved into a C/C++ search code file (check the source/i18n diffs).

        It’s probably best to start by moving/porting CollationElementIterator
        API functions by copying equivalent bits and pieces from
        impl.coll.CollationElementIterator.java, and working top-down from
        there. Then delete unused code. (That’s pretty much what Markus did with
        the RuleBasedCollator.)

        Markus already moved over a few helper functions and the
        getMaxExpansion() stuff (in order to make progress on the RBC).

        Done 2014feb26: merge in API doc changes from [C++ i18n/unicode
        diffs](http://bugs.icu-project.org/trac/changeset?reponame=&new=HEAD%40icu%2Fbranches%2Fmarkus%2Fcollv2/source/i18n/unicode&old=34462%40icu%2Ftrunk/source/i18n/unicode)
        (coleitr.h, ucoleitr.h); fix userguide links, refer to the “root
        collator” rather than “UCA” or “DUCET” where appropriate

        Done 2014feb26: throw new RuntimeException("Illegal change of
        direction") -> IllegalStateException

    TODO(Markus, future): RBC setters: Change void setXyz() to returning
    this=RBC. Consider pushing APIs down to Collator, too (covariant setters).
    (Note: C++ virtual setAttribute() is already in the base class and returns
    void. May not be worth duplicating for a chainable version.)

### Port changes in other existing files

    common - Markus - done

    common - Markus - n/a

        C++ had
        [unorm_it.h](http://bugs.icu-project.org/trac/browser/icu/trunk/source/common/unorm_it.h?rev=11809)/.c
        with UNormIterator which got deleted. Is there something like it in
        Java?

        -> unable to find it, and it looks like the old Java code never used a
        CharacterIterator for collation (CEI apparently always just extracted
        the underlying string)

    i18n (not search code) - Markus - done

        only AlphabeticIndex improvements, aside from core collation and search

### Search code changes

    - Yoshito - done

    ucoleitr.h/cpp copied into .impl.coll.CollationElementIterator.java; some of
    the C++ functionality is only there

### Builder

Get rid of all errorReason fields and getters: Add that information to the
exception message string. Throw the exception from inside existing (in the new
code) setParseError() functions etc. - done

    CollationBuilder.java - Markus - done

    CollationDataBuilder.java - Markus - done

    CollationFastLatinBuilder.java - Markus - done

    CollationRuleParser.java - Markus - done

    CollationWeights.java - Markus - done

    delete com.ibm.icu.text.CollationRuleParser & CollationParsedRuleBuilder -
    Markus - done

### Do not port to Java

    C API, except if there is no C++ API and we need to port C to Java

    tools/genuca2

    CollationBaseDataBuilder: used only by genuca2 tool

    UTF8CollationIterator, compareUTF8(), ...

    Handling of NUL-terminated strings

    Data swapping

    CollationDataWriter.java: Java Collator and RBC have no equivalent for C++
    RBC::cloneBinary() which serializes the collator data and settings into a
    binary blob. It also does not write .res files nor ucadata.icu.

### Tests

    CollationTest.java - Yoshito - done

        There already is a com.ibm.icu.dev.test.collator.CollationTest.java –
        Markus deleted the old-data-driven-test code that was there – pour the
        new CollationTest code in there.

        see UCAConformanceTest.openTestFile(); and/or see how Andy finds & loads
        UTF-8 .txt files for rbbi & regex testing

    [collationtest.txt](http://bugs.icu-project.org/trac/browser/icu4j/branches/markus/collv2/main/tests/collate/src/com/ibm/icu/dev/data/collationtest.txt)
    - Markus - done

    CollationTest_\*.txt - Markus - done

    other test changes: see C/C++diffs - Markus - done

        modify as in cintltst:

            custrtst.c - n/a

            cmsccoll.c - CollationMiscTest.java - done

            citertst.c - CollationIteratorTest.java - done

            creststn.c - n/a

            callcoll.c - CollationDummyTest.java - done

            crestst.c - n/a

            capitst.c - CollationAPITest.java - done

            udatatst.c - n/a

        modify as in intltest:

            colldata.cpp - n/a

            intltest.cpp - n/a

            ucaconf.cpp - UCAConformanceTest.java - done

            ssearch.cpp - n/a

            regcoll.cpp - CollationRegressionTest.java - done

            tscoll.cpp - n/a

            alphaindextst.cpp - AlphabeticIndexTest.java - done

            itercoll.cpp - CollationIteratorTest.java - done

            svccoll.cpp - CollationServiceTest.java - n/a

            thcoll.cpp - n/a

            apicoll.cpp - CollationAPITest.java - done

        delete as in intltest:

            cntabcol.cpp - unable to find equivalent Java test code - n/a

            dadrcoll.cpp - remove old data-driven test code from
            CollationTest.java - done

### Cleanup

    Review TODOs in collation .java files (note: some are also in the C++ code)
    - Markus - done

    Review copyright dates - Markus - done

        Copyright of new Java files should reflect C++ where they were ported
        from, or the start of the Java port?

        Markus started the Java port on 2014-jan-13.

    Review @since/@author tags. - Markus - done (reverted to C style comments)

    Done 2014mar03: Merge Java port into trunk

## More Tests

Overlap of contractions vs. decomposition

    Richard Wordingham, unicode list 20130206: Just add <U+0302, U+0067> to the
    default set of collating elements. The NFC string <U+011D, U+011D, U+011D>
    then consists of the four collating elements <U+0067>, <U+0302, U+0067>,
    <U+0302, U+0067>, <U+0302>, and it is impossible to handle the NFC string
    without using NFD normalisation.

getTailoredSet()

    Test \[optimize \[0-10ffff\]\]

    Test \[suppressContractions \[0-10ffff\]\]

    Test that for a character with both prefixes (p|c) and contractions (p|ci),
    if it has no-prefix contractions (ch) and has prefixes without no-suffix
    mappings (no p|c), the no-prefix contractions are not repeated for each such
    prefix (the set should not include p|ch).

TestCEValidity()

    Done 2013nov11: iterate over CollationRootElements? skip primary ranges?

→ CollationRootElementsTest (paper note 20130225)

    Done 2013nov11: Iterate over CollationRootElements

    Done 2013nov11: Check CE validity

    Check that for each primary the one before & after is returned (within given
    limitations)

    Done 2013nov11: For each primary range, test edges and one internal CE, or
    better all; test that we reach the range end exactly when iterating “after”
    through the range

    Check secondary/tertiary before & after too

    Done 2013nov11: Check that between two CEs there is a tailoring gap of
    appropriate strength (try to allocate at least one item between them)

Obsolete 2013dec06: Changed the data builder to use the runtime collation
implementation for fetching build-time CEs. As a result, there is only one
algorithm implementation, and no need to test a second one.

    CollationDataBuilder.getCEs() should get same CEs (for NFD string) as
    CollationIterator on built CollationData

    try with just baseData, optimize some (copy from base to builder), try with
    some tailoring

    maybe even run the UCAConformanceTest with it

    would need to either run the CollationBuilder to just before where it calls
    build() on the data builder, or somehow copy mappings from CollationData

Postponed: More tests

    genrb --omitCollationRules

        Test again after trunk integration that this works.

        Try to make test code work with this option, except for missing-data
        warnings.

    Try combinations of base vs. tailoring for discontiguous & nested
    contractions, to test fallbacks and fetching inner CEs from the original
    data.

    Test CODAN with first digit in base and others in tailoring.

    CollationIterator::numCpFwd: Test this with contiguous & discontiguous
    contractions and digit specials! Try to start iterating backwards from all
    code point boundaries inside varied text.

    For each tailoring in the data, which was built by ICU4C: In ICU4J, build
    from rules and check that the data is exactly the same.

    fast Latin: at least data-driven tests: try both with and without Latin
    fastpath

    Withdrawn 2014mar01 (not worth the time to find this bug in old code):
    CollationIterator::previousCEUnsafe(): Verify that the v1 code uses a
    backward buffer and gets into trouble with a prefix match that would need to
    move before the backward buffer.

    Done 2014mar01 (could not find bad behavior with old-code collation demo):
    CollationKeys: (Tertiary weights with caseFirst=upperFirst) Does v1 handle
    03 & 04 tertiary weights properly? 02? Try &\[before 3\]a<<<x and U+FFFE
    with all case settings.

## Documentation

### LDML spec

Done 2013sep04: document [new case bits
algorithm](https://docs.google.com/document/d/1XQIVPwyXVCu7mpfNNtp4FcPB_smgaVLNJFrXbcTUzvY/edit#heading=h.enzf54i37goo)

Done 2014mar10: \[first variable\] and \[first regular\] and \[first trailing\]
should be the first real such CEs, not the boundary CEs.

Done 2014mar10: Forbid tailoring to \[last trailing\]. (That’s U+FFFF.)

Done 2013sep04: Special reset positions always resolve to single CEs. They can
be affected by rules that tailor expansions.

Update the spreadsheet of special byte values. Include sec/ter boundaries. Add
such a table into the LDML spec, as far as is relevant for FractionalUCA.txt.

At least document FractionalUCA.txt compressible lead bytes, range of sec/ter
bytes for compression.

Rule syntax: (setting | comment | rule chain)\*

rule chain = reset (comment | relation)+ // at least one relation

relation = operator \[prefix ‘|’\] string \[‘/’ extension\]

Done 2014mar10: Script reordering after check for variableTop

Gotcha: &ĉ<c<<<C yields c <2 ĉ because of canonical closure!

Done 2014mar10: CLDR Coll Alg also adds spec for

    case weights in a separate level or as high-order components in tertiary
    level, see 3.13 Case Parameters, which enable

        lowercase first & uppercase first (caseFirst setting)

        “ignore accents” (strength=primary & caseLevel=on)

        “ignore variants” (strength=secondary & caseLevel=on)

    reordering groups and their boundaries, for maxVariable and script
    reordering

Done 2014mar10: Case Param. notes

    p+c ignores sec because it wants to “ignore accents”

    s+c ignores ter for equivalent reason: “ignore variants” (except case)

    t+c or caseFirst: ter CEs need high case weight for WF2; together with usual
    high-tertiary-weight the combined ct is high too

### ICU User Guide

Hangul limitation

    limited contextual mappings with conjoining Jamo

    verify that it’s a new limitation, document version numbers

Closure limitations

    no closure on suppressContractions set

    tail closure on last starter that is NFC_QC=Yes and not a Jamo L

    no other overlap closures

ICU Data -> new data formats

Forbid use of \[last trailing\]. (That’s U+FFFF.)

Forbid tailoring to \[last implicit\].

Done 2014mar30: document byte values
(<http://site.icu-project.org/design/collation/bytes>) , maybe in LDML spec,
maybe also ICU-specific stuff in User Guide (from
<http://bugs.icu-project.org/trac/ticket/7757>)

ICU pri lead bytes E0..FF different from FractionalUCA.txt.

Document genrb options in collation and/or ICU Data chapters:
--omitCollationRules --includeUnihanColl --noBinaryCollation

Email to self on 2012nov06:

    <http://userguide.icu-project.org/collation/architecture#TOC-Merging-sort-keys>

    <http://icu-project.org/apiref/icu4j/com/ibm/icu/text/CollationKey.html#merge(com.ibm.icu.text.CollationKey)>

        at a minimum, show APIs in user guide

See not-done LDML spec notes above. Maybe just in the ICU User Guide?

### ICU API docs

Done 2014mar04: Update API docs for hiraganaQuaternary attribute.

### ICU Readme & download page

    Done 2014feb26

    Java Collator: getters for strength, decomposition mode, and locales return
    hardcoded default values; their setters do nothing

    Java Collator.compare(Object, Object) now works with CharSequence, not just
    String

    Passes full UCA conformance tests now

    Data 1.1MB smaller; trunk at r35221: 4.7MB, with collv2 merged in: 3.6MB

## Cleanup

    Done 2013oct: Increment runtime & builder version numbers.

    Obsolete 2013nov09: Can we remove the UTrie(1) builder code once collation
    has switched over to UTrie2?

        no, still in use

        the RBBISetBuilder still uses a UTrie(1)

        gensprep builds a UTrie(1)

    Done 2012nov26: remove unorm_it.h / unorm_openIter()

    Done 2013dec12: TODOs in AlphabeticIndex and its tests

        <http://bugs.icu-project.org/trac/ticket/9858> and other “index” tickets
