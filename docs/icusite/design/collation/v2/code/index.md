# code

[FractionalUCA.txt](http://source.icu-project.org/repos/icu/icu/trunk/source/data/unidata/FractionalUCA.txt)

[UnicodeData.txt](http://source.icu-project.org/repos/icu/icu/trunk/source/data/unidata/UnicodeData.txt)

prerequisites

*   finish string trie builder in C++
*   instrument v1 builder (genuca & CLDR tailorings) to find out if
    proposed/desirable limitations are feasible
    *   especially, collect and analyze all contraction strings in UCA & CLDR
    *   max. number of CEs in an expansion
    *   sizes of prefix/contraction tables & expansion tables
    *   min & max tailored code points \[consider special, compact-or-fast
        lookup tables; consider 3-level lookups for search collators, eor
        collators, etc.\]
    *   when we tailor after an expansion, do we just create a single CE with
        the specified relation over the reset character's first CE? or do we
        automatically create a modified expansion?
    *   when we tailor after a contraction (&sch<x<y<z), does only the first
        following item get the expansion (&s<x/ch<y<z) or every following one
        too (&s<x/ch<y/ch<z/ch)?
*   understand secondary/tertiary compression; shorter range for that, more
    space for real weights
    *   low values 5 6 7 ... if the after-common weight is <5
    *   high value 45 44 43 ... if the after-common weight is >5
    *   looks like low & high value bands can intersect at one middle value for
        max-count common weights (low 05..24 middle 25 high 26..45; max-count
        0x21)
*   Does tailoring like &ch<x work if we have a prefix element for c|h? Does it
    yield CE(c)CE(c|h) or CE(c)CE(h)? Affects how we can express the UCA
    prefixes in rules. Might affect Japanese too.
*   maybe add v2 implicit-CE calculation to v1 code: a) removes manual
    adjustment of CJK ranges, b) cleanly splits Han vs. unassigned lead bytes

TODO: How do we create expansions like in FractionalUCA.txt that tend to modify
the last expansion CE rather than the first one? Or is it ok to modify any of
them?

rules:

<        '('

<<<      （

<<<      ⑴ / 1')'

<<<      ⑽ / 10')'

<<<      ⑾ / 11')'

fractional:

0028; \[0A B5, 05, 05\]

FF08; \[0A B5, 05, 07\]

FE59; \[0A B5, 05, 1D\]

207D; \[0A B5, 05, 27\]

208D; \[0A B5, 05, 29\]

FE35; \[0A B5, 05, 2B\]

2474; \[0A B5, 05, 09\]\[14, 05, 09\]\[0A B7, 05, 3D\] # ⑴

247D; \[0A B5, 05, 09\]\[14, 05, 09\]\[12, 05, 3D\]\[0A B7, 05, 3D\] # ⑽

247E; \[0A B5, 05, 09\]\[14, 05, 09\]\[14, 05, 3D\]\[0A B7, 05, 3D\]

247F; \[0A B5, 05, 09\]\[14, 05, 09\]\[16, 05, 3D\]\[0A B7, 05, 3D\]

TODO: Tailoring to a string and its expansions (as in &sch<x or &l·<x) could be
trouble if we make it different from the current equivalent &s<x/ch etc. because
it might be hard to find and manage the reset point in existing token lists.

*   Idea: For getting the correct expansion when a prefix is involved, rather
    than &l·<x we could add syntax to do &l<x/l|· -- that is, provide the prefix
    in the expansion string but have it generate an expansion CE only for the
    part after the pipe symbol. We would get a pair of CEs where the first is
    primary-different from CE(l) and the second is the CE of
    middle-dot-preceded-by-l.

TODO: With contraction ba, does canonical closure add contractions bä, bå, bà?

TODO: Tertiary weights: Default case handling masks off case bits, leaving only
the tertiary weight. Thus:

*   Assume that only lowercase or uncased characters should have the common
    weight. Are there any mixed/upper characters where there is no corresponding
    lowercase that has the common tertiary?
*   Keep runtime computation to create the 05..45 gap for "common" compression,
    because if we pre-spread the stored data via the case bits, that would be
    undone by the case bit mask.
*   When case bits are off, we could expand the "common" compression gap to
    05..C5. Would reduce sort key length for very long stretches (>33) of
    all-common tertiary weights, but that might be unusual enough not to be
    worth it.

TODO: Tertiary weights:

*   In lowerFirst, do we move 06..BF up by 40, same compression gap as default?
    Is "common" only lowercase?
*   In upperFirst, do we xor the case bits and move mixed/upper down (or move
    then xor) for a gap 85..C5?

TODO: In two-byte script boundaries like \[first Grek\] we can use 03 in the
second byte. It is only a boundary in invuca, never generated in sort keys. 03
is of course ok in the third byte of three-byte boundaries. There should be no
single-byte boundaries; those would just waste a lead byte.

### Tricky contractions

with lccc!=0

