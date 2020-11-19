# ICU4C Footprint

ICU4C can be configured to different sizes, based on the functionality that is
needed. The largest part of ICU is the data for different countries and charset
converters, so the easiest way to reduce the size is to use configuration
switches to remove data that is not needed by the particular implementation.

For a description of how to reduce the size of ICU4C, see [Customizing ICU's
DataLibrary](http://userguide.icu-project.org/icudata#TOC-Customizing-ICU-s-Data-Library).
For corresponding information on ICU4J (including sizes), see [How to Modularize
ICU4J](http://source.icu-project.org/repos/icu/trunk/icu4j/readme.html#HowToModularize).
The [Data Library Customizer](http://apps.icu-project.org/datacustom/) can also
be used to modularize the data for the latest version of ICU.

### Data

Here are the sizes of the main data file, icudt30.dll on Windows, when certain
configuration files are removed or renamed.

8.17 MB normal
7.98 MB trnsfiles.mk
7.00 MB *... and remove* ucmebcdic.mk
5.43 MB *... and remove* ucmfiles.mk
4.38 MB *... and remove* ucmcore.mk
2.88 MB *... and remove* resfiles.mk
1.18 MB *... and remove* colfiles.mk
1.06 MB *... and remove* miscfiles.mk

Here are the sizes of the remaining items that are not easy to remove without
editing makefiles, but which can be done if necessary.

354 KB (363,358 bytes) UCA collation (invuca.icu + ucadata.icu + coll/\*.res)
341 KB (349,258 bytes) uprops.icu + unorm.icu + unames.icu + pnames.icu
288 KB (295,024 bytes) Break iteration (\*.brk)
34.3 KB (35,140 bytes) root.res + res_index.res
27.3 KB (28,030 bytes) cnvalias.icu
20.2 KB (20,714 bytes) uidna.spp
5.06 KB (5,188 bytes) ibm-1047 + ibm-37

### Code

If you would like to see other code changes that can be done to make ICU4C
smaller, please read the [Packaging
ICU](http://userguide.icu-project.org/packaging.html) section of the User's
Guide.

If you are only using small portions of ICU, you can link against the static
form of the ICU libraries. You need to make sure that you specify the
--enable-static configure option when building ICU. Statically linking against
ICU will allow the linker to only bring in the code that is being used by your
application, but it will make it more difficult to upgrade or share your copy of
ICU that you provide with your application.

### Notes

*   The minimal configuration above, which contains the remaining items, will
    work for people that need BiDi, normalization, IDNA, simple Unicode
    conversion, UnicodeSet, plain UCA collation and break iteration.
*   Also see an overview of [ storage used for different ICU
    services](http://source.icu-project.org/repos/icu/icuhtml/trunk/design/icu_footprint.html),
    and also (rough) [ plans for future
    modularization](http://source.icu-project.org/repos/icu/icuhtml/trunk/design/modularization/icu_modularization_overview.html).
