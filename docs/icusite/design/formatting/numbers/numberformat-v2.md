# NumberFormat v2

[TOC]

(Much of this page is copied directly from Markus' collation v2 page as much of
the same applies to number formatting.)

## Status

The "number format v2" reimplementation is currently in progress as of 4 May
2015 and is being done based on many of the elements of the design doc below.
(See also the !!number format v2 log!!)

## Introduction

### 2014

(First ideas noted early 2014)

What if we could implement Decimal Format anew?

What if we could write new NumberFormat API from scratch?

For reference

*   [Mark Davis' original design
    document](https://docs.google.com/document/d/186917k9tofzF4KsG77fGGDukRR5ifPSQ6YuVhi8I3gA/edit?usp=sharing)
*   [Requirements Traceability
    Matrix](https://docs.google.com/spreadsheets/d/12s6UaM_cPeKpYwZMDPaunB4tf_j4-aPrgT3ipAzULlI/edit?usp=sharing)
*   [Some Number Format usage
    statistics](https://docs.google.com/spreadsheets/d/1aKZ9SRXuHCjAh-w_pU65KwHJO_XI_fbKItQvDiNGEDc/edit?usp=sharing)
*   [Travis Keep's detailed design
    doc](https://docs.google.com/document/d/1cwyvsniY18zRIrvBMRH5pqPsCPXkrjAFtDenkR5U5cA/edit?usp=sharing)
*   [Testing old API rewrite
    progress](https://docs.google.com/spreadsheets/d/1Z8O0j5GQ0g46uyiTxiuc3NezF-mAVncpqI6i8gzDD9o/edit?usp=sharing)
*   [Metrics](https://docs.google.com/spreadsheets/d/1A9Si3_G5ECdxvFVBi2jTcSjfBxaHaGDoo0_JzK2H258/edit?usp=sharing)
*   [Code
    size](https://docs.google.com/spreadsheets/d/1t86dShhUX9cjC-7R3gyQvpMUMIInL40qNZJacq3zjkM/edit?usp=sharing)
*   [Fastpath for formatting common floating point
    values](https://docs.google.com/document/d/15qReqyYVVyAQryjycAJo0_d8m-TXlpyRdgyebTIaJFY/edit?usp=sharing)

JAVA:

*   [Decimal Format JAVA code](http://bugs.icu-project.org/trac/review/11889)
*   [FreezableBase design
    Doc](https://docs.google.com/document/d/1IhEQ-zTYd0_l7hnkLG4li1fMj_YvawV5aYr9niNhNXU/edit?usp=sharing)
*   [Work done and work left to
    do](https://docs.google.com/document/d/14RdLwuyF6r_Cs8IEoOD4448xhF47NL29Gspzb2wcdqk/edit?usp=sharing)

Open tickets:
[ICU](http://bugs.icu-project.org/trac/query?status=accepted&status=design&status=new&component=format_number&group=type&max=300&col=id&col=summary&col=component&col=status&col=owner&col=type&col=priority&col=milestone&col=weeks&order=priority)
([opened by
travis](http://bugs.icu-project.org/trac/query?component=format_number&reporter=%5Etkeep&group=resolution&col=id&col=summary&col=owner&col=type&col=status&col=priority&col=milestone&desc=1&order=id)),
[CLDR Ticket 8359](http://unicode.org/cldr/trac/ticket/8359),

## Why?

The current code ("v1") is large and complex, and feels fragile. Fixing bugs and
adding new features has become nearly impossible to do. The number formatting
code is very tightly coupled, so changing one area of code is likely to affect
everything else.

*   Easier maintenance.
*   Simpler.
*   Replace API that cannot work (e.g CurrencyPluralInfo)
*   Thread safety (e.g Calling format doesn't change the prefix string as a side
    effect).
*   Hopefully fewer bugs.
*   Maybe faster.

The current DecimalFormat API is a classic example of "kitchen sink" API. It
supports ~ 36 named attributes all of which can be set independently of each
other as well as any number of unnamed, mutable attributes. No one quite
understands how all these attributes interact. DecimalFormat objects are big and
can format and parse any amount under the sun including currency amounts. Users
often want to just format small ints.

### CurrencyPluralInfo

The CurrencyPluralInfo sub API of DecimalFormat is broken. It allows the user to
specify a different format pattern for each of the 6 plural forms. The flaw in
this design is that the plural form is determined by both the amount and the
format pattern, not just the amount.

## How?

### Good software engineering

Encapsulation. Layering.

Several different object types each with only one primary purpose. Avoid object
types that do a little bit of everything. Easier to test: More isolated
functional units with their own tests.

## Testing

Write a data-driven test so that new test cases are easy to set up and
understand. Also ICU4C DecimalFormat, ICU4J DecimalFormat, JDK DecimalFormat,
and new DecimalFormat can all be run against the same data driven tests.

## **Data Driven Test**

See
[here](https://docs.google.com/document/d/1T2P0p953_Lh1pRwo-5CuPVrHlIBa_wcXElG-Hhg_WHM/edit?usp=sharing)
for Data Driven test syntax along with instructions for running and maintaining
data driven tests for Numberformat.

## Rollout plan

Phase 1cQ2 '15C++ implementationformatting onlyno new APIPhase 1jQ2 '15Java
implementationformatting onlyno new APIPhase 2ctbdExpose top-level APIformatting
onlyPhase 2jtbdExpose top-level APIformatting onlyPhase 3ctbdParsingPhase
3jtbdParsing
