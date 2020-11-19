# One Time Initialization

4/1/2013 (but not a joke)

Andy Heninger

This is a design for a one time lazy initialization service in ICU.

*   Internal only, not public API.
*   Replaces the existing UMTX_CHECK for lazy initialization of ICU
    implementation objects
*   An alternative to the Singleton classes.
*   Minimize disruption to existing code when switching from UMTX_CHECK.
*   Simpler to use than UMTX_CHECK, reduce the chance of errors.

This proposal is loosely inspired by the InitOnceExecuteOnce function in
Windows,

[
http://msdn.microsoft.com/en-us/library/windows/desktop/ms683493(v=vs.85).aspx](http://msdn.microsoft.com/en-us/library/windows/desktop/ms683493(v=vs.85).aspx)

The idea is to separate out the control of the one time initialization from the
state of the items being initialized.

## Before and after Example

This example is from common/unames.cpp. The code is typical of the existing lazy
initialization that occurs widely throughout ICU.

The "after" code compiles and runs with my experimental implementation.

**Before**:

```none
static UDataMemory *uCharNamesData=NULL;
static UCharNames *uCharNames=NULL;
static UErrorCode gLoadErrorCode=U_ZERO_ERROR;
...
static UBool U_CALLCONV unames_cleanup(void)
{
    if(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = NULL;
    }
    if(uCharNames) {
        uCharNames = NULL;
    }
    gMaxNameLength=0;
    return TRUE;
}
...
static UBool
isDataLoaded(UErrorCode *pErrorCode) {
    /* load UCharNames from file if necessary */
    UBool isCached;
    /* do this because double-checked locking is broken */
    UMTX_CHECK(NULL, (uCharNames!=NULL), isCached);
    if(!isCached) {
        UCharNames *names;
        UDataMemory *data;
        /* check error code from previous attempt */
        if(U_FAILURE(gLoadErrorCode)) {
            *pErrorCode=gLoadErrorCode;
            return FALSE;
        }
        /* open the data outside the mutex block */
        data=udata_openChoice(NULL, DATA_TYPE, DATA_NAME, isAcceptable, NULL, pErrorCode);
        if(U_FAILURE(*pErrorCode)) {
            gLoadErrorCode=*pErrorCode;
            return FALSE;
        }
        names=(UCharNames *)udata_getMemory(data);
        /* in the mutex block, set the data for this process */
        {
            umtx_lock(NULL);
            if(uCharNames==NULL) {
                uCharNamesData=data;
                uCharNames=names;
                data=NULL;
                names=NULL;
                ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
            }
            umtx_unlock(NULL);
        }
        /* if a different thread set it first, then close the extra data */
        if(data!=NULL) {
            udata_close(data); /* NULL if it was set correctly */
        }
    }
    return TRUE;
```

**After**:

<pre><code>static UDataMemory *uCharNamesData=NULL;
static UCharNames *uCharNames=NULL;
<b>static UInitOnce gCharNamesInitOnce = U_INITONCE_INITIALIZER;</b>
...
static UBool U_CALLCONV unames_cleanup(void)
{
    if(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = NULL;
    }
    if(uCharNames) {
        uCharNames = NULL;
    }
    <b>gCharNamesInitOnce.reset();</b>
    gMaxNameLength=0;
    return TRUE;
}
...
static void U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == NULL);
    U_ASSERT(uCharNames == NULL);
    uCharNamesData = udata_openChoice(NULL, DATA_TYPE, DATA_NAME, isAcceptable, NULL, &status);
    if(U_FAILURE(status)) {
        uCharNamesData = NULL;
    } else {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}
static UBool
isDataLoaded(UErrorCode *pErrorCode) {
    <b>umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);</b>
    return U_SUCCESS(*pErrorCode);
}
</code></pre>

`UInitOnce` is a struct that holds the initialization state, which can be one of
these three:

1.  The initialization has not yet begun. The first thread to call
    umtx_initOnce() will invoke the initialization function.
2.  The initialization is in progress. Any additional threads calling
    umtx_initOnce() with this UInitOnce will block until the initialization is
    complete.
3.  The initialization is complete. Calls to umtx_initOnce() will do any
    required memory fencing voodoo to insure that the initialization is visible,
    and return.

`umtx_initOnce(UInitOnce &, init_function_pointer, *optional parameters*)`

The initOnce function calls through to the initialization function if needed.
Only one call will be made to the init function. If multiple threads race into
umtx_initOnce() with the same UInitOnce struct, all but the first will block
until the initialization is complete.

No mutex is held during the invocation of the init function. Blocking, when
needed, is done with condition variables (POSIX or Windows Vista or newer), or a
sleep() loop (WIndows XP only). Because control of the initialization process
does not depend on the variables being initialized, the init function does not
need to worry about the ordering of initialization when more than one global
item is being set.

