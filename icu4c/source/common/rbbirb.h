//
//  rbbirb.h
//
//  Copyright (C) 2002-2003, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains declarations for several classes from the
//    Rule Based Break Iterator rule builder.
//


#ifndef RBBIRB_H
#define RBBIRB_H

#include "unicode/utypes.h"
#include "unicode/uobject.h"
#include "unicode/rbbi.h"
#include "unicode/uniset.h"
#include "unicode/parseerr.h"
#include "uhash.h"
#include "uvector.h"
#include "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.



U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
struct              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : public UMemory { // The symbol table hash table contains one
public:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          key;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

private:
    RBBISymbolTableEntry(const RBBISymbolTableEntry &other); // forbid copying of this class
    RBBISymbolTableEntry &operator=(const RBBISymbolTableEntry &other); // forbid copying of this class
};


class RBBISymbolTable : public UMemory, public SymbolTable {
private:
    const UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

    // These next two fields are part of the mechanism for passing references to
    //   already-constructed UnicodeSets back to the UnicodeSet constructor
    //   when the pattern includes $variable references.
    const UnicodeString      ffffString;      // = "/uffff"
    UnicodeSet              *fCachedSetLookup;

public:
    //  API inherited from class SymbolTable
    virtual const UnicodeString*  lookup(const UnicodeString& s) const;
    virtual const UnicodeFunctor* lookupMatcher(UChar32 ch) const;
    virtual UnicodeString parseReference(const UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) const;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, const UnicodeString &fRules, UErrorCode &status);
    virtual ~RBBISymbolTable();

    virtual RBBINode *lookupNode(const UnicodeString &key) const;
    virtual void      addEntry  (const UnicodeString &key, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    virtual void      rbbiSymtablePrint() const;
#else
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int  fFakeField;
    #define rbbiSymtablePrint() fFakeField=0; 
#endif

private:
    RBBISymbolTable(const RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(const RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : public UMemory {
public:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    static BreakIterator * createRuleBasedBreakIterator( const UnicodeString    &rules,
                                    UParseError      &parseError,
                                    UErrorCode       &status);

public:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(const UnicodeString  &rules,
                    UParseError          &parseErr,
                    UErrorCode           &status
        );

    virtual    ~RBBIRuleBuilder();
    char                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    const UnicodeString           &fRules;           // The rule string that we are compiling

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *fForwardTree;     // The parse trees, generated by the scanner,
    RBBINode                      *fReverseTree;     //   then manipulated by subsequent steps.
    RBBINode                      *fSafeFwdTree;
    RBBINode                      *fSafeRevTree;

    RBBINode                      **fDefaultTree;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         fChainRules;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLBCMNoChain;      // True:  suppress chaining of rules on
                                                     //   chars with LineBreak property == CM.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *fSetBuilder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *fForwardTables;   // State transition tables
    RBBITableBuilder              *fReverseTables;
    RBBITableBuilder              *fSafeFwdTables;
    RBBITableBuilder              *fSafeRevTables;

    RBBIDataHeader                *flattenData();    // Create the flattened (runtime format)
                                                     // data tables..
private:
    RBBIRuleBuilder(const RBBIRuleBuilder &other); // forbid copying of this class
    RBBIRuleBuilder &operator=(const RBBIRuleBuilder &other); // forbid copying of this class
};




//----------------------------------------------------------------------------
//
//   RBBISetTableEl   is an entry in the hash table of UnicodeSets that have
//                    been encountered.  The val Node will be of nodetype uset
//                    and contain pointers to the actual UnicodeSets.
//                    The Key is the source string for initializing the set.
//
//                    The hash table is used to avoid creating duplicate
//                    unnamed (not $var references) UnicodeSets.
//
//                    Memory Management:
//                       The Hash Table owns these RBBISetTableEl structs and
//                            the key strings.  It does NOT own the val nodes.
//
//----------------------------------------------------------------------------
struct RBBISetTableEl {
    UnicodeString *key;
    RBBINode      *val;
};


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#include <stdio.h>
#define RBBIDebugPrintf printf
#else
inline void RBBIDebugPrintf(...) {}
#endif

U_NAMESPACE_END
#endif



