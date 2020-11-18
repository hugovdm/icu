# ICU Ticket Life cycle

[TOC]

(See [Submitting ICU Bugs and Feature Requests](../bugs.md) for general
information about the ICU JIRA issue tracker.)

ICU tickets follow a definite path through their life, from initially being
opened, through implementation or fixes, to closing and release of the changes
in an updated version of ICU.

## Ticket Types

1.  **Bugs**: These are opened to report some problem with ICU library code.
    *   A good bug report should include enough information to allow the problem
        to be reproduced, including (as appropriate)
        *   the version of ICU
        *   the Operating system type and version
        *   the compiler version
        *   for Java code, the JDK version
        *   a code snippet that exhibits the problem, preferably small and
            self-contained
    *   Some old tickets have Type=Bug but are bug reports on API docs, tools
        issues, etc. These predate the more specific ticket types.
2.  **Enhancements**: These are opened to report areas where ICU can be improved
    but where there is not necessarily a defect. Examples: performance
    improvement; sanitizer patches; build system changes.
3.  **New Features**: These are opened to request the addition of new functions
    or features to ICU.
    *   Note: Some old tickets have an "RFE:" summary prefix (Request for
        Enhancement), but since we switched to better issue trackers with Type
        fields we no longer use such prefixes.
    *   Most tickets with this type will require an API proposal.
4.  **Tasks**: These represent work items to be done in support of the ICU
    project that aren't directly visible to users of the library.
5.  **Tests**: For bugs or enhancements in test code that do not affect library
    code.
6.  **API Docs**, **User Guide**, ...: For non-code issues.

## Ticket Component