0F71;TIBETAN VOWEL SIGN AA;Mn;**129**;NSM;;;;;N;;;;;

0F72;TIBETAN VOWEL SIGN I;Mn;**130**;NSM;;;;;N;;;;;

0F73;TIBETAN VOWEL SIGN II;Mn;**0**;NSM;**0F71 0F72**;;;;N;;;;;

0F74;TIBETAN VOWEL SIGN U;Mn;**132**;NSM;;;;;N;;;;;

0F75;TIBETAN VOWEL SIGN UU;Mn;**0**;NSM;**0F71 0F74**;;;;N;;;;;

0F80;TIBETAN VOWEL SIGN REVERSED I;Mn;**130**;NSM;;;;;N;TIBETAN VOWEL SIGN SHORT
I;;;;

0F81;TIBETAN VOWEL SIGN REVERSED II;Mn;**0**;NSM;**0F71 0F80**;;;;N;;;;;

0F71; \[74 AA, 05, 05\]

0F72; \[74 AC, 05, 05\]

0F71 0F72; \[74 AE, 05, 05\]

0F73; \[74 AE, 05, 05\]

0F80; \[74 B0, 05, 05\]

0F71 0F80; \[74 B2, 05, 05\]

0F81; \[74 B2, 05, 05\]

0F74; \[74 B4, 05, 05\]

0F71 0F74; \[74 B6, 05, 05\]

0F75; \[74 B6, 05, 05\]

TODO: Which CEs to get for <0F71 0F71 0F72 0F72>? Should it make a discontiguous
contraction with the first and third characters, leaving the second (skipped)
character and fourth character for another contraction??

TODO: Collect data about Unicode decompositions

*   Are there characters for which lccc!=0 and their decomposition has more than
    2 characters? If so, are there more than 2 different ccc values in the
    decomposition?
*   Are there characters for which lccc!=0 and tccc=0? Or generally lccc>tccc?
*   If not, then we should be able to use the fcd16 value of a character in
    discontiguous contractions. Using the lccc and tccc from the fcd16 should
    improve behavior of the precomposed Tibetan vowel signs over checking their
    ccc.

TODO: How does tailoring merge "before" and "after" rules? &a<x plus &\[before
1\]b<y -> &a<x<y (<b)? Does it require looking up base-table weights for reset
points early?

// Collation v2 initial code collection

// created on: 2010-oct-27

/\*\*

\* Basic definitions and static helper functions.

\*/

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

TODO: 03 & ff should be usable in the second primary weight byte if the lead
byte is not compressible; that is, the weight allocator could take the
compressibility flags as a parameter for its min/max bytes setup. Gives us yet a
little more room, especially for 2-byte ideographs after \[last regular\] or
\[first Hani\].

// Default secondary/tertiary weight lead byte.

DEFAULT_SECONDARY_BYTE=5,

DEFAULT_TERTIARY_BYTE=5,

FIRST_HAN_IMPLICIT_BYTE=0xf3, // F3..F7 TODO: could be F8..FC

FIRST_UNASSIGNED_IMPLICIT_BYTE=0xf8, // F8..FA TODO: could be just FD

TRAIL_WEIGHT_BYTE=0xfb, // TODO: could be FE

FIRST_SPECIAL_BYTE=0xfc; // TODO: could be just FF

TODO: If \[last regular\] < \[first Hani\] then existing large tailorings would
have to go in between. Probably need to make them the same.

TODO: Maybe use invalid weights like xx02 for script boundaries? They only serve
as tailoring boundaries and are stored in invuca but never returned at runtime.
Try to avoid wasting usable weights.

/\*\*

\* Special-CE32 tags, from bits 2..0 of a special 32-bit CE.

\* Bits 25..3 are used for data.

\* TODO: Maybe make some room for a few more tags, and move tag bits back to the
top. First special byte 0xfb, bits 26..23 for 10 tag values 6..15, bits 22..0
for data.

\* TODO: Maybe fewer data bits for most specials; e.g., 20 bits for
prefixes/contractions, 20 = 16 (index) + 4 (length<=16) for expansions. Still
want 23 bits for LATIN_EXPANSION_TAG.

\* -> One primary lead byte FE for LATIN_EXPANSION_TAG and room for another
23-data-bits tag, and one primary lead byte FFtiiiil for all other specials
where t=tag, iiii=index, l=length or digit; or iiiil=index that does not need
length. (getSignedData(), getUnsignedData(), getTag(), getIndex(), getLength()?)

\* -> ==or Latin Expansion could share a lead byte with trail weights.==
MIN_LATIN_EXPANSION_CE32=FE830000, MAX_PRIMARY_WEIGHT=FE820000 (for U+FFFF),
tailored trail weights FE04..FE81; allows to have just FF as the one specials
lead byte; isSpecial(ce32) if(>=FE84), isSpecialLatinExpansion(ce32) if(<FF00)

\* **==TODO: Even better:==** FE all for trail weights,
MAX_PRIMARY_WEIGHT=FEFE0000, FFt for specials where t=0..5 for LATIN_EXPANSION.

