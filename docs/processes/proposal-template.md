# API Proposal Email Template

to: icu-design <icu-design@lists.sourceforge.net> \[requires
[subscription](../contacts.md)\]
subject: ICU API proposal: **<topic>**

---

Dear ICU team & users,
I would like to propose the following API for: **ICU 63**

Please provide feedback by: **next Tuesday, 2018-mm-dd**
Designated API reviewer: **Markus**

Ticket: https://unicode-org.atlassian.net/browse/ICU-<number>

---

**<detailed API description with exact function signatures>**

---

***Example of detailed API***

I would like to propose the following C++ Locale method for ICU 50:
/\*\*
\* Set the value for a keyword.
\*
\* @param keywordName name of the keyword to be set. Case insensitive.
\* @param keywordValue value of the keyword to be set. If 0-length or
\* NULL, will result in the keyword being removed. No error is given if
\* that keyword does not exist.
\* @param status Returns any error information while performing this operation.
\*
\* @draft ICU 50
\*/
void setKeywordValue(const char\* keywordName, const char\* keywordValue,
UErrorCode &status);
\[Optional: More background info, link to design doc, etc.\]
