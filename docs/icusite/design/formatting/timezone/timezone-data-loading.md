# Timezone Data Loading

This is a proposal to support the loading of time zone data resources from
individual files in ICU4C, in configurations that do not otherwise support
loading data from separate files.

Please provide feedback by July 15, 2014.

Designated API Reviewer: Markus

Ticket: [10942](http://bugs.icu-project.org/trac/ticket/10942)

The intent is to permit the updating of time zone data separately from the rest
of the ICU data. When the option is enabled, ICU will check first in a separate
directory for time zone related resource files; if they are not found then the
normal ICU data loading conventions will be followed.

The option can be specified in two ways,

*   At build time, by defining the preprocessor variable U_TIMEZONE_FILES_DIR to
    be the desired absolute path.
*   At run time by setting the environment variable U_TIMEZONE_FILES_DIR. This
    will override any build time setting.

The option adds a directory path to be checked first when opening these four
resources:

> metaZones.res

> timezoneTypes.res

> windowsZones.res

> zoneinfo64.res

If the stand-alone file(s) are not present the standard ICU resource data
loading mechanisms will be followed, including any options set by
udata_setFileAccess(). Any combination of the files can be supplied; for
example, zoneinfo64.res could be supplied separately, with the other three
resources being taken from the default ICU data file.

Setting U_TIMEZONE_FILES_DIR overrides UCONFIG_NO_FILE_IO, but for the purposes
of time zone data loading only.

Issue: No API makes testing harder. Maybe we should have an internal API to
allow testing without requiring a whole separate build of ICU.