\*/

enum {

/\*\*

\* Points to 1..32 64-bit CEs.

\* Bits 25..8: Index into CE64 table.

\* Bits 7..3: Length-1

\*/

EXPANSION_TAG,

TODO: variant for sequence of only non-special CE32s?

TODO: tag for characters like U+0F81 if they always want to be decomposed?

/\*\*

\* Decimal digit. Points to expansion for when numeric collation (CODAN) is off.
TODO: Instead point to a 32-bit CE? Usually smaller, and supports digits with
specials.

\* TODO: Exponent in one or two bytes so that very long digit sequences are
handled as a single unit without overflow? -- See CODAN ideas near bottom.

TODO: Try to store the digit value in 4 of the data bits and use this rather
than a generic-property lookup.

TODO: What happens when a digit gets tailored? Does/should it lose the
DIGIT_TAG? Should it be reapplied for the tailoring?

\* Bits 25..8: Index into CE64 table.

\* Bits 7..3: Length-1

\*/

DIGIT_TAG,

/\*\* Points to prefix trie. Bits 25..3: Index into trie table. \*/

PREFIX_TAG,

/\*\* Points to contraction trie. Bits 25..3: Index into trie table. \*/

CONTRACTION_TAG,

/\*\*

\* TODO: This might help reduce actual expansions for a-umlaut and similar. It
might also make the runtime Latin-1 table less necessary.

\* TODO: Make sure we do not overflow the tag bits. (Remove lead-surrogate
special or use 4 bits for specials tags or expand the range of primary lead
bytes for specials.)

\* Latin expansion with two simple CEs \[pp, 05, tt\] \[00, ss, 05\].

\* Bits 25..19: Single-byte primary weight pp=0..7F of the first CE. (Could be
6-bit offset from primary weight of 'a'.)

\* Bits 18..11: Tertiary weight tt of the first CE.

\* Bits 10..3: Secondary weight ss of the second CE.

\*/

LATIN_EXPANSION_TAG, (MINI_EXPANSION_TAG)

/\*\*

\* Points to the forward-iteration 32-bit CE and

\* indicates that the character is unsafe for previousCE().

\* Not all CollationData.isUnsafeCodePoint() have this tag, only

\* contraction-ending characters or similar.

\* TODO: How disruptive is this for forward iteration?

\*/

UNSAFE_BACKWARD_TAG,

/\*\*

\* Tag for a lead surrogate code unit. -- TODO: worth it? see comments in
prototype nextCE()

\* Bit 3 indicates whether any of the 1024 corresponding supplementary
characters

\* has a UTrie2 value other than NO_CE32.

\*/

LEAD_SURROGATE_TAG,

/\*\* Tag for a Hangul syllable. TODO: or Jamo? TODO: data? \*/

HANGUL_TAG,

/\*\*

\* Tag for NO_CE32 (hanOffset=-1) or a Unified_Ideograph.

\* Bits 25..3: Signed Han-implicit offset (see hanOffsetFromUnifiedIdeograph()).

\* See also hanPrimaryFromUnifiedIdeographAndCE32().

TODO: Use data=0 for fallback to base table. Use data=fffff for
unassigned-implicit calculation. Use OFFSET_TAG for Unified_Ideographs.

TODO: in tailoring prefix/contraction result, store CE32 vs. "implicit" but not
fallback? or at least not fallback if the base has an implicit for the code
point.

\*/

IMPLICIT_TAG

};

TODO: Consider an OFFSET_TAG for characters that in the UCA are sorted in code
point order, with secondary/tertiary=common, and evenly spaced primary weights.
Best if dozens or hundreds are in a row. Reduces the cost for the UCA, and
especially for multiple base collators.

*   Goal: Same CE32 values for many consecutive code points to help UTrie2
    compaction.
*   Cost: One more trie lookup and a little computation.
*   Do not use this in tailorings because we assume that those want speed for
    tailored characters.
*   For example: Unified Canadian Aboriginal Syllabics (U+1401), Yi (starting
    from U+A000), Egyptian Hieroglyphs (U+13000).
*   Encode the base code point's CE normally. Encode all others as something
    like the lower 16 bits of the base code point and the small per-code point
    increment. For UCAS, U+1401->\[77 AA 3C, 05, 05\] and all others get
    0xFFt==1401==7 for an increment of 8=7+1 each. U+1425 gets primary weight 77
    AA 3C + (0x1425-0x1401)\*(7+1) with overflow from the third primary weight
    byte (and skipping the forbidden tertiary bytes 0..2).
    *   No need to mask off the base bits and reconstruct the full base code
        point, just to get the offset. Just use (c-(ce32>>4))&0xffff.
    *   Base and actual code point need not be in the same plane (although they
        normally will be because of the end-of-plane noncharacters), they just
        need to be at most 0xffff apart.
    *   Variations are possible. If the increment is a power of two, then we
        could store the log2(increment) rather than the increment itself. Then
        we might use fewer bits for that and more bits for the base code point
        (but more c.p. bits might not be worth it). Power-of-two turns into a
        shift, cheaper than multiplication, but we do have to use divmod anyway.