We set a component for the part of ICU that has a bug or could be improved. We
use this for filtering/prioritizing/scheduling. See the [ICU Components page on
Jira](https://unicode-org.atlassian.net/projects/ICU?selectedItem=com.atlassian.jira.jira-projects-plugin%3Acomponents-page).

## Submission

Tickets may be submitted by anyone, at any time. Before submitting a bug report,
it is good practice to check whether the problem has already been reported.

## Incoming Tickets

The ICU team reviews new tickets roughly once a week. (Sometimes new tickets
come in faster than we can handle.)

We discuss each ticket and either assign it to a team member or close it as
duplicate, out of scope, etc.

Even team members should assign/accept tickets only after discussion in our team
meeting. If something is urgent, send an email to the team mailing list with the
ticket link and a brief explanation and note that you are grabbing the ticket.

We usually schedule tickets for an upcoming ICU release, but we have been more
eager with accepting and scheduling tickets than we have been able to handle.
Unfortunately, neither an upcoming "Fix Release" nor a high priority is a
guarantee that a ticket will actually be fixed then. The only guarantee is
Status=Done, after the fact.

Prioritization is primarily based on the requirements of the companies providing
the developers to the project, and/or based on the priorities of the team
members.

In order to move a ticket out of Status=New, it needs to have the following
fields set:

*   component
*   fix version (can be "future" to leave it unscheduled)
*   assignee
*   priority≠assess

Once these are set, and after team agreement or email notice, team members can
click "Accept".

## Design

If we are not sure about the right approach for something, we put the ticket
into Status=Design. The owner should come up with a plan that the team agrees
with before moving to Accept or maybe closing the ticket as infeasible etc.

## Development and Review

At the time actual work begins, the developer should accept the ticket, and set
the Fix Version field to the upcoming ICU release (if it is not set already).

For code changes: See the [git for ICU
Developers](../repository/gitdev/index.md) page. After one or more pull requests
are approved and merged, an ICU team member (the ticket owner if they are one)
closes the ticket as Done/Fixed.

For non-code changes: Update the User Guide text, do your task, etc., then add a
comment to that effect, set a ticket reviewer, and put the ticket into
Status=Reviewing. The reviewer may ask you to do more via Status=ReviewFeedback,
or they may close it as Fixed if they are satisfied.

## Obsolete: svn branch development

For non-trivial commits (If in doubt, it's non-trivial), it's best to do feature
work on a **branch**. See
["Merging-and-Branching"](http://site.icu-project.org/repository/tips#TOC-Merging-and-Branching)
for some tips on branch work. Here are some examples of why you would do work on
a branch:

*   You expect *contention*: It's a large involved change, over a period of
    time, and other developers may commit code that conflicts with yours.
*   You are looking for *review* and/or *testing* during the development
    process: Others can pick up the branch and try it, or view the commits.
*   You expect *test breakage* during the development process. Commits to the
    trunk should not break tests. Work on a branch until the tests are clean.

All commits of code back to subversion must include the associated ticket number
with the commit message. This information will be used by reviewers to identify
the complete list of changes associated with a particular ticket.

svn commit messages have the form
ticket:1234: a short message describing the changes being checked in

## Fixed by Other Changes

If a ticket - defect or feature - is handled by changes associated with a
different ticket, a brief explanation of the situation should be added to each
ticket involved, and all tickets except for the one where the real work happened
should go to Status=Review; or go directly to Done/Fixed if the connection is
obvious.

We do not want to lose track of the fact that a reported problem was fixed
simply because it was one of a group of related problems that were fixed by a
single set of code changes.

## Testing

For defects, before making any fixes to the implementation code, a test should
be written that fails because of the problem. The problem should then be fixed,
the fix being checked by the just-written test.

## C and Java

Most ICU services exist in both C and Java. Any defect reported against one side
should be checked (the test ported) to the other, and if it exists, it must
there as well. This must be done *before* moving to the review step.

## Maintenance Releases

Certain high-impact tickets may be added to maintenance branches. To propose
that a commit be added to the maintenance branch, cherry-pick your commits to
the maintenance branch and create a pull request against the maintenance branch.
For more details, see [gitdev](../repository/gitdev/index.md).

Mostly Obsolete:

~~For merging changes from the Unicode master to a Unicode maintenance branch, use a maintenance ticket such as [ICU-20049](https://unicode-org.atlassian.net/browse/ICU-20049) and use pull requests with cherry-pick commits from theUnicode master. For the ticket Fix Version use the next maintenance number (62.2 after 62.1) regardless of whether we actually plan for a release.~~

~~**TODO**: Add instructions for how to make a cherry-pick commit.~~

~~**TODO: Review this section for the GitHub era. This seems like the wrong page for this contents. Merge into [BRS/Publish#](goog_994586480)[Tag related svn files](release/tasks/publish/index.md) and/or [Source Code Access](../index.md) and/or [git for ICU Developers](../repository/gitdev/index.md)?**~~

~~For each release we create a maintenance branch, typically before we publish a release candidate. Example: <http://bugs.icu-project.org/trac/log/branches/maint/maint-60>~~

~~The trunk and maintenance branch may diverge some before the initial release. Changes that are agreed for the release candidate are cherry-picked by the release manager into the maintenance branch.~~

~~When we publish a release candidate, we create a tag from the maintenance branch. Example: <http://bugs.icu-project.org/trac/log/tags/release-60-rc>~~

~~The release manager may commit further cherry-pick changesets from the trunk to the maintenance branch.~~

~~The initial release is another tag off of the maintenance branch. Example: ICU 60.1 <http://bugs.icu-project.org/trac/log/tags/release-60-1>~~

~~When we have bugs that should go into a maintenance branch or release, we set the next milestone (e.g., 61m1) and add the keyword "maint". The bug fix changeset is committed to the trunk using the bug fix ticket number.~~

~~When we decide to apply a bug fix to a maintenance branch:~~

*   ~~For a maintenance branch or release for the most recent ICU version, say,
    60.2:~~
    *   ~~If there is no 60.2 milestone yet, then add it to the roadmap and
        create a 60.2 release ticket.~~
    *   ~~The 60.2 release manager commits the bug fix cherry-pick changeset to
        the maintenance branch.~~
    *   ~~Then change the bug ticket to milestone 60.2.~~
    *   ~~Note: As a result, tickets for ICU 61 show what changed since the last
        ICU 60.x maintenance release, corresponding to what users see who stay
        up to date with the latest official releases.~~
*   ~~For a maintenance branch or release for an earlier ICU version, say,
    58.3:~~
    *   ~~If there is no 58.3 milestone yet, then add it to the roadmap and
        create a 58.3 release ticket.~~
        *   ~~Use 58.3 even if we do not plan to actually publish a 58.3
            release.~~
        *   ~~Do not call the milestone "58.x". Use the next available
            maintenance release minor version number.~~
    *   ~~The 58.3 release manager commits the bug fix cherry-pick changeset to
        the maintenance branch.~~
    *   ~~Then add the keyword "maint58.3" to the bug ticket.~~
    *   ~~A ticket may collect multiple such keywords.~~

~~When we publish a maintenance release, we create a tag from the maintenance branch. Example: <http://bugs.icu-project.org/trac/log/tags/release-60-2>~~

~~Note: Some of this is new since 2017-dec-13. Before that, we treated the maintenance branch and releases of the most recent ICU version like earlier ones, and we did not use release-specific maint58.3 etc. keywords.~~
