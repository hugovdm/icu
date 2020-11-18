# How to contribute to ICU

[TOC]

## Why Contribute?

ICU is an open source library that is a de-facto industry standard for
internationalization libraries. Our goal is to provide top of the line i18n
support on all widely used platforms. By contributing your code to the ICU
library, you will get the benefit of continuing improvement by the ICU team and
the community, as well as testing and multi-platform portability. In addition,
it saves you from having to re-merge your own additions into ICU each time you
upgrade to a new ICU release.

## Paperwork

Starting with ICU 58, ICU is a project of the Unicode® Consortium and the ICU
Project Management Committee is a Unicode Technical Committee, the ICU-TC. ICU
will continue to be released under the [Unicode open source
license](http://www.unicode.org/copyright.html#License), which is similar to the
old ICU license. For more details, see the [Unicode
announcement](http://blog.unicode.org/2016/05/icu-joins-unicode-consortium.html).

Since 2018-July, ICU is hosted on GitHub. For patches, please submit both a Jira
ticket and a GitHub pull request. See [Submitting ICU Bugs and Feature
Requests](../bugs.md).

For contributions — bug fix patches, one-time feature additions, or ongoing work
as a team member — please sign the Unicode CLA. You are automatically asked to
do so when you create a pull request for the first time. Signing it once per
person or organization is sufficient for future contributions. (As of 2018-Aug,
the CLA assistant is not yet set up to recognize an organization affiliation, so
individuals still have to sign it for now.)

See also [git for ICU Developers](../repository/gitdev/index.md).

If you want to join the team, then please contact us. Once everything is agreed,
the ICU team adds you to the GitHub project and the Jira issue tracker.

## General Contribution Requirements

We will be glad to take a look at the code you wish to contribute to ICU. We
cannot guarantee that the code will be included. Contributions of general
interest and written according to the following guidelines have a better chance
of becoming a part of ICU.

For any significant new functionality, contact the ICU development team through
the icu-design mailing list first, and discuss the features, design and scope of
the possible contribution. This helps ensure that the contribution is expected
and will be welcome, that it will fit in well with the rest of ICU, and that it
does not overlap with other development work that may be underway.

While you are considering contributing code to ICU, make sure that the legal
terms (see ==Legal Issues== (§) below) are acceptable to you and your
organization.

Here are several things to keep in mind when developing a potential contribution
to the ICU project:

    ICU has both C/C++ and Java versions. If you develop in one programming
    language, please either provide a port or make sure that the logic is clear
    enough so that the code can be reasonably ported. We cannot guarantee that
    we will port a contribution to the other library.

    Before implementation, read and understand ICU's [coding
    guidelines](https://sites.google.com/site/icuprojectuserguide/dev/codingguidelines)
    . Contributions that require too much adaptation to be included in the ICU
    tree will probably wait for a long time.

    During implementation, try to mimic the style already present in the ICU
    source code.

    Always develop the code as an integral part of the library, rather than an
    add-on.

    Always provide enough test code and test cases. We require that our APIs are
    100% tested and that tests cover at least 85% of the ICU library code. Make
    sure that your tests are integrated into one of ICU's test suites
    ([cintltst](http://source.icu-project.org/repos/icu/icu/trunk/source/test/cintltst/)
    and
    [intltest](http://source.icu-project.org/repos/icu/icu/trunk/source/test/intltest/)
    for ICU4C and
    [com.ibm.icu.dev.test](http://source.icu-project.org/repos/icu/icu4j/trunk/src/com/ibm/icu/dev/test/)
    classes in ICU4J). New tests and the complete test suite should pass.

    Compile using the strictest compiler options. Due to ICU's multi-platform
    nature, warnings on some platforms may mean disastrous errors on other
    platforms. This can be enabled for C++ by using the --enable-strict
    configure option on any platform using the gcc or clang compilers.

    Test on more than one platform. For ICU4C, it is good to combine testing on
    Windows with testing on Linux, Mac OS X or another Unix platform. It is
    always good to try to mix big and little endian platforms. For ICU4J, test
    using both Oracle's and IBM's JDKs and/or on Android.

    Each contribution should contain everything that will allow building,
    testing and running ICU with the contribution. This usually includes: source
    code, build files and test files.

## Team

We have several [mailing lists](http://site.icu-project.org/contacts).
Contributors should at least subscribe to the icu-design mailing list, and we
also have a team-internal list that you should get added to.

We meet once a week by phone. See the [agenda & meeting
minutes](http://site.icu-project.org/projectinfo/meetings).

## Understand ICU

*   Home page: <http://site.icu-project.org/>
*   User Guide: <http://userguide.icu-project.org/>
    *   Coding guidelines:
        <http://userguide.icu-project.org/dev/codingguidelines>
    *   Introduction: <http://userguide.icu-project.org/intro>
    *   Design: <http://userguide.icu-project.org/design>
    *   How to use ICU: <http://userguide.icu-project.org/howtouseicu>
    *   etc.
*   API References: [C](http://icu-project.org/apiref/icu4c) &
    [J](http://icu-project.org/apiref/icu4j)

## Setup & Workflow

[Source Code Access](../index.md)

[git for ICU Developers](../repository/gitdev/index.md)

[Tips for developers](../repository/tips/index.md)

[Eclipse and other setup](../setup/index.md)

[Submitting ICU Bugs and Feature Requests](../bugs.md)
[ICU Ticket Life cycle](ticket-lifecycle.md)

Significant/disruptive changes should be discusses on the icu-design list or on
the team-internal list.

**API changes must be proposed** on the icu-design list, see the [API Proposal
Email Template](proposal-template.md).

Proposed changes will be discussed at the ICU meeting that follows at least six
days later. Proposals are frequently modified during email and in-meeting
discussion. Please do not "jump the gun" unless you are very confident your
proposal will go through as-is, or be prepared to revert your changes.

## Welcome

... and thanks for contributing!