*   Use a bit or separate special-tag for 2-byte vs. 3-byte primaries? Or just
    figure that out at runtime looking at the base character's weight?
*   Or limit to 3-byte primaries?! We could require that the base CE32 is a
    regular long-primary value.
*   Make sure to have an equivalent optimization for such a range in the invuca
    data.

TODO: Use the OFFSET_TAG for Unified_Ideograph blocks too, rather than the
IMPLICIT_TAG. Share code between Han and other large, code-point-order scripts.
Unless we want to really optimize UCA Han lookup speed. (The offset tag needs an
extra trie lookup for the base weight, but the computation is similar).

*   The base builder would dynamically allocate the Han blocks from a
    UnicodeSetIterator(\[:Unified_Ideograph:\]). We can do this if the Han
    weights are data-driven (changes just need new data), rather than hardcoded
    in the semantics of a special-tag (where changes need runtime code
    modifications).
*   Dense: No wasted weights in incompletely used blocks. The 12 ideographs in
    the compatibility block would just get non-special CE32s.
*   The base builder would dynamically figure out how many lead bytes are
    needed. We should be able to use 3 lead bytes, not 5, until a lot more Han
    characters are added.
*   Unlike the Han-implicit algorithm below, this would not rely on assumptions
    about future CJK allocations, nor impose an artificial limit on them.
*   Just make sure to put the Han blocks into the right order.
*   We could add the Han blocks to the UCA rules. However, we should probably
    keep them out and hardcode them in the builder so that the rare user that
    fetches the UCA rules does not get swamped with Han characters. (Since the
    tailoring would not use this mechanism.)

// Special 32-bit CEs and other values.

static uint32_t

/\*\*

\* Lower 32 bits of a CE with default secondary and tertiary weights.

\* = DEFAULT_SEC_TER_BYTE<<24 | DEFAULT_SEC_TER_BYTE<<8

\*/

DEFAULT_SEC_AND_TER_CE=0x05000500,

/\*\*

\* Mask for determining special 32-bit CE and its tag, ignoring its data.

\*/

SPECIAL_CE32_AND_TAG_MASK=0xfc000007, // TODO: needed?

/\*\* Special-32-bit-CE-tag mask. \*/

TAG_MASK=7,

/\*\*

\* Mask for determining special 32-bit CE and its tag, ignoring its data.

\*/

LEAD_SURROGATE_SPECIAL_CE32_NO_DATA=0xfc000000|LEAD_SURROGATE_TAG, // TODO:
needed?

/\*\*

\* Data lookup yields no collation element.

\* When found in the UCA table, construct an unassigned-character CE.

\*/

NO_CE32=0xffffffff;

// Special 64-bit CEs.

static int64_t

/\*\*

\* No CE: End of input. Only used in runtime code, not stored in data.

\*/

NO_CE64=1;

// Data structures except for expansion tables store 32-bit CEs which are

// either specials (see tags below) or are compact forms of 64-bit CEs.

// The expansion table stores sequences of 64-bit CEs.

static inline UBool isSpecialCE32(uint32_t ce32) {

return ce32>=((uint32_t)FIRST_SPECIAL_BYTE<<24);

}

static inline int32_t getSpecialCE32Tag(uint32_t ce32) {

return (int32_t)ce32&TAG_MASK;

}

static inline UBool isLeadSurrogateSpecialCE32(uint32_t ce32) { // TODO: needed?

return (ce32&SPECIAL_CE32_AND_TAG_MASK)==LEAD_SURROGATE_SPECIAL_CE32_NO_DATA;

}

static inline UBool hasDataFromLeadSurrogateSpecialCE32(uint32_t ce32) { //
TODO: needed?

return (ce32&8)!=0;

}

/\*\* Returns a 64-bit CE from a non-special 32-bit CE. \*/

static inline int64_t ceFromCE32(uint32_t ce32) {

#if 1

// TODO: this is new; maybe adjust other design doc text

// This should make it easier to distinguish between the three forms.

uint32_t tertiary=ce32&0xff;

if(tertiary>1) {

// normal form ppppsstt -> pppp0000ss00tt00

return ((int64_t)(ce32&0xffff0000)<<32)|((ce32&0xff00)<<16)|(tertiary<<8);

} else if(tertiary==0) {

// long-primary form pppppp00 -> pppppp00050000500

return ((int64_t)ce32<<32)|DEFAULT_SEC_AND_TER_CE;

} else /\* tertiary==1 \*/ {

// long-secondary form sssstt01 -> 00000000sssstt00

return ce32-1;

}

#else

// (this is old)

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

#endif

}

/\*\*

\* Returns the signed Han-implicit offset for c. c must be a Unified_Ideograph.

\* Han characters up to U+3FFFF are supported, with (c+offset) resulting in
indexes