Error handling: umtx_initOnce() and the init function itself optionally take a
UErrorCode parameter. When one is provided, the error status returned by the
init function will be remembered and returned by all subsequent calls to
umtx_initOnce() made with the same UInitOnce. There is no provision for retrying
a failed initialization.

Context Parameter: The init function can take an optional context parameter. The
context parameter may be of any type - the function is templatized. As long as
the types supplied to umtx_initOnce() and the types required by the init
function match, and are something that can actually be used as a function
parameter, it will work.

## Status

I have been experimenting with this design on a branch,
<http://source.icu-project.org/repos/icu/icu/branches/andy/10051/source/>

All source files from the common directory have been switched to use
umtx_initOnce(), and the design of initOnce() definitely evolved with going
through the process of finding out what was convenient to use and useful, and
what wasn't.

The usage code is pretty close to what I think it would actually look like. The
implementation code is not complete for all platforms, and needs to be better
organized to make the platform dependencies clear.

## Issues and Questions

### User Mutexes

It will be quite awkward to build this service on top of ICU's user mutexes.
User mutexes are defined in uclean.h by u_setMutexFunctions() and it's
associated facilities.

ICU user mutexes surface the bare minimum functionality that was needed to
support ICU synchronization as it existed many years ago. They are already
struggling. Problems include:

*   ICU mutexes (as seen by ICU implementation code) are self initializing. User
    mutexes, by the API surfaced to the providers of the mutex services, are
    not. This results in most ICU umtx_lock(m) operations requiring two
    underlying user mutex locks, one to acquire a global mutex before checking
    whether the actual desired mutex has yet been initialized, then a second to
    acquire the desired mutes.
*   ICU makes heavy use of UMTX_CHECK, which on most platforms does an efficient
    load-acquire read from a memory operation. With user mutexes, the only
    option is to fully mutex the memory read.
*   The code to manage user mutexes is complex. Mutex code is difficult to get
    right, and difficult to verify. I would be wary of undiscovered bugs.
*   The umtx_initOnce() functions that I am proposing are difficult to build on
    top of the existing user mutexes. Not impossible, but it would be complex
    and somewhat inefficient (although the inefficiencies probably wouldn't be
    worse than they are for what we have now.)
*   User mutexes are not widely used, although they are important for those who
    do use them.

I think the best solution is to withdraw support for user mutexes. The
replacement would be to provide better organized and documented ICU mutex
implementation files, making it easier for a platform with special requirements
to build ICU with mutex wrappers customized for its needs.

### C++11

The C++11 language provides a set of low level atomic memory operations, and the
C++ std library provides mutexes. C++11 is effectively a new platform.

I propose that we start supporting an ICU configuration based on pure C++11 now,
and prefer it over the use native Operating System functions and compiler
specific extensions when it is available. Over the long term, as C++11 support
grows more widespread, we should be able to reduce the amount of platform
dependent code in ICU.

### Windows XP

Windows XP presents some problems in implementing initOnce(). It has no native
support for condition variables, and they are not easily emulated. (Windows
InitOnceExecuteOnce() and condition variables were both introduced with Windows
Vista.) We can do a hacky work-around implementation, but again it is extra
complexity. It's probably too soon to do it now, but dropping XP support would
help simplify the code.

## Code

Here are the declarations for the init once constructs that would be directly
used by the rest of ICU. From umutex.h. TODO: docs, symbolic constants.

```none
struct UInitOnce {
    atomic_int32_t   fState;
    UErrorCode       fErrCode;
    void reset()     {fState = 0; fState=0;};
    UBool isReset()  {return umtx_LoadAcquire(fState) == 0;};
};
#define U_INITONCE_INITIALIZER {ATOMIC_INT32_T_INITIALIZER(0), U_ZERO_ERROR}
// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
inline void umtx_initOnce(UInitOnce &uio, void (*fp)()) {
    if (umtx_LoadAcquire(uio.fState) == 2) {
        return;
    }
    if (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio, TRUE);
    }
}
// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
inline void umtx_initOnce(UInitOnce &uio, void (*fp)(UErrorCode &), UErrorCode &errCode) {
    if (U_FAILURE(errCode)) {
        return;
    }    
    if (umtx_LoadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio, TRUE);
    } else {
        // Someone else already ran the initialization.
        if (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}
// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
template<class T> void umtx_initOnce(UInitOnce &uio, void (*fp)(T), T context) {
    if (umtx_LoadAcquire(uio.fState) == 2) {
        return;
    }
    if (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio, TRUE);
    }
}
// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
template<class T> void umtx_initOnce(UInitOnce &uio, void (*fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    if (U_FAILURE(errCode)) {
        return;
    }    
    if (umtx_LoadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio, TRUE);
    } else {
        // Someone else already ran the initialization.
        if (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}
```
