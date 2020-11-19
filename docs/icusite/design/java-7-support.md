# Java 7 Support

### Category Locale

Java 7 added default locales for formatting and display
\[http://download.oracle.com/javase/7/docs/technotes/guides/intl/enhancements.7.html#category\].
There are two new APIs in java.util.Locale class below.

---

public static
[Locale](http://download.oracle.com/javase/7/docs/api/java/util/Locale.html)
getDefault([Locale.Category](http://download.oracle.com/javase/7/docs/api/java/util/Locale.Category.html)
category)

Gets the current value of the default locale for the specified Category for this
instance of the Java Virtual Machine.

The Java Virtual Machine sets the default locale during startup based on the
host environment. It is used by many locale-sensitive methods if no locale is
explicitly specified. It can be changed using the setDefault(Locale.Category,
Locale) method.

Parameters:`category` - - the specified category to get the default
localeReturns:the default locale for the specified Category for this instance of
the Java Virtual
MachineThrows:`[NullPointerException](http://download.oracle.com/javase/7/docs/api/java/lang/NullPointerException.html)`
- - if category is nullSince:1.7See Also:[`setDefault(Locale.Category,
Locale)`](http://download.oracle.com/javase/7/docs/api/java/util/Locale.html#setDefault%28java.util.Locale.Category,%20java.util.Locale%29)

---

public static void
setDefault([Locale.Category](http://download.oracle.com/javase/7/docs/api/java/util/Locale.Category.html)
category,
[Locale](http://download.oracle.com/javase/7/docs/api/java/util/Locale.html)
newLocale)

Sets the default locale for the specified Category for this instance of the Java
Virtual Machine. This does not affect the host locale.

If there is a security manager, its checkPermission method is called with a
PropertyPermission("user.language", "write") permission before the default
locale is changed.

The Java Virtual Machine sets the default locale during startup based on the
host environment. It is used by many locale-sensitive methods if no locale is
explicitly specified.

Since changing the default locale may affect many different areas of
functionality, this method should only be used if the caller is prepared to
reinitialize locale-sensitive code running within the same Java Virtual Machine.

Parameters:`category` - - the specified category to set the default
locale`newLocale` - - the new default
localeThrows:`[SecurityException](http://download.oracle.com/javase/7/docs/api/java/lang/SecurityException.html)`
- - if a security manager exists and its checkPermission method doesn't allow
the
operation.`[NullPointerException](http://download.oracle.com/javase/7/docs/api/java/lang/NullPointerException.html)`
- - if category and/or newLocale is nullSince:1.7See
Also:[`SecurityManager.checkPermission(java.security.Permission)`](http://download.oracle.com/javase/7/docs/api/java/lang/SecurityManager.html#checkPermission%28java.security.Permission%29),
[`PropertyPermission`](http://download.oracle.com/javase/7/docs/api/java/util/PropertyPermission.html),
[`getDefault(Locale.Category)`](http://download.oracle.com/javase/7/docs/api/java/util/Locale.html#getDefault%28java.util.Locale.Category%29)

---

Here are some technical summaries.

*   DISPLAY and FORMAT category default are separated from the existing default
    locale. getDefault() / setDefault(Locale) remain unchanged.
*   setDefault(Locale) also sets the specified Locale to DISPLAY/FORMAT category
    default, but setDefault(Category, Locale) does not change default locales
    other than the specified category default.
*   The default DISPLAY locale is used by methods returning localized display
    names in Java i18n service classes (typically getDisplayXXXX). More
    specifically -
    *   TimeZone#getDisplayName
    *   Currency#getSymbol
    *   Currency#getDisplayName
    *   Locale#getDisplayLanguage
    *   Locale#getDisplayCountry
    *   Locale#getDisplayVariant
    *   Locale#getDisplayName
    *   Locale#getDisplayScript (This should use DISPLAY, but not now. This is
        obviously a bug in Java 7)
*   The default FORMAT locale is used by creating Java i18n service object for
    formatting date (including calendar), number and currency. More
    specifically, constructors/factory method in following classes -
    *   Calendar and its subclasses
    *   Formatter
    *   DateFormat/SimpleDateFormat/DateFormatSymbols
    *   NumberFormat/DecimalFormat/DecimalFormatSymbols
    *   MessageFormat
*   The core locale sensitive services as in java.lang package classes, Collator
    and BreakIterator continue to use the existing default. There is no category
    for these services.

ICU4J default locale is synchronized with underlying JVM default locale.
However, ICU4J 4.8.1 does not support the classification of these category
locales. To support Java 7, it is necessary to add the parallel APIs and
synchronize the category locales with Java 7 runtime. The support problem is
reported by ICU Trac[#8630](http://bugs.icu-project.org/trac/ticket/8630)
Followings are proposed APIs.

---

/\*\*
\* Enum for locale categories. These locale categories are used to get/set the
default locale for
\* the specific functionality represented by the category.
\* @stable ICU 49
\*/
public enum Category {
/\*\*
\* Category used to represent the default locale for displaying user interfaces.
\* @stable ICU 49
\*/
DISPLAY,
/\*\*
\* Category used to represent the default locale for formatting date, number
and/or currency.
\* @stable ICU 49
\*/
FORMAT
}

---

/\*\*
\* Returns the current default ULocale for the specified category.
\*
\* @param category the category
\* @return the default ULocale for the specified category.
\* @stable ICU 49
\*/
public static ULocale getDefault(Category category)

---

/\*\*
\* Sets the default <code>ULocale</code> for the specified
<code>Category</code>.
\* This also sets the default <code>Locale</code> for the specified
<code>Category</code>
\* of the JVM. If the caller does not have write permission to the
\* user.language property, a security exception will be thrown,
\* and the default ULocale for the specified Category will remain unchanged.
\*
\* @param category the specified category to set the default locale
\* @param newLocale the new default locale
\* @see SecurityManager#checkPermission(java.security.Permission)
\* @see java.util.PropertyPermission
\* @stable ICU 49
\*/
public static synchronized void setDefault(Category category, ULocale newLocale)

---

Note: These new APIs are marked as @stable, because they are parallel to JDK
APIs and we won't change them.
In ICU4J, DISPLAY/FORMAT default should be used in Java compatible APIs as well
as some ICU specific classes/methods for formatting and returning display names.
The spreadsheet below shows all the call sites of ULocale.getDefault() in trunk
and proposed changes.

#### ICU4J Category Locale Support

<iframe
src="https://spreadsheets.google.com/spreadsheet/loadredirect?chrome=false&key=0AjD1-LizLRWldDF6MEc2OXpvZ3pVM0p2RDFJeGxnRVE&output=html&pubredirect=true&widget=true"
width="100%" height="600" allow="fullscreen" />

### Critical Bug Fixes

*   [#8734](http://bugs.icu-project.org/trac/ticket/8734) icu cannot load under
    certain java7 locales