\* that can be turned into 3-byte primary weights with 5 lead bytes.

\*

\* The offsets and index ranges are designed to accommodate full blocks,

\* but the UCA specifies that Han-implicit weights are to be used only

\* for assigned characters.

\*

\* Compatibility ideographs are excluded because they decompose to unified
ideographs.

\* The F900..FAFF compatibility ideographs block contains twelve unified
ideographs

\* among FA0E..FA29.

\*

\* Block code points offset indexes=c+offset

\*

\* Unihan 4E00.. 9FFF - 4E00 0.. 51FF

\* compat FA0E.. FA29 - A80E 5200.. 521B

\* Ext A 3400.. 4DBF + 1E20 5220.. 6BDF

\* Ext B 20000..2A6DF -19420 6BE0..112BF

\* Ext CDE 2A700..2F7FF -19440 112C0..163BF

\* Future Ext 2FA20..3FFFF -19660 163C0..2699F

\*/

static int32_t hanOffsetFromUnifiedIdeograph(UChar32 c) {

if(c<0x4E00) {

return 0x1E20; // Ext A

} else if(c<=0x9FFF) {

return -0x4E00; // Unihan

} else if(c<=0xFA29) {

return -0xA80E; // compat

} else if(c<=0x2A6DF) {

return -0x19420; // Ext B

} else if(c<0x2F800) {

return -0x19440; // Ext CDE

} else {

return -0x19660; // 2FA20..3FFFF

}

}

/\*

\* TODO: Alternative implicit weights?

\* One single lead byte for 3-byte BMP Han implicits,

\* and one lead byte for 4-byte supplementary Han implicits.

\* The 4-byte Han implicits would use the same code as the unassigned-implicits.

\* With one extra parameter, they could have a gap size of 84 (mod 3 for the 4th
byte).

\*

\* Pro: Saves 3 lead bytes.

\* Con: Slightly more code, smaller gaps between Extension B characters.

\* ***Con!!*** 4-byte Han implicits would not fit into long-primary CEs.

\* (E.g., UCA canonical closure & compatibility characters.)

\*/

/\*\*

\* Returns the Han character primary weight for a Unified_Ideograph c

\* where hanIndex=c+hanOffsetFromUnifiedIdeograph(c).

\*/

static uint32_t hanPrimaryFromIndex(int32_t hanIndex) {

// Third byte: 126 values, gap of 1.

uint32_t primary=(4+(hanIndex%126)\*2)<<8;

hanIndex/=126;

// Second byte: 251 values 04..FE excluding the primary compression bytes.

primary|=(4+(hanIndex%251))<<16;

hanIndex/=251;

// First byte: 5 values cover all Han character indexes
(hanIndex<0x269B2=5\*251\*126).

return primary|((FIRST_HAN_BYTE+hanIndex)<<24);

}

// TODO: Tailor before U+4E00 -> same as after \[last regular\], and vice versa.

// TODO: Tailor otherwise before/after UIdeo -> get index, dec/inc, get weight,

// or directly compute prev/next weight from known gaps & encoding.

// TODO: Set \[first implicit\] to hanPrimaryFromIndex(0).

// TODO: Set \[last implicit\] to FIRST_HAN_BYTE+4.FE.FF so that we

// do not accidentally tailor non-Han characters in the last Han lead byte.

// (Or to hanPrimaryFromIndex(0x40000+hanOffsetFromUnifiedIdeograph(0x40000))

// if it does not matter.)

/\*\*

\* Returns the Han character primary weight for a Unified_Ideograph c

\* and its special CE32.

\*/

static inline uint32_t hanPrimaryFromUnifiedIdeographAndCE32(UChar32 c, uint32_t
ce32) {

return hanPrimaryFromIndex(c+((int32_t)ce32<<6)>>9); // signed offset from bits
25..3

}

/\*\*

\* Returns the unassigned-character primary weight for any valid code point c.

\*/

static uint32_t unassignedPrimaryFromCodePoint(UChar32 c) {

// TODO: ++c here would eliminate the requirement that U+0000 not be unassigned,
so that tailoring before the first unassigned-implicit could always use the
initial gap

// Fourth byte: 6 values, every 42nd byte value (gap of 41).

uint32_t primary=4+(c%6)\*42;

c/=6;

// Third byte: 253 values.

primary|=(3+(c%253))<<8;

c/=253;

// Second byte: 251 values 04..FE excluding the primary compression bytes.

primary|=(4+(c%251))<<16;

c/=251;

// First byte: 3 values cover all code points (c<0x11710E=3\*251\*253\*6).

return primary|((FIRST_UNASSIGNED_BYTE+c)<<24);

}

// TODO: This assumes that U+0000 is not unassigned \[TEST\], so that there is a
gap

// before the first actual unassigned code point. Tailor before/after ->

// essentially same as for Han.

// TODO: Set \[first unassigned\] to unassignedPrimaryFromCodePoint(0).

