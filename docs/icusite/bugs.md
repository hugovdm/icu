# Submitting ICU Bugs and Feature Requests

**Note:** ICU uses [CLDR](http://cldr.unicode.org/) locale data. Please submit
feedback on locale data [there](http://cldr.unicode.org/index/bug-reports).

If you would like to submit an ICU bug report or feature request, or search the
ICU bug database, please visit the following link:

*   **[ICU JIRA page](https://unicode-org.atlassian.net/)**
*   Note: You need to have a Jira account for submitting a ticket or commenting
    on one.
*   Click the + icon on the left edge to submit a new ticket.
*   Click the 🔍 icon on the left edge to search for existing tickets.

For patches, please submit both an ICU Jira ticket and an [ICU
GitHub](https://github.com/unicode-org/icu) pull request. The pull request text
should start with "ICU-<Jira ticket number> ".

If you are encountering a build problem, encountering an API usage problem, or
you have a question about ICU, you will usually get a faster response by joining
our [icu-support mailing list](contacts.md) and asking there.

## Finding previous bugs

As of July 1, 2018, all tickets from the previous (ICU trac) ticket tracking
system have been converted over to issues on the [ICU
JIRA](https://unicode-org.atlassian.net/projects/ICU/summary) page. Due to
privacy concerns, email addresses of all tickets filed previously were not
carried over. If you want to look for your previous tickets, perhaps filed under
the email address “**myname**@example.com” you can use a URL like this:

*   ==https://unicode-org.atlassian.net/issues/?jql=tracReporter%20~%20**myname**==
    (but replace “**myname**” with the part of your email address before the @
    sign.)

## Ticket Lifecycle

*   Information about the ICU ticket process is on the [ticket
    lifecycle](processes/ticket-lifecycle.md) page.

### ~~Excel Spreadsheet View~~

~~An Excel spreadsheet view of the existing Trac Tickets can be found in
[traclist.xls](http://icu-project.org/repository/traclist.xls). *Control-u* will
update from the Trac database while in Excel.~~

~~For OpenOffice, because of the use of macros, the above Excel spread sheet will not work. The actual data, as tab-delimited text, comes from this URL: <http://bugs.icu-project.org/trac/report/12?format=tab>~~
~~It can be inserted into an OO spreadsheet using the Insert->Sheet From File menu, and entering the URL.~~
