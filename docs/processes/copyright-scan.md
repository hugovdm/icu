# ICU Copyright Scanner

## What is the copyright scanner?

---

ICU requires all files checked in to have either one of the following Unicode
copyright statement. Usually this is in a comment at the top of the file. The
scanner is looking for this statement within the first few lines of file data.

ASCII:

Copyright (C) 2016 and later: Unicode, Inc. and others.
License & terms of use: http://www.unicode.org/copyright.html#License

UTF-8:

© 2016 and later: Unicode, Inc. and others.
License & terms of use: http://www.unicode.org/copyright.html#License

A command-line version of the copyright scanner can also be run manually, from
<http://source.icu-project.org/repos/icu/trunk/tools/scripts/cpysearch/>

**cpyscan.pl** might be used for verifying copyright comments as a part of
release preparation. The script loads cpyskip.txt located in the current
directory. If cpyskip.txt is not found locally, it will use the online version
(<http://source.icu-project.org/cpyskip.txt>) instead.

*   Note: cpy**search** attempts to find items with out of date copyright.
    However, cpy**search** will not produce useful results because the date of
    files on disk cannot be depended on.
    Ignore cpy**search**, and try to use cpy**scan** instead.

### Exclusion list

If a file can **not** contain a copyright, like a binary file, simply add the
file's path to the file
`svn+ssh://source.icu-project.org/repos/icustuff/trunk/src/svnhooks/cpyskip.txt`,
and check in the file. Changes to this file take effect every 15 minutes, on the
quarter hour. You may have to wait for the changes to take effect. After
updating cpyskip.txt, you will be able to check in your new file.

The latest copy that the server is using is viewable as
<http://source.icu-project.org/cpyskip.txt>

### Exclusion list syntax

The following are some example formats that can be used in cpyskip.txt

#### Comment

`# This is a comment. I love comments.`

#### Path

`icu/source/extra/uconv/samples/utf8/chinese-ulysses.txt`

#### Suffix match

A full file path, starting with the module (icu). It will match files which end
with ".jar". (Note, it is not a full regular expression.)

`*.jar`

#### Prefix match

`icu4j/eclipseProjectMisc/*`

It will exclude all files under icu4j/eclipseProjectMisc.
