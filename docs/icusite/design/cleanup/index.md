# Cleanup

As a part of the switch-over to JDK 1.5, I think we also should clean up some of
the oldover methods and constants cruft. Examples:

*   HashTable
*   Vector
*   StringBuffer
*   ...
*   UTF16 stuff in:
    *   UCharacter
    *   UTF16
    *   ...
*   Others?

Some of this refactoring can be done fairly easily in Eclipse. Example:
The following perform the same functions.
UTF16.getCharCount(codePoint);
Character.charCount(codePoint);
To fix the situation, do the following:

1.  Change the implementation of UTF16 to just call the JDK.
2.  Copy the changed method
3.  Select the changed method, add @deprecated, and Rightclick>Source>Toggle
    Comment to comment it out.
4.  Paste the changed method.
5.  Select the changed method name, and Rightclick>Refactor>Inline.
6.  Uncomment the changed method.

This causes all of the instances in the code base (and we should do CLDR at the
same time), to change to use the JDK method, but leaves the old method active so
we don't break people.
I haven't investigated doing the JDK classes, but I think they can be done with
a search and replace of all instances in the code base that are not
public/protected parameters (even in the APIs, we might be able to broaden to
Map<X,Y>, etc. in some cases).
