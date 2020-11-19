# Resource Bundle Issues

Yoshito Umaoka sent the following to the ICU team on 2008-nov-05:

ICU resource bundle issues
Hi folks,
I was trying to fix some resource bundle issues and did some
investigations. Now I think there are several design issues in ICU
resource bundle implementation. I do not have any concrete plan for
making fixes/changes. For now I just want to bring some topics on table
and check with you if there are any misunderstandings/historic background
and etc.
1. Top level table vs. nested table
Locale bundles are organized based on locale inheritance model. Any
resources associated with a key is folded into the locale inheritance
hierarchy. A table is a collection of resource items associated with
keys. To access a resource item, you're supposed to use ures_getByKey to
locate the container resource. However, the behavior of ures_getByKey
with a top level table and a nested table is different. For a top level
table, locale inheritance is involved - that is, if a resource associated
with the given key is missing in the table, the implementation tries to
find a resource in its parent locale bundle. On the other hand, if
ures_getByKey is called against a nested table and a resource associated
with the given key is missing, an error code U_MISSING_RESOURCE_ERROR is
returned. To access a resource in nested table based on locale
inheritance, ures_getByKeyWithFallback must be used, but this is not a
public API.
For example -
----------------------
en {
month1 { "January"}
dow {
day1 { "Sunday" }
}
}
----------------------
en_US {
dow {
day2 { "Monday"}
}
}
----------------------
UResourceBundle \*bundle = ures_open(MYBUNDLE, "en_US", &status);
bundle = ures_getByKey(bundle, "month1", bundle, &status);
This code will return a UResourceBundle whose type is URES_STRING with
"January" (picked from bundle "en").
UResourceBundle \*bundle = ures_open(MYBUNDLE, "en_US", &status);
bundle = ures_getByKey(bundle, "dow", bundle, &status);
bundle = ures_getByKey(bundle, "day1", bundle, &status);
However, above code will return U_MISSING_RESOURCE_ERROR, because "dow" is
available in "en_US", but does not contain "day1".
I think there is no distinction between UResourceBundle returned by
ures_open (top level) vs. ures_getByKey (nested) in public API (isTopLevel
is set in the implementation although), the
difference of the behavior is not a good idea.
Steven told me that there was clear distinction between top level bundle
and nested bundle when ICU resource bundle was originally developed. And
he pointed out that some ICU consumer do not want any locale fallback in
nested resource access. However, we agreed that the default behavior of
nested items should be "with fallback". From the design point of view,
the fallback behavior should be governed by the mode when a bundle is
opened - i.e. ures_open (with fallback) vs. ures_openDirect (with NO
fallback).
2. Resource iteration
As I mentioned above, top level item look up is done with fallback.
However, iteration API - ures_getNextResource does not. From resource
bundle consumer's point of view, fallback should be transparent. In the
example above, "month1" resides in bundle "en". You can get the resource
by ures_getByKey with "month1" even you actually opened the bundle
"en_US". However, with ures_getNextResource, you cannot get there. If
the top level item does not fallback, it makes sense, but it does it.
3. Hierarchical key access
In our locale bundles, we organize the contents in hierarchical manner.
All items related to calendar is under "calendar". I think ICU consumer
also wants to organize their own resources in such way. With public APIs,
you have to navigate through the hierarchical node to reach to the actual
resource object, that is, you have to call ures_open first, then call
ures_getByKey to down to the desired leaf. I think we should have an API
locating a leaf object in one call, instead of repeative ures_getByKey.
For example, by specifying a hierarchical key -
"calendar/gregorian/dayNames/format/abbreviated", it should get to the
desired resource instead of calling ures_getByKey 5 times.
4. Alias resolution problem (ICU4J only?)
I mentioned this potential issue a while ago. Assume we have locale
bundles below -
----------------------
root {
calendar {
buddhist {
AmPmMarkers {"AM", "PM"}
dayNames:alias {"/LOCALE/calendar/gregorian/dayNames"}
}
}
}
----------------------
xx {
calendar {
gregorian {
dayNames {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"}
}
buddhist {
AmPmMarkers {"AA", "PP"}
}
}
----------------------
xx_YY {
calendar {
gregorian {
dayNames {"NUS", "NOM", "EUT", "DEW", "UHT", "IRF", "TAS"}
}
}
----------------------
To get calendar/buddhist/dayNames, there are two ways to get there.
a) getWithFallback with a hierarchical key
ICUResouceBundle b = ICUResourceBundle.getBundleInstance(MYLOCALEBUNDLE,
new ULocale("xx_YY"));
b = b.getWithFallback("calendar/buddhist/dayNames");
b) getWithFallback by navigating resource hierarchy
ICUResouceBundle b = ICUResourceBundle.getBundleInstance(MYLOCALEBUNDLE,
new ULocale("xx_YY"));
b = b.getWithFallback("calendar");
b = b.getWithFallback("buddhist");
b = b.getWithFallback("dayNames");
The major difference is - if you go down to navigate the hierarchy one by
one (case b), "/LOCALE" in dayNames:alias (root) is interpreted as locale
name "xx", while it is resolved as "xx_YY" with the single hierarchical
key "calendar/buddhist/dayNames" (case a). This is because the orignal
context for opening a bundle is not carried when you open a bundle one by
one.
5. ICU4J UResourceBundle extending java.util.ResourceBundle??
com.ibm.icu.util.UResourceBundle extends java.util.ResourceBundle. There
is one major design difference between these two classes - Java
ResourceBundle does not support hierarchical keys. On the other hand,
UResourceBundle supports key hierarchy. "get" in Java ResourceBundle
simply return a resource object, while "get" in UResourceBundle returns a
UResourceBundle.
By the contract, a subclass of Java ResourceBundle must implement Object
handleGetObject(String key). Resource bundle look up framework is a part
of Java ResourceBundle. So a subclass of JavaResourceBundle should be
just a container of resources (supporting string key map).
ICU4J UResourceBundle comes with its own bundle look up logic which is
independent from Java ResourceBundle. By the design of Java
ResourceBundle, a resource lookup is done by following "parent" chain.
But, with the current UResourceBundle implementation, "parent" always
point to its parent bundle's top level table. If a .res file has flat
structure (all keys belong to the top level table), it should be
equivalent to Java ResourceBundle. However, by the nature of
UResourceBundle, it usually has nested tables. So the basic contraction
in Java ResourceBundle is broken in UResourceBundle.
In my opinion, there is no benefit for implementing UResourceBundle as a
subclass of java.util.ResourceBundle for the reasons above.
UResourceBundle duplicates and extends the resource lookup framework
defined by Java ResourceBundle. Even someone wants to use .res via
java.util.ResourceBundle (for example, Java 6 allows you to implement your
own resource format using ResourceBundle.Control), it does not work quite
well unless .res files only contain top level keys.
6. ICU4J UResourceBundle (more specifically, ICUResourceBundle)
performance issues
Actually, this is the reason why I started look into ICU resource bundle
architecture closely. I think there are several implementation issues.
- Contents of .res is copied into byte\[\] when a bundle object is created.
- A new Java Object is created when a resource is requested.
- Keys are represented by ASCII bytes in byte\[\] and resource lookup is
done by b-tree search. (compact, but much slower than HashMap)
On the other hand, java.util.ListResourceBundle does followings -
- Java objects for keys/values are created when a bundle class is loaded.
- A reference of a Java Object is returned when a resource is requested.
- Keys are interned and HashMap is used for resource look up.
In ICU locale bundle, there are many string resources. When a string
resource is requested, it eventually copies bytes for creating String
object. Before ICU 4.0, UResourceBundle creates a new String object every
time for a same resource. We introduced bundle cache for tables at every
hierarchical level using HashMap in 4.0, so it no longer creates new Java
Object if the type is immutable. However, with this implementation,
UResourceBundle objects themselves are cached including various fields
other than actual resource data. Some fields are actually depending on
loading context - therefore, the implementation intentionally excludes a
ICUResourcebundle instance loaded with non-default options, or cached
incorrectly by an implementation bug and causes some problems. Also, even
all of resources are loaded into the caches, the byte\[\] keeping the entire
.res contents is never released. So technically, the implementation keep
duplicated data (one in the bundle caches, another in byte\[\]) in Java
heap.
To reduce the memory footprint, I was thinking about the use of memory
mapped file like ICU4C does. But, the contents of .res is loaded as a
resource stream via Java class loader - that is, it is not a regular file.
For this reason, we cannot use java.nio.MappedByteBuffer.
I think the right thing to do is (if we continue to use .res) -
- Contents of .res should be transformed Java Objects (for example, text
data -> String, int vector -> int\[\]...) when a bundle is created. Also,
String objects for keys are created/interned at the same time.
- Release byte\[\] holding the raw .res data. (Ideally, we want to read
.res as stream and transform the contents into native Java Object
structure in serialized manner. But .res format is not designed to
support stream parsing and it requires to load the entire cotents into
byte\[\] once...)
- UResourceBundle/ICUResourceBundle should work like a simple cursor for
navigating Java resource objects loaded above.
- HashMap is created when a resource is requested in a table resource.
By the implementation above, it has to pay one time cost at the loading
time. However, it should not be much different from Java standard
resource types (loading a ResourceBundle class also creates Java Objects
from bytecode.)
7. Resource accessor method for mutable resource types in ICU4J
UResourceBundle
UResourceBundle has some methods returning mutable Java types below -
public ByteBuffer getBinary()
public byte\[\] getBinary(byte\[\] ba)
public int\[\] getIntVector()
public String\[\] getStringArray()
Technically, resources are read-only and should not be modifiable. So,
with these method signatures, the implementation must create a new array
every time. Even we want to cache a resource object, it should be cloned
when the resource is accessed via these methods. This is actually a
problem of Java language (no const array type). I think we should add
read-only array wrapper type (and read-only ByteBuffer for the first one)
and use these types.
In addition to this, I think "public byte\[\] getBinary(byte\[\] ba)" does not
make sense. Even you provide fillin arg - byte\[\] ba, it cannot be used if
the size of bytes does not match the given byte\[\]. I think it was ported
from C, which takes length for input/output arg.
8. Bad APIs in ICU4J UResourceBundle
This is actually my fault. Just before Ram left the proejct, he was
working on moving some internal APIs (ICUResourceBundle) to public
(UResourceBundle). He forgot to assign some API status comments and I
added them without understanding the entire picture. Now I think I should
not have made following APIs public (I meant - public APIs, the signature
is actually "protected"). These APIs takes HashMap as an argument to
avoid cyclic references caused by invalid ALIAS, which is really our
internal implementation.
protected UResourceBundle handleGet(String aKey,
HashMap table,
UResourceBundle requested)
protected UResourceBundle handleGet(int index,
HashMap table,
UResourceBundle requested)
