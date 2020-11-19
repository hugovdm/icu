// Collation v2 initial code collection
// created on: 2010-oct-27

/**
 * Basic definitions and static helper functions.
 */
class Collation {
    // Fixed primary lead bytes.
    static const uint8_t
        TERMINATOR_BYTE=0,
        LEVEL_SEPARATOR_BYTE=1,
        MERGE_SEPARATOR_BYTE=2,
        // Primary compression low terminator, not used in primary second byte.
        PRIMARY_COMPRESSION_LOW_BYTE=3,
        // Primary compression high terminator, not used in primary second byte.
        PRIMARY_COMPRESSION_HIGH_BYTE=0xff,
        // Default secondary/tertiary weight lead byte.
        DEFAULT_SEC_TER_BYTE=5,
        FIRST_HAN_BYTE=0xf3,  // F3..F7
        FIRST_UNASSIGNED_BYTE=0xf8,  // F8..FA
        TRAIL_WEIGHT_BYTE=0xfb,
        FIRST_SPECIAL_BYTE=0xfc;

    /**
     * Special-CE32 tags, from bits 2..0 of a special 32-bit CE.
     * Bits 25..3 are used for data.
     */
    enum {
        /**
         * Points to 1..32 64-bit CEs.
         * Bits 25..8: Index into CE64 table.
         * Bits  7..3: Length-1
         */
        EXPANSION_TAG,
        /**
         * Decimal digit. Points to expansion for when numeric collation (CODAN) is off.
         * Bits 25..8: Index into CE64 table.
         * Bits  7..3: Length-1
         */
        DIGIT_TAG,
        /** Points to prefix trie. Bits 25..3: Index into trie table. */
        PREFIX_TAG,
        /** Points to contraction trie. Bits 25..3: Index into trie table. */
        CONTRACTION_TAG,
        /**
         * Tag for a lead surrogate code unit.
         * Bit 3 indicates whether any of the 1024 corresponding supplementary characters
         * has a UTrie2 value other than NO_CE32.
         */
        LEAD_SURROGATE_TAG,
        /** Tag for a Hangul syllable. TODO: or Jamo? TODO: data? */
        HANGUL_TAG,
        /**
         * Tag for a Unified_Ideograph.
         * Bits 25..3: Signed Han-implicit offset (see hanOffsetFromUnifiedIdeograph()).
         * See also hanPrimaryFromUnifiedIdeographAndCE32().
         */
        HAN_IMPLICIT_TAG,
        /** Only used for NO_CE32. */
        UNASSIGNED_IMPLICIT_TAG
    };

    // Special 32-bit CEs and other values.
    static uint32_t
        /**
         * Lower 32 bits of a CE with default secondary and tertiary weights.
         * = DEFAULT_SEC_TER_BYTE<<24 | DEFAULT_SEC_TER_BYTE<<8
         */
        DEFAULT_SEC_AND_TER_CE=0x05000500,
        /**
         * Mask for determining special 32-bit CE and its tag, ignoring its data.
         */
        SPECIAL_CE32_AND_TAG_MASK=0xfc000007,  // TODO: needed?
        /** Special-32-bit-CE-tag mask. */
        TAG_MASK=7,
        /**
         * Mask for determining special 32-bit CE and its tag, ignoring its data.
         */
        LEAD_SURROGATE_SPECIAL_CE32_NO_DATA=0xfc000000|LEAD_SURROGATE_TAG,  // TODO: needed?
        /**
         * Data lookup yields no collation element.
         * When found in the UCA table, construct an unassigned-character CE.
         */
        NO_CE32=0xffffffff;

    // Special 64-bit CEs.
    static int64_t
        /**
         * No CE: End of input. Only used in runtime code, not stored in data.
         */
        NO_CE=1;

    // Data structures except for expansion tables store 32-bit CEs which are
    // either specials (see tags below) or are compact forms of 64-bit CEs.
    // The expansion table stores sequences of 64-bit CEs.

    static inline UBool isSpecialCE32(uint32_t ce32) {
        return ce32>=((uint32_t)FIRST_SPECIAL_BYTE<<24);
    }