// TODO: Set \[last unassigned\] to FIRST_UNASSIGNED_BYTE+2.FE.FF.FF

// or unassignedPrimaryFromCodePoint(0x110000).

// TODO: Reduce gaps to 28-1 or 14-1 for only two or one lead bytes for
unassigned-implicits?

private:

Collation(); // No instantiation.

};

/\*\*

\* Collation element and character iterator.

\* Handles normalized UTF-16 text inline, with length or NUL-terminated.

\* Other text is handled by subclasses.

\* TODO: Function pointer for tertiary-case-bit twiddling.

\*/

class CollationIterator : public UObject {

public:

CollationIterator(const CollationData \*tailoring, const CollationData
\*ucaData,

const UChar \*s, int32_t length)

: data(tailoring), uca(ucaData),

ces(NULL), cesIndex(-1), cesLength(0),

start(s), pos(s), limit(length>=0 ? s+length : NULL) {

trie=data->getTrie();

}

inline int64_t nextCE() {

if(0<=cesIndex && cesIndex<cesLength) {

// Read a 64-bit CE from a list.

return ces\[cesIndex++\];

}

UChar32 c;

uint32_t ce32;

if(pos!=NULL && pos!=limit) {

// TODO: Make a choice of whether we try to optimize UTF-16 data

// lookup with UTRIE2_GET32_FROM_U16_SINGLE_LEAD()

// which would save one "if(!U16_IS_LEAD(c))" in the common case

// and make this function slightly smaller,

// vs. just using UTRIE2_U16_NEXT32() which avoids a level of

// complexity of dealing with LEAD_SURROGATE_TAG and

// nextCodePointFromLeadSurrogate().

// My hunch is that with all of the other processing here and in

// the string comparison function this optimization is not worth it,

// and Andy agrees that "it's probably in the noise". -- Markus 2010nov02

#if 1

UTRIE2_U16_NEXT32(trie, pos, limit, c, ce32);

if(c==0 && limit==NULL) {

limit=--pos;

return Collation.NO_CE64;

}

#else

// Fastpath:

// Read one UTF-16 code \*unit\* and look up its 32-bit CE value.

// All lead surrogate code units have a special CE.

c=\*pos;

if(c==0 && limit==NULL) {

limit=pos;

return Collation.NO_CE64;

}

++pos;

ce32=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);

#endif

} else {

ce32=handleNextCE32(c);

}

int64_t ce;

if(!Collation.isSpecialCE32(ce32)) {

// Normal CE from the main data.

ce=Collation.ceFromCE32(ce32);

} else if(ce32==Collation.NO_CE32 && uca!=NULL) {

if(c<0) {

return Collation.NO_CE64; // end of input

}

// Fallback to UCA.

ce=uca->nextCE(\*this, c);

} else {

ce=data->nextCEFromSpecialCE32(\*this, c, ce32);

if(ce==Collation.NO_CE64) {

ce=uca->nextCE(\*this, c);

}

}

return ce;

}

inline int64_t previousCE() {

if(cesIndex>0) {

// Read a 64-bit CE from a list.

return ces\[--cesIndex\];

}

UChar32 c;

uint32_t ce32;

if(pos!=NULL && pos!=start) {

UTRIE2_U16_PREV32(trie, start, pos, c, ce32);

} else {

ce32=handlePreviousCE32(c);

if(c<0) {

Collation.NO_CE64;

}

}

int64_t ce;

if(!Collation.isSpecialCE32(ce32)) {

// Normal CE from the main data.

ce=Collation.ceFromCE32(ce32);

} else if(ce32==Collation.NO_CE32 && uca!=NULL) {

// Fallback to UCA.

ce=uca->previousCE(\*this, c);

} else {

ce=data->previousCEFromSpecialCE32(\*this, c, ce32);

if(ce==Collation.NO_CE64) {

ce=uca->previousCE(\*this, c);

}

}

return ce;

}

/\*\*

\* Returns the previous CE when c has an UNSAFE_BACKWARD_TAG.

\* Must only be called by CollationData.previousCEFromSpecialCE32().

\* TODO: Move this function into CollationData?!

\*/

int64_t previousCEUnsafe(UChar32 c) {

// We just move through the input counting safe and unsafe code points

// without collecting the unsafe-backward substring into a buffer and

// switching to it.

// This is to keep the logic simple. Otherwise we would have to handle

// prefix matching going before the backward buffer, switching

// to iteration and back, etc.

// In the most important case of iterating over a normal string,

// reading from the string itself is already maximally fast.

// The only drawback there is that after getting the CEs we always

// skip backward to the safe character rather than switching out

// of a backwardBuffer.

// But this should not be the common case for previousCE(),

// and correctness and maintainability are more important than

// complex optimizations.

saveLimitAndSetAfter(c);

// Find the first safe character before c.

int32_t numBackward=1;

while(c=previousCodePoint()>=0) {

++numBackward;

if(!CollationData.isUnsafeCodePoint(c)) {

break;

}

}

// Ensure that we don't see CEs from a later-in-text expansion.

cesIndex=-1;

ceBuffer.clear();

// Go forward and collect the CEs.

int64_t ce;

while(ce=nextCE()!=Collation.NO_CE64) {

ceBuffer.append(ce);

}

// TODO: assert 0!=ceBuffer.length()

restoreLimit();

backwardNumCodePoints(numBackward);

// Use the collected CEs and return the last one.

ces=ceBuffer.getBuffer();

cesIndex=cesLength=ceBuffer.length();

return ces\[--cesIndex\];

// TODO: Does this method deliver backward-iteration offsets tight enough

// for string search? Is this equivalent to how v1 behaves?

}

