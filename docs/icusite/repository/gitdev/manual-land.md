# Manually Landing PRs

# Landing an ICU commit manually

*   for [ICU](https://github.com/unicode-org/icu) core developers only
*   by [Steven R. Loomis](https://srl295.github.io/)
*   influenced by the procedure in Node.JS’s
    [COLLABORATOR_GUIDE](https://github.com/nodejs/node/blob/master/COLLABORATOR_GUIDE.md)
*   Original: <https://gist.github.com/srl295/10105f9ed9d48d2b87a5e4277b00077d>

You want to land a contribution to ICU. It's not by a team member. Given ICU's
rebase-and-merge policy, the commits need to be formatted properly. You could
ask the PR author to reformat their commits, and then use the Big Green Button.
Otherwise, read on…

*   **Note: Steven recorded a screen capture of this process, at
    <https://www.youtube.com/watch?v=3E2TZjTXgU8>**
*   The screen capture covers many of the steps here, except that the final push
    is blocked by branch settings (see below).

## Prerequisite

    Send an email to the ICU core list announcing that you are going to perform
    this task. Ensure that you have appropriate consensus before proceeding.

    Note that we assume `upstream` is the master fork of ICU.

    Be sure the
    [commit-checker](https://github.com/unicode-org/icu/tree/master/tools/commit-checker)
    is installed and functional.

*   (Note: this guide depends on steps not available in the commit-checker yet.
    See [ICU-20233](https://unicode-org.atlassian.net/browse/ICU-20233) and
    [Pull Request #473](https://github.com/unicode-org/icu/pull/473)

## Preparation

### Online

*   Ensure CI builds are clean
*   check review status

### Workspace

Make sure you aren’t in the middle of anything (uncommitted files, etc.)

Clear any `am`/`rebase` that may already be underway:

Checkout proper target branch:

Update your tree:

## Apply patches from the PR

Apply external patches, cleaning up trailing whitespace. Replace 99999 with the
PR #.

If there is a merge conflict you can try *one* of these options:

    OPTION A: Reset with `git am --abort` and then try a 3-way merge with the
    `curl -L https://github.com/unicode-org/icu/pull/99999.patch | git am
    --whitespace=fix -3`

    OPTION B: fix the merge yourself

    OPTION C: contact the PR author and ask them to rebase.

## Verify

At this point, verify the commit(s):

Diff to see the actual contents, that they are as expected:

## Test?

*   If in doubt, or At any step below, especially after a rebase, you can run
    ICU's tests locally.

## Fix Commits

### Rebase/Squash

Here's where you can squash commits (if need be) and edit any commit messages.
`--autosquash` will attempt to squash any commits as requested by the author.
(Note: I recommend a tool such as
[interactive-rebase-tool](https://gitrebasetool.mitmaro.ca/) for the best
git-rebase experience.)

    Change any lines from `pick` to `reword` if you need to change the commit
    message.

    To squash commits together, change lines from `pick` to `squash`. *(You
    can't squash the first commit, leave that one as `reword` or `pick`.)*

    Save and quit the editor

### Reword

Now you will get another editor for each commit you are editing.

*   Make sure the 1st line has this form, where there's a space after the
    number:

> `ICU-00000 `*`Some fix…`*

*   Make sure the 2nd line is blank
*   Break up long lines

Change the text to match conventions, and save and quit.

## Check commits

### Eyeball check

Check to see that the updated commits are as expected. Note that the original
author's name is preserved, but the commit hash has changed.

### Automated check

If the automated check fails, or you don't like the results, you can always run
`git rebase -i` again to `reword` a commit.

## Land it

Double check that no steps were skipped..

*   You will need to disable branch protection, see
    <https://help.github.com/en/articles/enabling-required-status-checks> (you
    must be a repository administrator to be able to do this.)
    *   It may be sufficient to disable the "Include administrators" button, to
        keep protection for most users of the branch.
*   Now you are ready to land this plane with git push
*   Remember to turn the branch protection back on when you are done!

### Rejected push?

If someone else landed a commit while you were doing all of this, you will get a
`rejected… fetch first` message from git.

*   DO NOT RUN `git pull` HERE!!

If you run `git pull` you will create a merge commit. And you really don't want
to do that.

Instead:

1.  Run `git pull upstream master --rebase` to rebase your local master
2.  Go back to
    [Verify](https://gist.github.com/srl295/10105f9ed9d48d2b87a5e4277b00077d#Verify)
    to check the state of your local master.
3.  You should then be able to push to master.

## Mark as closed

Close the PR with a message such as `Landed in
95526ca301fa9951a6b4bcc04b52431d423ccdc4`

# Help, I'm stuck in a rebase!

I run some of these when I get stuck in a rebase. This will destroy any local
work in your work tree, and get you back to a clean state.
