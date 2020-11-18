# ICU Processes and Procedures

[TOC]

### ICU Release Version Number Policy

ICU version number consists of up to four digits separated by dot.

*   First two digits are used for major release number. Major release numbers
    for C and J are always synchronized.
    *   Even number at the second digit indicates an official release (e.g. 4.2)
    *   Odd number at the second digit indicates a development release (e.g.
        4.3.1) for next major release.
*   Third digit in an official release number indicates a maintenance update
    which is applicable to both C and J.
    *   Incremented only when C and J have common changes, such as CLDR update.
    *   For example, CLDR 1.7.1 changes are integrated into ICU4C 4.2.1 and
        ICU4J 4.2.1.
*   Third digit in a development release number indicates a milestone
    *   ICU4C 4.3.1 -> ICU4C 4.4 development milestone 1
*   Fourth digit in an official release number indicates a maintenance update
    only applicable to either C or J.
    *   For example, ICU4C 4.2.0.1 resolved some platform specific build issues
        and no equivalent changes in J.

### Ticket Life Cycle

[Here is a description](ticket-lifecycle.md) of the life cycle of an ICU Trac
ticket, a feature request or a bug report, from initial submittal through
implementation and review to release.

### Feature Proposals and Design

1.  All API design proposals (for any additions and changes) must be proposed on
    the [icu-design](../contacts.md) mailing list.
2.  The proposal email itself must include: (See the [template
    subpage](proposal-template.md))
    1.  The proposed (new or changed) API signature(s)
    2.  A Trac ticket number
    3.  A Suggested API reviewer
    4.  A deadline for comments (usually one week from proposal).
3.  Background information in the proposal can be helpful if the topic is
    complex. If there is a lot of information, write a [design
    doc](../design/_index.md).
4.  The API signature must be reviewed by someone before submitting, and the
    reviewer must reply to the posting.
5.  In the ICU core team meeting, we have a regular agenda item to confirm
    recent API proposals (before or after code submission). Here we confirm that
    someone has reviewed it and no one objects.
6.  New/changed API that is discovered to not have proposal is withdrawn at or
    before the API Freeze milestone.
7.  We will try to write a tool to find API additions. The idea is to add a text
    file to the repository with expected additions/changes, and to check the
    actual set of APIs against that.
8.  There should not be major API additions after the API Slush milestone. Small
    additions (for example, new methods on existing classes) should be ok.
9.  Major functionality/new API or disruptive changes need to be implemented on
    a branch. The API there must be reviewed and confirmed before merging it to
    the trunk.

### ICU Coding Style Guidelines

The ICU coding guidelines are described in the user guide at
<http://icu-project.org/userguide/conventions.html>

### ICU Release Process

[The release task list](release/index.md) contains the list of things to
complete when releasing a a new version of the ICU libraries.

### Design Documents

Design Documents for various ICU services are maintained in the following
locations:

*   [Design Documents in Sites](../design/_index.md) (newer documents)
*   [Subversion Repository design
    docs](http://source.icu-project.org/repos/icu/icuhtml/trunk/design/) (older
    documents)