    static inline int32_t getSpecialCE32Tag(uint32_t ce32) {
        return (int32_t)ce32&TAG_MASK;
    }

    static inline UBool isLeadSurrogateSpecialCE32(uint32_t ce32) {  // TODO: needed?
        return (ce32&SPECIAL_CE32_AND_TAG_MASK)==LEAD_SURROGATE_SPECIAL_CE32_NO_DATA;
    }

    static inline UBool hasDataFromLeadSurrogateSpecialCE32(uint32_t ce32) {  // TODO: needed?
        return (ce32&8)!=0;
    }

    /** Returns a 64-bit CE from a non-special 32-bit CE. */
    static inline int64_t ceFromCE32(uint32_t ce32) {
        if(ce32<=0xffffff) {
            // long-secondary form 00sssstt -> 00000000sssstt00
            return (int64_t)ce32<<8;
        } else {
            uint32_t tertiary=ce32&0xff;
            if(tertiary!=0) {
                // normal form ppppsstt -> pppp0000ss00tt00
                return ((int64_t)(ce32&0xffff0000)<<32)|((ce32&0xff00)<<16)|(tertiary<<8);
            } else {
                // long-primary form pppppp00 -> pppppp00050000500
                return ((int64_t)ce32<<32)|DEFAULT_SEC_AND_TER_CE;
            }
        }
    }

    /**
     * Returns the signed Han-implicit offset for c. c must be a Unified_Ideograph.
     * Han characters up to U+3FFFF are supported, with (c+offset) resulting in indexes
     * that can be turned into 3-byte primary weights with 5 lead bytes.
     *
     * The offsets and index ranges are designed to accommodate full blocks,
     * but the UCA specifies that Han-implicit weights are to be used only
     * for assigned characters.
     *
     * Compatibility ideographs are excluded because they decompose to unified ideographs.
     * The F900..FAFF compatibility ideographs block contains twelve unified ideographs
     * among FA0E..FA29.
     *
     *  Block           code points     offset  indexes=c+offset
     *
     *  Unihan           4E00.. 9FFF    - 4E00      0.. 51FF
     *  compat           FA0E.. FA29    - A80E   5200.. 521B
     *  Ext A            3400.. 4DBF    + 1E20   5220.. 6BDF
     *  Ext B           20000..2A6DF    -19420   6BE0..112BF
     *  Ext CDE         2A700..2F7FF    -19440  112C0..163BF
     *  Future Ext      2FA20..3FFFF    -19660  163C0..2699F
     */
    static int32_t hanOffsetFromUnifiedIdeograph(UChar32 c) {
        if(c<0x4E00) {
            return 0x1E20;  // Ext A
        } else if(c<=0x9FFF) {
            return -0x4E00;  // Unihan
        } else if(c<=0xFA29) {
            return -0xA80E;  // compat
        } else if(c<=0x2A6DF) {
            return -0x19420;  // Ext B
        } else if(c<0x2F800) {
            return -0x19440;  // Ext CDE
        } else {
            return -0x19660;  // 2FA20..3FFFF
        }
    }

    /**
     * Returns the Han character primary weight for a Unified_Ideograph c
     * where hanIndex=c+hanOffsetFromUnifiedIdeograph(c).
     */
    static uint32_t hanPrimaryFromIndex(int32_t hanIndex) {
        // Third byte: 126 values, gap of 1.
        uint32_t primary=(4+(hanIndex%126)*2)<<8;
        hanIndex/=126;
        // Second byte: 251 values 04..FE excluding the primary compression bytes.
        primary|=(4+(hanIndex%251))<<16;
        hanIndex/=251;
        // First byte: 5 values cover all Han character indexes (hanIndex<0x269B2=5*251*126).
        return primary|((FIRST_HAN_BYTE+hanIndex)<<24);
    }
    // TODO: Tailor before U+4E00 -> same as after [last regular], and vice versa.
    // TODO: Tailor otherwise before/after UIdeo -> get index, dec/inc, get weight,
    // or directly compute prev/next weight from known gaps & encoding.
    // TODO: Set [first implicit] to hanPrimaryFromIndex(0).
    // TODO: Set [last implicit] to FIRST_HAN_BYTE+4.FE.FF so that we
    // do not accidentally tailor non-Han characters in the last Han lead byte.
    // (Or to hanPrimaryFromIndex(0x40000+hanOffsetFromUnifiedIdeograph(0x40000))
    //  if it does not matter.)

