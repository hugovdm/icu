# Double-Checked Locking

Update 2014-06-14

All lazy initialization in ICU should use umtx_initOnce(). See
[one-time-initialization](one-time-initialization.md)

The implementation does double-checked locking where possible, including the
appropriate platform/compiler annotations needed for memory barriers and to
prevent compiler reordering of operations.

---

ICU uses double-checked locking for access to immutable, not statically
initializable objects in performance-sensitive code. For example, we have static
API functions (without a service object) for access to Unicode properties, and
use double-checked locking for the lazy loading and initialization of the
property names data (pnames.icu).

TBD: Point to code, especially the UMTX_CHECK() macro.

Double-checked locking is generally thread-unsafe, so we have to use a mutex to
read the variable (pointer to the object, or boolean flag for whether it has
been initialized). This is expensive, so we turn it off in a single-threaded
build, and we try to turn it off for platforms with a sufficiently strong memory
model.

TBD: List platforms where we don't mutex. Just Windows?

We should expand the set of platforms for which we provide more efficient
double-checked locking. We should consider using intermediate, as cheap as
possible (cheaper than mutex), mechanisms on platforms that do not require a
mutex but don't have a memory model strong enough to not do anything. For
example, we should try to use volatile if that is sufficient, or consider using
memory barriers.

TBD: The following is just a collection of material for now.

Double-checked locking (Wikipedia):
http://en.wikipedia.org/wiki/Double_checked_locking_pattern

The "Double-Checked Locking is Broken" Declaration:
http://www.cs.umd.edu/~pugh/java/memoryModel/DoubleCheckedLocking.html

Double-checked locking and the Singleton pattern:
http://www.ibm.com/developerworks/java/library/j-dcl.html

Understand the Impact of Low-Lock Techniques in Multithreaded Apps:
http://msdn.microsoft.com/en-us/magazine/cc163715.aspx

Who ordered memory fences on an x86?
http://bartoszmilewski.wordpress.com/2008/11/05/who-ordered-memory-fences-on-an-x86/

Multicores and Publication Safety:
http://bartoszmilewski.wordpress.com/2008/08/04/multicores-and-publication-safety/

IA Memory Ordering (Google Tech Talk):
http://www.youtube.com/watch?v=WUfvvFD5tAA

Intel 64 Architecture Memory Ordering White Paper: This document has been merged
into Volume 3A of Intel 64 and IA-32 Architectures Software Developer’s Manual
(System Programming Guide): http://www.intel.com/Assets/PDF/manual/253668.pdf
(See chapter 7.2 Memory Ordering).

AMD64 Architecture Programmer’s Manual:
http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/24593.pdf

Parallelism and the ARM Instruction Set Architecture:
http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=01463106 (see "Weakly
ordered memory consistency")

Don't rely on memory barriers for synchronization... Only if you don't aware of
Relacy Race Detector!
http://software.intel.com/en-us/blogs/2009/03/03/dont-rely-on-memory-barriers-for-synchronization-only-if-you-dont-aware-of-relacy-race-detector/
(good-sounding links in reader posts)

Memory Barrier for reader (user question, Intel answer):
http://software.intel.com/en-us/forums/threading-on-intel-parallel-architectures/topic/65071/

Single-Producer/Single-Consumer Queue:
http://software.intel.com/en-us/articles/single-producer-single-consumer-queue/

gcc atomic operation built-ins.
<http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html> (do not seem
to include a simple "fetch", only "fetch-and-do-something")

Original? paper on Double-Checked Locking, An Optimization Pattern for
EfficientlyInitializing and Accessing Thread-safe Objects (does not address weak
memory models): http://www.cs.wustl.edu/~schmidt/PDF/DC-Locking.pdf
