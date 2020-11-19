# Caching

[TOC]

The in-memory size of ICU is important. Both that and performance are closely
related to how we cache. We definitely have the opportunity to improve both size
and performance by doing better caching. Right now we don't cache things we
should cache, and we don't dump things out of the cache when they are
sufficiently old or unused. Because we have separate caches for different
objects, we also can't have a good overall strategy for removal when no longer
needed.
Here are some thoughts on how to do a better job. At least part of this involves
making the job of caching easier, so that it isn't as onerous to "do the right
thing" for programmers.

## Low-level cache

At a low level, we have a global cache. To get something out of the cache, you
supply a service key, a specific key, and a functor. The functor is specific to
the service, and knows how to create the requested object if not available in
the cache. The cache also supplies iteration over service keys, and within that
over specific keys. Public methods can also flush the cache, either partially
(to meet some maximum size objective or other criterion) or completely. Of
course, the cache is thread-safe. We also provide low-level hooks, so that
people can replace the caching mechanism with their own.

## Higher-level objects

At a higher level, we use the low level cache to create classes (by subclassing)
that make this syntactically easier for the service. Each such class knows about
a type of object, and how to create one, so it will supply the functor needed
for the global cache. These classes may provide some semantic sugar to cover
common cases:

*   immutable objects
*   mutable objects internal to the service
*   mutable objects that the service will clone and allow access to (eg through
    getInstance())

Some of this overlaps with the current registration mechanisms, so we should
look at those at the same time; we may be able to do some sharing of code,
although the registration has somewhat different goals.
