# Compact Encoding of Names

What? Person names, company names, place names, etc.

Problem: Encode name strings in a compact way.

Assumption: Match names case-insensitively. If this is the case, then we can
normalize names using Unicode NFKC_Casefold or similar.

Assumption: We decode more often than encode, and don't need random access on
the encoded byte sequence.

Observations: Names mostly consist of letters, spaces and occasional
punctuation. After normalization, there are no uppercase letters. Names normally
use letters from a single script. No control codes, few format control
characters (probably no RLM/LRM), few symbols.

Ideas:

*   Optimize for high-runner cases, with an escape mode for all of Unicode.
*   For the encoding process: Try out different sub-encodings for a name string,
    choose the one that yields the smallest number of bytes.
*   We probably need to indicate inside the encoded byte sequence which
    sub-encoding we use. (As opposed to out-of-band information.)
*   With names normally using a single script, a single sub-encoding should
    suffice in most cases, without the need to switch mid-stream. (Unlike SCSU
    for example which is designed for switching between modes.)
*   Therefore, the first byte can indicate the sub-encoding used in the whole
    sequence. For the most common sub-encodings, the first byte can also contain
    the first letter of the name, or part of it (a lead byte).
*   Example sub-encodings:
    *   Use 5 bits per character for Latin a-z, space, and some other characters
        commonly used in Latin names. Use 32 values in the first byte to
        indicate this sub-encoding and the first character.
        *   With a five-bit encoding, make sure we can tell which quintet is the
            last. We need to be careful with a trailing 0 quintet. Set some of
            the bits following it, or fail the 5-bit sub-encoding if the last
            quintet is a 0 and there are no spare bits to set.
    *   Use 5 bits per character for the 32 most common Cyrillic letters (out of
        the 33 main letters). Use 32 values in the first byte to indicate this
        sub-encoding and the first character.
    *   Use 5 bits per character for the 32 most common Arabic letters. Use 32
        values in the first byte to indicate this sub-encoding and the first
        character.
    *   Use 8 bits per character for Hiragana & Katakana (U+3040..U+30FF). Throw
        in the 32 characters from 5-bit Latin and maybe U+3000..U+301F.
    *   Maybe use 5 or 6 bits per character for various other small scripts,
        although the majority of names will be covered by sub-encodings listed
        here.
    *   Maybe use 8 bits per character for Indic/Thai/Lao U+0900..U+0EFF where
        the first byte would be one of 6 values indicating the lead byte for the
        remaining truncated character codes. 2..4 byte values (e.g., fc..ff)
        could be reserved for cherry-picked characters like ZWNJ and ZWJ.
    *   Maybe use 12 bits per character for U+0000..U+0FFF for most small
        scripts not covered by more efficient sub-encodings, and as an escape
        valve for Cyrillic and Arabic etc. that don't fit into their 5-bit
        sub-encodings. Maybe throw in a few other common characters like maybe
        ZWJ & ZWNJ, replacing control codes.
    *   Alternative 12-bit-per-character encoding, considering that Latin text
        is better handled by the Unicode mode below:
        *   U+0300..U+07FF -> 0..0x4ff // 0x500 combining marks, Greek,
            Cyrillic, Arabic, Hebrew...
        *   U+0900..U+10FF -> 0x500..0xcff // 0x800 Indic scripts, Thai, Lao,
            Tibetan, Burmese, Georgian
        *   U+1200..U+13FF -> 0xd00..0xeff // 0x200 Ethiopic, Cherokee
        *   U+1780..U+17FD -> 0xf00..0xf7d // 0x7E Khmer
        *   U+200C..U+200D -> 0xf7e..0xf7f // 2 ZWNJ & ZWJ
        *   U+2D80..U+2DDF -> 0xf80..0xfdf // 0x60 Ethiopic Extended
        *   space, a-z, some other characters -> 0xfe0..0xfff // 0x20 same as
            5-bit Latin
    *   Use 2 bytes per character for Unihan (U+4E00..U+9FFF) and Hangul
        (U+AC00..U+D7FF). Use 126 values in the first byte to indicate this
        sub-encoding and the lead byte of the first character. Maybe use another
        26 values in the first byte to also indicate this sub-encoding, but with
        a-z as the first character.
        *   Throw in the 32 characters from 5-bit Latin as single bytes E0..FF,
            pushing rarely-used high-BMP characters to a 3-byte-per-character
            encoding, all while staying in this mostly-16-bit mode. We could
            also use some other single bytes (e.g., D8..DF) for a few other
            common characters like maybe ZWJ & ZWNJ, for 3-byte modes for
            rest-of-BMP and plane 2, and a 4-byte mode for other supplementary
            characters. (Or 17 byte values for rest-of-Unicode indicating bits
            20..16.)
        *   Note also that U+3400..U+4DFF and most of U+2000..U+2FFF occur
            rarely, so (some of) their lead bytes might be used for something
            else too.
        *   These extra modes turn this mode into a reasonable all-Unicode mode.