    /**
     * Returns the Han character primary weight for a Unified_Ideograph c
     * and its special CE32.
     */
    static inline uint32_t hanPrimaryFromUnifiedIdeographAndCE32(UChar32 c, uint32_t ce32) {
        return hanPrimaryFromIndex(c+((int32_t)ce32<<6)>>9);  // signed offset from bits 25..3
    }

    /**
     * Returns the unassigned-character primary weight for any valid code point c.
     */
    static uint32_t unassignedPrimaryFromCodePoint(UChar32 c) {
        // Fourth byte: 6 values, every 42nd byte value (gap of 41).
        uint32_t primary=4+(c%6)*42;
        c/=6;
        // Third byte: 253 values.
        primary|=(3+(c%253))<<8;
        c/=253;
        // Second byte: 251 values 04..FE excluding the primary compression bytes.
        primary|=(4+(c%251))<<16;
        c/=251;
        // First byte: 3 values cover all code points (c<0x11710E=3*251*253*6).
        return primary|((FIRST_UNASSIGNED_BYTE+c)<<24);
    }
    // TODO: This assumes that U+0000 is not unassigned [TEST], so that there is a gap
    // before the first actual unassigned code point. Tailor before/after ->
    // essentially same as for Han.
    // TODO: Set [first unassigned] to unassignedPrimaryFromCodePoint(0).
    // TODO: Set [last unassigned] to FIRST_UNASSIGNED_BYTE+2.FE.FF.FF
    // or unassignedPrimaryFromCodePoint(0x110000).

private:
    Collation();  // No instantiation.
};

/**
 * Collation element and character iterator.
 * Handles normalized UTF-16 text inline, with length or NUL-terminated.
 * Other text is handled by subclasses.
 */
class CollationIterator : public UObject {
public:
    CollationIterator(const CollationData *tailoring, const CollationData *ucaData,
                      const UChar *s, int32_t length)
            : data(tailoring), uca(ucaData),
              ces(NULL), cesIndex(-1), cesLength(0),
              start(s), pos(s), limit(length>=0 ? s+length : NULL) {
        trie=data->getTrie();
    }

    inline int64_t nextCE() {
        if(0<=cesIndex && cesIndex<cesLength) {
            // Read a 64-bit CE from a list.
            return ces[cesIndex++];
        }
        UChar32 c;
        uint32_t ce32;
        if(numCodePointsForPreviousCE>0) {
            // TODO: Do we need to support switching iteration direction?
            // If we do, can we move this code into a direction-switching function
            // to keep the nextCE() fastpath as fast as possible?
            forwardNumCodePoints(numCodePointsForPreviousCE);
            numCodePointsForPreviousCE=0;
        }
        if(pos!=NULL) {
            if(pos!=limit) {
                // Fastpath:
                // Read one UTF-16 code *unit* and look up its 32-bit CE value.
                // All lead surrogate code units have a special CE.
                c=*pos;
                if(c==0 && limit==NULL) {
                    limit=pos;
                    return Collation.NO_CE;
                }
                ++pos;
                ce32=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
            } else if(inBackwardBuffer) {
                // Stop reading at the end of the temporary buffer that contains
                // the substring collected for previousCE() when it saw an unsafe code point.
                return Collation.NO_CE;
            } else {
                ce32=handleNextCE32(c);
            }
        } else {
            ce32=handleNextCE32(c);
        }
        int64_t ce;
        if(!Collation.isSpecialCE32(ce32)) {
            // Normal CE from the main data.
            ce=Collation.ceFromCE32(ce32);
        } else if(ce32==Collation.NO_CE32 && uca!=NULL) {
            if(c<0) {
                return Collation.NO_CE;  // end of input
            }
            // Fallback to UCA.
            ce=uca->nextCE(*this, c);
        } else {
            ce=data->nextCEFromSpecialCE32(*this, c, ce32);
            if(ce==Collation.NO_CE) {
                ce=uca->nextCE(*this, c);
            }
        }
        return ce;
    }