inline UChar32 nextCodePoint() {

if(pos!=NULL && pos!=limit) {

UChar32 c=\*pos;

if(c==0 && limit==NULL) {

limit=pos;

return U_SENTINEL;

}

++pos;

UChar trail;

if(U16_IS_LEAD(c) && pos!=limit && U16_IS_TRAIL(trail=\*pos)) {

++pos;

return U16_GET_SUPPLEMENTARY(c, trail);

} else {

return c;

}

}

return handleNextCodePoint();

}

inline UChar32 previousCodePoint() {

if(pos!=NULL && pos!=start) {

UChar32 c=\*--pos;

UChar lead;

if(U16_IS_TRAIL(c) && pos!=start && U16_IS_LEAD(lead=\*(pos-1))) {

--pos;

return U16_GET_SUPPLEMENTARY(lead, c);

} else {

return c;

}

}

return handlePreviousCodePoint();

}

inline void forwardNumCodePoints(int32_t num) {

while(num>0) {

if(pos!=NULL) {

// Go forward in the inner buffer as far as possible.

while(pos!=limit) {

UChar32 c=\*pos;

if(c==0 && limit==NULL) {

limit=pos;

break;

}

++pos;

--num;

if(U16_IS_LEAD(c) && pos!=limit && U16_IS_TRAIL(\*pos)) {

++pos;

}

if(num==0) {

return;

}

}

}

// Try to go forward by one code point via the iterator,

// then return to the inner buffer in case the subclass moved that forward.

if(handleNextCodePoint()<0) {

return;

}

--num;

}

}

inline void backwardNumCodePoints(int32_t num) {

while(num>0) {

if(pos!=NULL) {

// Go backward in the inner buffer as far as possible.

while(pos!=start) {

UChar32 c=\*--pos;

--num;

if(U16_IS_TRAIL(c) && pos!=start && U16_IS_LEAD(\*(pos-1))) {

--pos;

}

if(num==0) {

return;

}

}

}

// Try to go backward by one code point via the iterator,

// then return to the inner buffer in case the subclass moved that backward.

if(handlePreviousCodePoint()<0) {

return;

}

--num;

}

}

/\*\*

\* Returns the code point starting with the given lead surrogate.

\* Reads a trail surrogate from the current position if there is one.

\* Returns a supplementary code point if there is a trail surrogate,

\* or else returns the lead surrogate.

\*

\* Must only be called by CollationData.nextCEFromSpecialCE32()

\* when that was called for a lead surrogate.

\*

\* TODO: Test performance: Is the LEAD_SURROGATE_TAG and its machinery worth it?

\* Test with Latin-1/Chinese text and string comparisons.

\*

\* The intent of the lead-surrogate optimization is this:

\* Using UTRIE2_GET32_FROM_U16_SINGLE_LEAD() in nextCE() (rather than
UTRIE2_U16_NEXT32())

\* should be slightly faster (but it only saves one "if(!U16_IS_LEAD(c))"

\* which might not make enough of a difference)

\* and might(!) help keep the nextCE() function small enough for inlining.

\*

\* If we remove LEAD_SURROGATE_TAG we could use separate special-32-bit-CE tags
again

\* for Han-implicits vs. unassigned-implicits (or have an unused/reserved tag).

\*/

inline UChar32 nextCodePointFromLeadSurrogate(UChar32 lead) {

UChar trail;

if(pos!=NULL) {

// We require whole code points within the start..limit boundaries.

if(pos!=limit && U16_IS_TRAIL(trail=\*pos)) {

++pos;

return U16_GET_SUPPLEMENTARY(lead, trail);

} else {

return lead;

}

}

return handleNextCodePointFromLeadSurrogate(lead);

}

// TODO: setText(start, pos, limit) ?

protected:

/\*\*

\* Returns U_SENTINEL and Collation.NO_CE32 if we are at the end of the input,
or:

\* A subclass may have an implementation reading from an iterator

\* or checking for FCD on the fly etc.

\*/

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

virtual UChar32 handleNextCodePointFromLeadSurrogate(UChar32 lead) {

return lead;

}

/\*\*

\* Saves the current iteration limit for later,

\* and sets it to after c which was read by previousCodePoint() or equivalent.

\*/

