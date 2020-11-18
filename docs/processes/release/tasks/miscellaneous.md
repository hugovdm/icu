# Miscellaneous

### Complete code reviews

Nag all the reviewers to finish reviewing the code and change the status of
bugs.

[TOC]

### Check in serialization compatibility test data

ICU4J unit test contains serialization compatibility test cases. When a new
reference version is released, we build serialized object data with the version
and use it for future testing.

1.  Run "ant serialTestData" at ICU4J root directory
2.  The target generates test data and runs some serialization test cases.
3.  Once you confirm the test runs clean, copy out/serialTestData/ICU_<version>
    to main/tests/core/src/com/ibm/icu/dev/test/serializable/data/ICU_<version>.

### Release ticket

After every milestone (GA / RC / Milestone-N) is completed, create a new release
ticket in ICU Jira, then add the ticket link to the next
[milestone](http://bugs.icu-project.org/trac/roadmap). The release ticket is
used for BRS tasks, such as version update, tagging new version, merging post RC
fixes from trunk and others.

### Check integrity of Jira issues in commit messages

Every commit being shipped in the next ICU release should be labeled with a Jira
ticket that is marked as fixed with the correct fix version. Further, there
should be no Jira tickets marked as fixed with the current fixVersion that do
not have commits. To check this, run the following tool:

<https://github.com/unicode-org/icu/tree/master/tools/commit-checker>

Follow the instructions in the README file to generate the report and send it
for review.

### Fix MisTicketted

If the commit checker tool above reports any malformed commit messages, it might
mean that a bad commit made its way onto ICU master. To fix this, a rebase is
required. Since rebases can be disruptive, confirm with ICU-TC before performing
one. If ICU-TC agrees to perform a rebase, initiate the process as follows:

$ git checkout master; git pull --ff-only upstream master

$ git rebase -i --preserve-merges latest

Note: although tempting, please do not use --committer-date-is--author-date. It
changes the order of commits in ways we don't want to do.

In the interactive rebase window, choose commit messages to rewrite using the
"reword" option. Save and exit the interactive screen, and then edit each commit
message.

When ready, force-push master to your fork and give ICU-TC a day or two to
review. Before force-pushing to upstream, create a new branch on upstream with
the latest commit on the old master; name it something like "pre63-old-master".
When ready, disable branch protection on master, force-push, and then reapply
branch protection. Create a new branch named something like "pre63-new-master"
to allow users to easily switch between the two heads.

Send an email to icu-support explaining the change and how to deal with it. You
can use [this email](https://sourceforge.net/p/icu/mailman/message/36426642/) as
a model.

### Scrub closed issues in code

(1) Search for "TODO(12345)" to detect TODO items for closed issues.

(2) Do the same for logKnownIssue. (the data related logKnownIssues are often
addressed during CLDR data integration)

If the TODO or logKnownIssue references a closed issue, if the problem is fixed,
remove the TODO/logKnownIssue, or if the problem is not fixed, open a new issue
and update the reference in the code.
