# Decimal Format construction performance

Objective

The Objective of this document is to layout a very high level proposal for
refactoring the ICU4C DecimalFormat. The purpose of the re-factoring is two fold

    To increase performance

    To make DecimalFormat code easier to understand and maintain

Background

Creating a DecimalFormat object can take over 50X longer than performing a
typical format operation even after the existing resource bundle cache is warmed
up. That means each time code creates a DecimalFormat object, it needs to use it
to format at least 50 numbers to justify the cost of creating it. On my multi
core machine, it takes 45 microseconds to create a DecimalFormat; while it takes
anywhere between 800 and 1700ns to format an integer depending on its size.
Imagine how long it takes to create a DecimalFormat on a small device like a
phone. Some customers report that it can take over 1 millisecond to create one
of these DecimalFormat objects.

One solution is to tell customers to reuse the same DecimalFormat object over
and over again to amortize the cost of its creation, but doing that takes extra
discipline and engineering effort because DecimalFormat objects are mutable.
Sharing DecimalFormat objects can mean thinking about concurrency which is hard
for the casual developer to do correctly. Its just so much easier to create a
new DecimalFormat on the fly whenever one is needed. We need to do something on
our end to make the Decimal Format API easier for others to use.

High level proposal

I propose that we make a DecimalFormat object a facade around other objects that
will do the actual work. Because creating these objects from scratch using ICU
resource bundles will be as expensive as creating a DecimalFormat is today, we
will cache them by locale ID in an LRU cache. Each cached object will be
read-only and contain only the default settings for its locale ID. The maximum
size of the LRU cache will be set to some default value like 100. When the LRU
cache fills up, we will attempt to find unused objects to evict. Next, we
discuss how the DecimalFormat object will use these cached objects.

A DecimalFormat will contain a pointer to these delegate objects. When caller
creates a new DecimalFormat object for a locale, the new DecimalFormat object
will point to the objects in the cache for that Locale. It will also increment
the reference count for that locale by 1. The reference count for a locale
indicates how many live DecimalFormat object there are for that locale. Only
when the reference count for a locale reaches 0 can the LRU cache evict the
contents for that locale. If the caller changes a DecimalFormat setting,
DecimalFormat will make a copy of only those objects that need to change before
changing the setting. Once DecimalFormat has its own copy of an object, that
object need not be recopied if it is changed again. A typical DecimalFormat
object will own its own copy of some objects while referencing the cache for
other objects. When the caller deletes a DecimalFormat object, the DecimalFormat
deletes any local copies of objects it made for itself and finally decrements
the reference count for its locale. In the rare event that the LRU cache is
filled up and there are locales that are eligible for eviction, DecimalFormat
will create and use its own copy of all delegate objects. In this rare case,
performance will degrade to what it is today.

Decision to use LRU cache rejected in favor of using a cache that grows
indefinitely. Rationale: 1) LRU caches require performance tuning by client.
Client has to know how many entries to allow which is problematic since entries
vary in size. Client can't answer such questions.

Since ICU 54, we have one universal cache in ICU4C for everything not just
formatting.If client finds they run out of memory, they can call flush.

If we are lucky and can simplify DecimalFormat enough, cloning the internal
classes may be cheap enough that we don't need the reference counting.

How this solves the performance issues we see today

Typically, creating a DecimalFormat will involve accessing the LRU cache to get
the pointer to the default objects and incrementing the reference count for that
locale. This should be 2 to 3 orders of magnitude faster than how we do it
today. There will be some performance hit when caller changes settings in a
DecimalFormat object, but this hit will be far less expensive than creating a
DecimalFormat object from scratch today. To clone a DecimalFormat object today
takes 3.5 microseconds on my machine compared to the 45 microseconds to create
from scratch. Moreover, DecimalFormat only needs to copy what it is changing
rather than copying everything when changing a setting for the first time. By
having DecimalFormat delegate to smaller objects, we can painlessly introduce
fast track objects. One use case for a fast track object is for formatting
integers. Today I can write simple code to format an integer with separators and
a plus/minus sign that runs in 45 to 130ns on my machine compared to 800ns to
1700ns for DecimalFormat.

How this solves the maintainability issues we see today

By making DecimalFormat a facade around other objects, we will have the
opportunity to build smaller objects that each do only one thing and one thing
well. Moreover, each of these smaller objects can be designed so that they can
be tested independently of one another. It is much easier to write good tests
for several smaller objects than it is to write tests for one giant, monolithic
one. Having good tests with high coverage means having more reliable code with
fewer bugs.

Alternative designs

An alternative design is to maintain a cache of default DecimalFormat objects,
one for each locale. Creating a DecimalFormat object would involve cloning one
from the cache. This design would be much easier to implement, and setter
methods on DecimalFormat would not require any copying of data. However cloning
a DecimalFormat object still takes 3.5 microseconds on my machine while the
typical format operation takes between 0.8 and 1.7 microseconds. Even with this
design, DecimalFormat creation would still be much more expensive than
DecimalFormat use. Moreover, we would not have the opportunity to break
DecimalFormat down into smaller objects.

A second alternative design is the same as the proposed design except that the
cache key would be (locale, data ID) instead of just locale. With an expanded
keyspace we could load only the most common stuff for a locale initially and
load stuff for the more esoteric code paths on demand. With this design, there
would be more overhead because destroying a DecimalFormat object would mean
updating multiple reference counts instead of just one. Also classes like
DecimalFormatSymbols would need to be chopped into smaller pieces to separate
the common data from the esoteric.

A third alternative is to is to leave DecimalFormat itself alone, but add a
NumberFormatPool with tunable performance. NumberFormatPool would be thread-safe
so that a single instance of it could be shared with an entire application. The
constructor would have arguments like LRU size and eviction policy. Callers
would use NumberFormatPool to create NumberFormat objects instead of the
NumberFormat:: static methods. This alternative is similar to the first, but has
the added advantage that we would not need to change DecimalFormat, but this
alternative shares the same disadvantages as the first.

Rollout plan

The first stage of this will be to move DecimalFormatSymbols objects out to the
LRU cache. Benchmarking that I did with valgrind shows that 67% of time to
create a DecimalFormat object goes into creating a DecimalFormatSymbols object.
After moving DecimalFormatSymbols to the cache, we should see a 3X performance
gain with creating DecimalFormat objects. The next class to go into the cache is
NumberingSystem. A DecimalFormat object creates not one, but two NumberingSystem
objects. DecimalFormat owns one while DecimalFormatSymbols owns the other.
Creating these two objects takes about 23% of execution time. With both of these
objects moved to the cache, we would enjoy a 10X performance gain when creating
DecimalFormat objects.

Schedule

TBD

Concluding Remarks

Currently, DecimalFormat creation can cause noticeable performance problems
especially on smaller devices like phones. I believe that an LRU cache with
reference counting that contains several smaller objects could solve this
performance issue. Formatting 64 bit ints with DecimalFormat could be an order
of magnitude faster than it is today, by introducing fast track objects.
Finally, it will be much easier to achieve 100% test coverage with several small
objects than with one monolithic one, and well tested code has fewer bugs.
