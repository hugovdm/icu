# Obsolete: Pre-GitHub Code reviews

The following became obsolete in 2018-July when we moved to git on GitHub.

Typically we commit changes and set a reviewer in the ticket. The reviewer
follows the "Review" link on the Trac ticket page to find the associated
changesets and their diffs. ==The problem is== that reviews are often done weeks
or months after the changes, the author is out of context, and feedback is done
via copy/paste into email.

Sometimes we commit changes to branches and iterate with a reviewer before
merging the complete work onto the trunk. This makes for better timing and
feedback.

*Ideally, we would get code reviewed before committing it*, and make it easy to
refer to a line of code and add comments to it. Code reviews would block
commits, but that would motivate us to get them done quickly. Experience shows
that easy and immediate feedback leads to more thorough reviews and better code.

**Rietveld** is an online code review tool that lets us do pre-commit code
reviews: <http://code.google.com/p/rietveld/>

It takes a little thought and setup to send a change for code review, but it's
really easy for the reviewer.

## Update October 2017

(Shane) I have made a couple contributions to the Rietveld project, but as of
yet they haven't been merged.
[#556](https://github.com/rietveld-codereview/rietveld/pull/558) fixes a bug
arising from a recent update to SVN, and
[#558](https://github.com/rietveld-codereview/rietveld/pull/558) adds a new
--svn_changelist flag to the tool to allow you to upload only a certain
changelist for review in the tool. The changelist feature allows you to select
only certain files for review and commit, which can be helpful if you have
multiple changes in the same client or if you have local changes that you don't
want to commit, such as IDE settings.

## Update May 2015

(Andy) Update from below:

The update.py script has been updated. Download the latest from
<http://codereview.appspot.com/static/upload.py>

--internal-diff is now the default, meaning that no diff options should be
required.

Password authentication no longer seems to work, but the --oauth2 option to
upload.py works well, at least for google accounts. When using OAuth2 the script
will open a browser window asking permission to use your account to send mail.

The command I use for reviewing changes from an svn branch:

<pre><code>
cd <i>dir-with-committed-changes</i>
upload.py --send_mail --oauth2 -m "<i>description of changes</i>" --rev=<i>revision-of-branch-creation</i>:HEAD -r <i>reviewer1@gmail.com,reviewer2@gmail.com...</i>
</code></pre>

## Update 2014

(Markus) Update from below:

First, when using svn, make sure that the svn diff program is the default
text-to-stdout diff. upload.py should use the --internal-diff option but does
not, and will create empty diffs if the svn diff program is a graphical one.

*   You can add " -- diff_options" to the upload.py command line.
    *   With a reasonably modern svn, you can append " -- --internal-diff".
    *   If your svn is old enough to not understand --internal-diff, you should
        be able to use " -- --diff-cmd diff".
*   You could temporarily modify your ~/.subversion/config file by commenting
    out your custom diff-cmd.

When specifying the base URL, it is probably better to use http:// rather than
svn+ssh:// to avoid requiring authentication.

You need to be a registered user of Rietveld.

It is apparently not possible any more to create a new issue from the Rietveld
web UI. You must use upload.py.

Sample actual Linux command line for **creating a code review issue from
committed svn branch changes** for [ticket
#10939](http://bugs.icu-project.org/trac/ticket/10939) "ICU4J optionally read
data from .dat and .res": (I obfuscated the reviewer email address below.)

```none
~/svn.icu4j/trunk/src$ ~/Downloads/rietveld/upload.py   -t 'ICU ticket #10939: ICU4J optionally read data from .dat and .res'   --base_url http://source.icu-project.org/repos/icu/icu4j/branches/markus/mapdata/   -r reviewer@example.com   -m 'branch code complete at r36098'   --send_mail   --rev 36076:36098
```

For some reason, this takes a few minutes. It will eventually ask for your
Rietveld user email address & password.

This created Patch Set 1 of <http://codereview.appspot.com/121870043>

I then made code review changes in r36102 and **updated the review issue** with
the following. Note that this time I provided the issue number (-i) and the
--internal-diff option, and I dropped the issue title (-t) and reviewer (-r)
options to keep those as is.

```none
~/svn.icu4j/trunk/src$ ~/Downloads/rietveld/upload.py   -i 121870043   --base_url http://source.icu-project.org/repos/icu/icu4j/branches/markus/mapdata/   -m 'code review changes r36102'   --send_mail   --rev 36076:36102   -- --internal-diff
```

This created Patch Set 2 of <http://codereview.appspot.com/121870043>

Sample actual Linux command line for **creating a code review issue from pending
trunk changes** (not committed yet) for [ticket
#11341](http://bugs.icu-project.org/trac/ticket/11341)
"getByteBufferFromInputStream() can be very inefficient": (I obfuscated the
email addresses below but kept the diff options.)

```none
~/svn.icu4j/trunk/src$ ~/Downloads/rietveld/upload.py   -t 'ICU ticket #11341: getByteBufferFromInputStream() can be very inefficient'   --base_url http://source.icu-project.org/repos/icu/icu4j/trunk   -r yoshito_umaoka@example.com   --cc andy.heninger@example.com   -m 'check InputStream.read()<0 for end of stream, not available()==0'   --send_mail   -- --internal-diff
```

This created Patch Set 1 of <http://codereview.appspot.com/155570043>

==ICU4C== example for **creating a code review issue from pending trunk
changes** (not committed yet):

```none
~/svn.icu/trunk/src$ ~/Downloads/rietveld/upload.py   -t '[ICU ticket #11336](http://bugs.icu-project.org/trac/ticket/11336): UnicodeString: recombine stack buffer arrays'   --base_url http://source.icu-project.org/repos/icu/icu/trunk   -r andy.heninger@gmail.com   --cc srl@icu-project.org   -m 'recombine stack buffer arrays; one int16_t for short length & flags'   --send_mail   -- --internal-diff
```

--> <http://codereview.appspot.com/178970043>

## Instructions 2012

The following is a write-up from Maxime Serrano, a Google intern hosted in the
summer of 2012 by Markus Scherer:

> My usage of rietveld

> Rietveld is a code review tool with a relatively simple interface. It allows
> users to upload commits-in-progress for review, and reviewers to attach
> comments to specific lines of a diff. \[...\]

> The interface for creating a commit is very simple: rietveld provides a python
> script, called upload.py, which you simply run from within the root of your
> checkout. Effectively, if I've checked out the ICU trunk into `~/icu`, I would
> want to:

> `$ cd ~/icu`

> before running the script. In general I keep upload.py in my home directory,
> so that running it is as simple as

> `$ python ~/upload.py`

> The script will then run the appropriate diff command (generally, git diff or
> svn diff) and create a changeset for you. It will prompt you for a name for
> your changeset, as well as for an AppEngine username (or gmail address) and
> password. From there, it should produce a link for you that your changeset
> will be there.

> If this does not work - and it is occasionally spotty - there exists an
> interface within rietveld that will accept a more general diff file. To use
> it, simply run:

> `$ svn diff > out.diff`

> or

> `$ git diff > out.diff`

> and navigate to the "Create issue" tab of rietveld in order to upload the
> diff. rietveld will ask you for a "base URL." This should be the URL that you
> checked the project out from - unless this is not a URL that rietveld can
> check out without permissions, in which case you should provide the "public"
> checkout URL.

> For example: when I check out a copy of ICU that I want write permissions
> with, I would use the URL
> `svn+ssh://source.icu-project.org/repos/icu/icu/trunk` . However, rietveld
> does not have my private key - and so it cannot authenticate to this URL.
> Therefore, I have to provide it with the public version:
> `http://source.icu-project.org/repos/icu/icu/trunk` - in order for it to be
> able to properly download the base files and display the diff.

> Once you have created a first diff, if you'd like to respond to comments, you
> can simply upload a new changeset to the same rietveld issue. To do so,
> navigate to your checkout root again, and then run:

> `$ python ~/upload.py -i <number>`

> where <number> is the number of your issue. It can be found in the URL of your
> issue: for example, the issue at <http://codereview.appspot.com/**6443126**/>
> has issue number 6443136.

> upload.py will prompt you for a name for this changeset - this does not need
> to be the same as the name of the issue. From there, your reviewers will be
> able to add comments to the updated diff (and see the differences between this
> diff and the previous one) as normal.
