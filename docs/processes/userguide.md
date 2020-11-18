# ICU Userguide

Located at <http://userguide.icu-project.org>

We might copy the entire userguide to a new location for each release, so we
save a 'snapshot'.

To mirror the entire site using [wget](http://www.gnu.org/software/wget/), you
can use the following command. The results won't be very pretty (stylesheets are
missing, etc), but it will be self contained.

*   wget -N -m -k http://userguide.icu-project.org
*   This does not seem to work very well: For pages that have sub-pages, the
    parent page is replaced by a content-less folder, and only the contents of
    the sub-pages are preserved.
*   This method does seem to work well for the **API references**:
    *   wget -N -m -k http://www.icu-project.org/apiref/icu4c/
    *   wget -N -m -k http://www.icu-project.org/apiref/icu4j/

A better method seems to be using the Google Sites API, for example via the Java
tool from the Google Data Liberation team:

*   <http://googlesystem.blogspot.com/2009/09/export-google-sites.html>
*   <http://code.google.com/p/google-sites-liberation/>
    *   (See the Downloads & Wiki tabs)
*   <http://www.google.com/support/forum/p/sites/thread?tid=6381dcf34c61570d&hl=en>
*   For me, on Linux/Gnome, double-clicking the .jar file did not work. I
    invoked the GUI app via
    *   java -cp ~/Downloads/google-sites-liberation-1.0.3.jar
        com.google.sites.liberation.util.GuiMain
    *   Host: Leave the default sites.google.com
    *   Webspace: icuprojectuserguide
    *   This requires a Google account login. It ==might== work only for Site
        owners or editors. (I have not tried a non-owner account.) \[markus\]
*   It is possible to use the command-line version of the tool for regular
    backups.
*   You can use the same tool to import the files into another Google Site, for
    example to create a mirror Site. (Choose your domain, if any, your Site, and
    your domain's Google Account login.)
    *   ==*This replaces the target Site's contents, rather than merging it.*==