    inline int64_t previousCE() {
        if(cesIndex>0) {
            // Read a 64-bit CE from a list.
            return ces[--cesIndex];
        }
        UChar32 c;
        uint32_t ce32;
        UBool fromInnerBuffer;
        // TODO: Do we need to support switching iteration direction?
        // If so, we probably need to track numCodePointsFor_Next_CE as well.
        // (Or rather, a signed delta.)
        // See question in nextCE() about handling this in a direction-switching function.
        numCodePointsForPreviousCE=1;
        if(pos!=NULL && pos!=start) {
            UTRIE2_U16_PREV32(trie, start, pos, c, ce32);
            fromInnerBuffer=TRUE;
        } else {
            ce32=handlePreviousCE32(c);
            if(c<0) {
                Collation.NO_CE;
            }
            fromInnerBuffer=FALSE;
        }
        if(CollationData.isUnsafeCodePoint(c)) {
            return previousCEUnsafe(c, fromInnerBuffer);
        }
        int64_t ce;
        if(!Collation.isSpecialCE32(ce32)) {
            // Normal CE from the main data.
            ce=Collation.ceFromCE32(ce32);
        } else if(ce32==Collation.NO_CE32 && uca!=NULL) {
            // Fallback to UCA.
            ce=uca->previousCE(*this, c);
        } else {
            ce=data->previousCEFromSpecialCE32(*this, c, ce32);
            if(ce==Collation.NO_CE) {
                ce=uca->previousCE(*this, c);
            }
        }
        return ce;
    }

    inline UChar32 nextCodePoint() {
        if(pos!=NULL && pos!=limit) {
            UChar32 c=*pos;
            if(c==0 && limit==NULL) {
                limit=pos;
                return U_SENTINEL;
            }
            ++pos;
            UChar trail;
            if(U16_IS_LEAD(c) && pos!=limit && U16_IS_TRAIL(trail=*pos)) {
                ++pos;
                return U16_GET_SUPPLEMENTARY(c, trail);
            } else {
                return c;
            }
        }
        if(inBackwardBuffer) {
            // Stop reading at the end of the temporary buffer that contains
            // the substring collected for previousCE() when it saw an unsafe code point.
            return U_SENTINEL;
        }
        return handleNextCodePoint();
    }

    inline UChar32 previousCodePoint() {
        if(pos!=NULL && pos!=start) {
            UChar32 c=*--pos;
            UChar lead;
            if(U16_IS_TRAIL(c) && pos!=start && U16_IS_LEAD(lead=*(pos-1))) {
                --pos;
                return U16_GET_SUPPLEMENTARY(lead, c);
            } else {
                return c;
            }
        }
        return handlePreviousCodePoint();
    }

    inline void forwardNumCodePoints(int32_t num) {
        UChar32 c;
        if(num<=0) {
            return;
        }
        if(pos!=NULL && limit==NULL) {
            // Go forward in the inner, NUL-terminated buffer.
            for(;;) {
                c=*pos;
                if(c==0) {
                    limit=pos;
                    break;
                }
                ++pos;
                --num;
                if(U16_IS_LEAD(c) && U16_IS_TRAIL(*pos)) {
                    ++pos;
                }
                if(num==0) {
                    return;
                }
            }
        }
        for(;;) {
            if(pos!=NULL && pos!=limit) {
                // Go forward in the inner buffer as far as possible.
                do {
                    c=*pos++;
                    --num;
                    if(U16_IS_LEAD(c) && pos!=limit && U16_IS_TRAIL(*pos)) {
                        ++pos;
                    }
                    if(num==0) {
                        return;
                    }
                } while(pos!=limit);
            }
            // Try to go forward by one code point via the iterator,
            // then return to the inner buffer in case the subclass moved that forward.
            if(handleNextCodePoint()<0) {
                return;
            }
            if(--num==0) {
                return;
            }
        }
    }