virtual void saveLimitAndSetAfter(UChar32 c) {

savedLimit=limit;

limit=pos+U16_LENGTH(c);

}

/\*\* Restores the iteration limit from before saveLimitAndSetAfter(). \*/

virtual void restoreLimit() {

limit=savedLimit;

}

// UTF-16 string pointers.

// limit can be NULL for NUL-terminated strings.

// This class assumes that whole code points are stored within \[start..limit\[.

// That is, a trail surrogate at start or a lead surrogate at limit-1

// will be assumed to be surrogate code points rather than attempting to pair it

// with a surrogate retrieved from the subclass.

const UChar \*start, \*pos, \*limit;

// TODO: getter for limit, so that caller can find out length of NUL-terminated
text?

// Main lookup trie of the data object.

const UTrie2 \*trie; // TODO: protected, for possibly optimized subclass CE
lookup?

// TODO: Do we need to support switching iteration direction?

// If so, then nextCE() and previousCE() must count how many code points

// resulted in their CE or ces\[\], and when they exhaust ces\[\] they need to

// check if the signed code point count is in the right direction;

// if not, move by that much in the opposite direction.

// For example, if previousCE() read 3 code points, set ces\[\],

// and then we switch to nextCE() and it exhausts those ces\[\],

// then we need to skip forward over those 3 code points before reading

// more text.

// We might be able to isolate the direction-switching code in a separate

// function, although the normal code probably has to track the code point
offset.

private:

// List of 32-bit CEs, normal weights for some special CE32s. TODO: Compact
expansions with sequences of non-special CE32s?

// TODO: Probably only in const CollationData.

const uint32_t \*ce32s;

int32_t ce32sIndex, ce32sLength;

// List of 64-bit CEs, for exansions and for CEs that do not fit into CE32.

const int64_t \*ce64s;

int32_t ce64sIndex, ce64sLength;

// Either data is for UCA and uca is NULL, or data is for a tailoring and uca is
not NULL.

// TODO: Subclasses for UCA vs. tailoring with specific code?

const CollationData \*data;

const CollationData \*uca;

// TODO: Flag/getter/setter for CODAN, so that CollationData's special-CE code

// can query it.

// Buffer for skipped combining marks during discontiguous contraction handling.

UnicodeString skipBuffer;

// See saveLimitAndSetAfter();

const UChar \*savedLimit;

// 64-bit-CE buffer for backward iteration and other complicated cases.

// TODO: MaybeStackArray? UVector64?

// TODO: Flag or UErrorCode for memory allocation errors? protected?

};

class FCDCollationIterator : public CollationIterator {

// TODO: On-the-fly FCD check.

// Remember maximum known boundaries of already-FCD text.

};

/\*\*

\* Data container with access functions which walk the data structures.

\*/

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

int64_t nextCEFromSpecialCE32(CollationIterator &iter, UChar32 c, uint32_t ce32)
const {

return Collation.NO_CE; // TODO

// TODO: For UNASSIGNED_IMPLICIT_TAG, if(isFinalData) return unassigned-implict
CE else return NO_CE.

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

int64_t previousCEFromSpecialCE32(CollationIterator &iter, UChar32 c, uint32_t
ce32) const {

return Collation.NO_CE; // TODO

// TODO: For UNASSIGNED_IMPLICIT_TAG, if(isFinalData) return unassigned-implict
CE else return NO_CE.

}

uint32_t getCE32(UChar32 c) { // for CODAN DIGIT_TAG and digit-value lookup

return UTRIE2_GET32(trie, c);

}

UBool isUnsafeCodePoint(UChar32 c) {

if(U_IS_TRAIL(c)) {

return TRUE;

}

return FALSE; // TODO

// TODO: Are all cc!=0 marked as unsafe for prevCE() (because of discontiguous
contractions)?

// TODO: Use a frozen UnicodeSet rather than an imprecise bit set?

}

/\*\*

\* Returns this data object's main UTrie2.

\* To be used only by the CollationIterator constructor.

\*/

const UTrie2 \*getTrie() { return trie; }

private:

// Main lookup trie.

const UTrie2 \*trie;

UBool isFinalData;

};

TODO: CODAN, digits as numbers

When we see a DIGIT_TAG, should we collect a run of digit characters or a run of
characters with DIGIT_TAG?

First primary weight byte: Single-byte weight for '0'.

Second byte: (avoid 03 & FF primary-compression terminators)

Cover numbers with up to 4 digits in super-compact encoding.

4+(0..31): numbers 0..31 // good for days & months

36+(0..39) with third byte 3..255: numbers 32..10151=32+40\*253-1 // good for
years etc.

76..?: number of digit pairs (76->3, 77->4, ...) but with trailing 00s removed
like in v1 code

?..254 with third byte 3..255: longer number of digit pairs to cover unusual
cases; up to some 10000 pairs? \[worth it?\] \[in overflow, just start second
number?\]

Collect only digits, not pairs, until actually writing the CEs.

Verify that we don't handle negative values.
