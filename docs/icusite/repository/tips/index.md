# Tips (for developers)

[TOC]

## C/C++ workspace structure

It is best to keep the source file tree and the build-output files separate
("out-of-source build"). It keeps your source tree clean, and you can build
multiple configurations from the same source tree (e.g., debug build, release
build, build with special flags such as no-using-namespace). You could keep the
source and build trees in parallel folders.

**Important:** If you use runConfigureICU together with CXXFLAGS or similar, the
==*custom flags must be before the runConfigureICU invocation*==. (So that they
are visible as environment variables in the runConfigureICU shell script, rather
than just options text.) See the sample runConfigureICU invocations below.

See the ICU4C readme's [Recommended Build
Options](https://htmlpreview.github.io/?https://github.com/unicode-org/icu/blob/master/icu4c/readme.html#RecBuild).

For example:

*   `~/icu/mine/**src**`
    *   source tree including icu (ICU4C) & icu4j folders
    *   setup: mkdir + git clone your fork (see the [Linux Tips
        subpage](linux.md)) + cd to here.
    *   Use `git checkout <branch>` to switch between branches.
    *   Use `git checkout -b <newbranchname>` to create a new branch and switch
        to it.
    *   After switching branches, remember to update your IDE's view of the
        source tree.
    *   For C++ code, you may want to `make clean` *before* switching to a
        different branch.
*   `~/icu/mine/icu4c/**bld**`
    *   release build output
    *   not-using-namespace is always recommended
    *   setup: mkdir+cd to here, then something like
        `CXXFLAGS="-DU_USING_ICU_NAMESPACE=0"
        CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1"
        ../../src/icu4c/source/**runConfigureICU** Linux
        --prefix=/home/*your_user_name*/icu/mine/inst > config.out 2>&1`
    *   build: `make -j5 check > out.txt 2>&1`
*   `~/icu/mine/icu4c/**dbg**`
    *   debug build output
    *   not-using-namespace is always recommended
    *   setup: mkdir+cd to here, then something like
        `CXXFLAGS="-DU_USING_ICU_NAMESPACE=0"
        CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1"
        ../../src/icu4c/source/**runConfigureICU** --enable-debug
        --disable-release Linux --prefix=/home/*your_user_name*/icu/mine/inst >
        config.out 2>&1`
    *   build: make -j5 check > out.txt 2>&1
    *   Be sure to test with gcc and g++ too! `CC=gcc CXX=g++
        CXXFLAGS="-DU_USING_ICU_NAMESPACE=0"
        CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1"
        ../../src/icu4c/source/runConfigureICU --enable-debug --disable-release
        Linux`
*   `~/icu/mine/icu4c/**nm_utf8**`
    *   not-using-namespace and default-hardcoded-UTF-8
    *   setup: mkdir+cd to here, then something like
        `../../src/icu4c/source/**configure**
        CXXFLAGS="-DU_USING_ICU_NAMESPACE=0" CPPFLAGS="-DU_CHARSET_IS_UTF8=1
        -DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1"
        --prefix=/home/*your_user_name*/icu/mine/inst > config.out 2>&1`
*   ~/icu/mine/icu4c/static
    *   gcc with static linking
    *   setup: mkdir+cd to here, then something like
        `../../src/icu4c/source/**configure**
        CXXFLAGS="-DU_USING_ICU_NAMESPACE=0"
        CPPFLAGS="-DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1 -O2 -ffunction-sections
        -fdata-sections" LDFLAGS="-Wl,--gc-sections" --enable-static
        --disable-shared --prefix=/home/*your_user_name*/icu/mine/inst >
        config.out 2>&1`
*   `~/icu/mine/`**`inst`**
    *   “make install” destination (don’t clobber your platform ICU during
        development)
*   `~/icu/**msg48**/src`
    *   Optional: You could have multiple parallel workspaces, each with their
        own git clones, to reduce switching a single workspace (and the IDE
        looking at it) from one branch to another.

### Run individual test suites

*   `cd ~/icu/mine/icu4c/dbg/test/intltest`
    *   `export LD_LIBRARY_PATH=../../lib:../../stubdata:../../tools/ctestfw`
    *   `make -j5 && ./intltest utility/ByteTrieTest utility/UCharTrieTest`
*   `cd ~/icu/mine/icu4c/dbg/test/cintltst`
    *   same relative `LD_LIBRARY_PATH` as for intltest
    *   `make -j5 && ./cintltst`

## Linux Tips

For more Linux-specific tips see the [Linux Tips subpage](linux.md).

## Merging and Branching

See the [git for ICU Developers](../gitdev/index.md) page.

## Obsolete svn branch workflow

Note: work on your workspace from the 'icu' or 'icu4j' level, not from the
'icu/source' or 'icu4j/src' level.

### Obsolete: Making a branch

This makes a branch on the subversion server. It does not use or modify your
workspace. It branches both icu4c and icu4j.
`# (The following command should be typed as one line. )`
` svn copy svn+ssh://source.icu-project.org/repos/icu/trunk`
` svn+ssh://source.icu-project.org/repos/icu/icu/branches/*user*/somebranch`

### Obsolete: Working on the branch (ICU4C)

Use "svn info" to determine what path a certain workspace is connected to.
You can check out the branch into an entirely new workspace locally:
` svn co
svn+ssh://source.icu-project.org/repos/branches/*user*/somebranch/icu4c`
Or, if you want an existing workspace to be connected to a branch, you can
switch its URL.
Any local changes against trunk will become changes against the branch.
` svn switch ^/branches/*user*/somebranch/icu4c `
Commits, etc can be done in any subdirectory of the workspace.

### Obsolete: Merging a branch back onto the trunk

**If you are very confident that the trunk hasn't changed substantially since
you branched off**, you can simply merge all changes made on the branch, back
into the trunk. Otherwise, bring the branch up to date (see the next section)
before merging back to the trunk (this section).
First, make sure all changes, including new files, have been checked into the
branch. If you can, test the branch on other platforms before merging.
Now, enter a directory containing the latest up-to-date trunk. You can use 'svn
switch' with the trunk URL (as above) to switch the branch workspace, if
desired.
` svn switch ^/trunk/icu4c`
Here is a command to do the dry run (to see what will be merged and check for
errors/conflicts). Take out --dry-run to make it stick.
`svn merge --dry-run ^/branches/*user*/somebranch/icu4c`
Note, you'll need to resolve any conflicts that occur.
Now, you can review diffs, test and check in your revised working copy of the
trunk.

### Obsolete: Keeping the branch up to date

If the branch is unmerged for any length of time, you will want to merge in the
latest from the trunk, and commit it onto the branch, before attempting to merge
it back onto the trunk.
First, check in all changes on the branch, and make sure it builds cleanly. You
can commit merges and other changes at the same time, but it's probably better
to have a commit just be one or the other, in case you need to undo something.
Now, merge the trunk into your local workspace
svn merge ^/trunk/icu4c`
Deal with any conflicts that arise here, try a build out, etc.
Then, you can commit this merge back to the trunk. Again, you can make manual
changes besides the merge, but you may want to keep track of them separately.
Repeat as needed..