    inline void backwardNumCodePoints(int32_t num) {
        // TODO
    }

    /**
     * Returns the code point starting with the given lead surrogate.
     * Reads a trail surrogate from the current UTF-16 position if there is one.
     * Returns a supplementary code point if there is a trail surrogate,
     * or else returns the lead surrogate.
     *
     * Must only be called by CollationData.nextCEFromSpecialCE32()
     * when that was called for a lead surrogate.
     */
    inline UChar32 nextCodePointFromLeadSurrogate(UChar32 lead) {
        UChar trail;
        if(pos!=limit && U16_IS_TRAIL(trail=*pos)) {
            ++pos;
            return U16_GET_SUPPLEMENTARY(lead, trail);
        } else {
            return lead;
        }
    }

protected:
    // TODO: setText(start, pos, limit)

    /**
     * Returns U_SENTINEL and Collation.NO_CE32 if we are at the end of the input, or:
     * A subclass may have an implementation reading from an iterator
     * or checking for FCD on the fly etc.
     */
    virtual uint32_t handleNextCE32(UChar32 &c) {
        c=U_SENTINEL;
        return Collation.NO_CE32;
    }

    virtual uint32_t handlePreviousCE32(UChar32 &c) {
        c=U_SENTINEL;
        return Collation.NO_CE32;
    }

    virtual UChar32 handleNextCodePoint() {
        return U_SENTINEL;
    }

    virtual UChar32 handlePreviousCodePoint() {
        return U_SENTINEL;
    }

private:

    int64_t previousCEUnsafe(UChar32 c, UBool fromInnerBuffer) {
        if(fromInnerBuffer) {
            // Find a safe code point in the inner buffer.
            const UChar *backwardLimit=pos+U16_LENGTH(c);
            while(pos!=start) {
                c=*--pos;
                UChar lead;
                if(U16_IS_TRAIL(c) && pos!=start && U16_IS_LEAD(lead=*(pos-1))) {
                    --pos;
                    c=U16_GET_SUPPLEMENTARY(lead, c);
                }
                ++numCodePointsForPreviousCE;
                if(!CollationData.isUnsafeCodePoint(c)) {
                    // The entire unsafe-backward substring is a portion of the inner buffer.
                    // Work with it directly, without copying its contents.
                    suspendedPos=pos;
                    suspendedLimit=limit;
                    limit=backwardLimit;
                    return previousCEFromBackwardBuffer();
                }
            }
            // Some part of the unsafe-backward substring needs to be read from the iterator.
            // Copy the suffix from the inner buffer into the backwardBuffer and continue below.
            backwardBuffer.clear().prepend(start, (int32_t)(backwardLimit-start));
        } else {
            backwardBuffer.clear().prepend(c);
        }
        // Find a safe code point from the iterator.
        while(c=previousCodePoint()>=0) {
            ++numCodePointsForPreviousCE;
            backwardBuffer.prepend(c);
            if(!CollationData.isUnsafeCodePoint(c)) {
                break;
            }
        }
        suspendedPos=pos;
        suspendedLimit=limit;
        pos=backwardBuffer.getBuffer();
        limit=pos+backwardBuffer.length();
        return previousCEFromBackwardBuffer();
    }

    int64_t previousCEFromBackwardBuffer() {
        inBackwardBuffer=TRUE;
        cesIndex=-1;  // Ensure that we don't see CEs from a later-in-text expansion.
        int64_t ce=nextCE();
        // TODO: assert ce!=Collation.NO_CE
        if(pos==limit) {
            // The entire unsafe-backward substring resulted in one expansion.
            // Do not copy its CEs.
            if(cesIndex<=0 || cesLength==1) {
                // Only one CE.
                inBackwardBuffer=FALSE;
                pos=suspendedPos;
                limit=suspendedLimit;
                return ce;
            }
            // Just return CEs from the ces[] expansion.
        } else {
            ceBuffer.clear();
            do {
                ceBuffer.append(ce);
            } while(ce=nextCE()!=Collation.NO_CE);
            ces=ceBuffer.getBuffer();
            cesIndex=cesLength=ceBuffer.length();
        }
        inBackwardBuffer=FALSE;
        pos=suspendedPos;
        limit=suspendedLimit;
        return ces[--cesIndex];
    }

    // UTF-16 string pointers.
    // limit can be NULL for NUL-terminated strings.
    const UChar *start, *pos, *limit;
    // TODO: getter for limit, so that caller can find out length of NUL-terminated text?

    // Number of code points consumed in previousCE().
    // Used when switching direction to nextCE().
    // nextCE() itself sets this to 0.
    // TODO: Do we need to support switching iteration direction?
    int32_t numCodePointsForPreviousCE;

    // List of 64-bit CEs.
    const int64_t *ces;
    int32_t cesIndex, cesLength;

    // Main lookup trie of the data object.
    const UTrie2 *trie;  // TODO: protected, for possibly optimized subclass CE lookup?

    // Either data is for UCA and uca is NULL, or data is for a tailoring and uca is not NULL.
    const CollationData *data;
    const CollationData *uca;

    // TODO: Flag/getter/setter for CODAN, so that CollationData's special-CE code
    // can query it.

    // Buffer for skipped combining marks during discontiguous contraction handling.
    UnicodeString skipBuffer;

    PrependUnicodeBuffer backwardBuffer;
    // TODO: Write PrependUnicodeBuffer class.
    const UChar *suspendedPos, *suspendedLimit;  // TODO: review need & use
    UBool inBackwardBuffer;

    // 64-bit-CE buffer for backward iteration and other complicated cases.
    // TODO: MaybeStackArray? Support efficient prepending.

    // TODO: Flag or UErrorCode for memory allocation errors? protected?
};

class FCDCollationIterator : public CollationIterator {
    // TODO: On-the-fly FCD check.
    // Remember maximum known boundaries of already-FCD text.
};

/**
 * Data container with access functions which walk the data structures.
 */
class CollationData : public UMemory {
public:
    int64_t nextCE(CollationIterator &iter, UChar32 c) const {
        uint32_t ce32=UTRIE2_GET32(trie, c);
        if(!Collation.isSpecialCE32(ce32)) {
            // Normal CE.
            return Collation.ceFromCE32(ce32);
        } else {
            return nextCEFromSpecialCE32(iter, c, ce32);
        }
    }

    int64_t nextCEFromSpecialCE32(CollationIterator &iter, UChar32 c, uint32_t ce32) const {
        return Collation.NO_CE;  // TODO
        // TODO: For UNASSIGNED_IMPLICIT_TAG, if(isFinalData) return unassigned-implict CE else return NO_CE.
    }

    int64_t previousCE(CollationIterator &iter, UChar32 c) const {
        uint32_t ce32=UTRIE2_GET32(trie, c);
        if(!Collation.isSpecialCE32(ce32)) {
            // Normal CE.
            return Collation.ceFromCE32(ce32);
        } else {
            return previousCEFromSpecialCE32(iter, c, ce32);
        }
    }

    int64_t previousCEFromSpecialCE32(CollationIterator &iter, UChar32 c, uint32_t ce32) const {
        return Collation.NO_CE;  // TODO
        // TODO: For UNASSIGNED_IMPLICIT_TAG, if(isFinalData) return unassigned-implict CE else return NO_CE.
    }

    UBool isUnsafeCodePoint(UChar32 c) {
        if(U_IS_TRAIL(c)) {
            return TRUE;
        }
        return FALSE;  // TODO
        // TODO: Are all cc!=0 marked as unsafe for prevCE() (because of discontiguous contractions)?
        // TODO: Use a frozen UnicodeSet rather than an imprecise bit set?
    }

    /**
     * Returns this data object's main UTrie2.
     * To be used only by the CollationIterator constructor.
     */
    const UTrie2 *getTrie() { return trie; }

private:
    // Main lookup trie.
    const UTrie2 *trie;
    UBool isFinalData;
};

/**
 * Low-level CollationData builder.
 * Takes (character, CE) pairs and builds them into runtime data structures.
 * Supports characters with context prefixes and contraction suffixes.
 */
class CollationDataBuilder : public UObject {
};
