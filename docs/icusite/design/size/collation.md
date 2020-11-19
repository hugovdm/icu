# Collation Resource Bundles

A substantial part of ICU's data is in collation resource bundles. In an
almost-ICU 4.2 build, they represent about a quarter of the total data size of
15.2MB.

genrb recognizes a collation tailoring rule, creates a Collator from it, and
adds its binary data into the resource bundle.

## Ideas for reduced size

### Binary collation data

We have several dozen files with **very simple rules that result in 14kB or
larger binaries. One could omit these binaries**, if the runtime code has the
builder code and the necessary data files (especially invuca.icu). This could be
done manually in a makefile with an existing genrb command-line option, or a new
option could decide based on the length or complexity of the rules string
whether to store the binary. A build (makefile) could always supply this new
option, and genrb manages when to include the binary. This would **save about
800kB. Cost:** Runtime code needs to include the collation builder and
invuca.icu, slower loading of a collator, more heap memory usage.

We could review and modify the data format for collation binaries.

#### Latin-1

With data for Latin-1 but very few other characters the size of the binary data
should be closer to **6 or 7kB not 14kB**.

I believe that we copy the Latin-1 data from the UCA table into the tailoring
table, for Latin-1 characters that would fall back to the UCA.

I believe that the collation runtime code already allocates and fills a Latin-1
CE table which changes depending on the runtime attributes. If this is the case,
then **copying the Latin-1 data from the UCA table may be unnecessary**.

If there is no data outside of the Latin-1 range, then we **may not need the
UTrie** at all. Or, we could have a canned UTrie/UTrie2 index array that is used
when only a certain set of characters is tailored, and in that case we do not
store a complete UTrie(2) but only its data32 array. Problem: If we adopt the
European Ordering Rules (EOR) and they tailor many non-Latin-1 characters, then
many of these tailorings will not fit these optimizations.

#### CJK

Investigate how dense or sparse the CJK data is in the UTrie. Is there an
opportunity for a different, CJK-specific data structure? For example, storing
only **16-bit primary weights** for Han characters? (With a separate 16-bit
UTrie for characters that fit, or a flat table for Unihan and/or Hangul?)

We might be able to ==always== store the top and bottom 16 bits of a CE in **two
separate 16-bit-value tries**. This would eliminate special checks for
size-optimized character ranges and whether there is a special trie, and it
should not have a performance impact in string comparisons that are decided by
primary differences. The bottom-16 trie would be very small if most of the CEs
have default values for secondary and tertiary weights. **Cost:** Generating
sort keys would be a bit slower, but we probably care more about sort key
*length* (which would not change) than generation time. We would have to always
store two tries, which adds overhead and probably negates the savings for small
tailorings.

Note: CLDR 1.9/ICU 4.6 tailors many more characters in several CJK tailorings.
While the range above \[last regular\] got larger again, some of the tailorings
still require 3-byte primary weights.

#### General

In addition to the CE mappings (main trie, contraction and expansion tables) we
also have additional data for certain operations, like bit sets to optimize
backward iteration. We could omit these and build them at load time. Or we could
**omit them only if they are the same as the ones in the UCA data**, and the
runtime could point there.

Maybe we could store small and sparse binary results (output from the collation
builder) in a **simple data format** that's much smaller than the full form, for
example a list of mappings from strings to CE sequences. At runtime, we would
need the UTrie2 builder and enough code to build expansion and contraction
tables from precomputed data, but not the full collation builder nor the
invuca.icu data, canonical equivalence code, etc. We could store this data under
a separate bundle key to avoid format versioning. We could even store this data
in a resource item tree (array of strings parallel with CE intvector, etc.)
instead of a custom binary. This could **save a significant amount of data.
Cost:** Runtime code needs UTrie and contraction/expansion table builders,
somewhat slower loading of a collator, more heap memory usage.

Vladimir's idea: For languages with small scripts (e.g., Cyrillic or Thai) we
might be able to use a **short, linear table** mapping a single range of code
points to CEs, to avoid the trie. Or maybe use two or three ranges, especially
if one of them is in redundant Latin-1 anyway and is only used at load time to
build the special Latin-1 table. Use the linear table(s) directly at runtime,
instead of building a trie at load time. **Cost:** Trades size for performance,
although *it might actually be faster* than the trie if the collator sees
characters in the linear range. It just adds yet another test into the chain of
where-is-the-data-for-this-character. Explore using a function pointer, or C++
subclass with virtual functions, to switch between variants of how CEs are
stored and accessed?

#### Note

We should make incompatible changes at the same time that we switch from the old
UTrie to the new UTrie2, which will anyway require a **major-version change**.

### Tailoring Rule Strings

For some of the rule strings, ICU could adopt **new syntax like what CLDR has**,
for a more compact representation of a sequence of characters with primary
differences from a reset point. This should significantly reduce the CJK rule
strings — by more than 50%! We would have to try it out to get exact numbers.

*   **Done in ICU 4.6**
*   We could even use special characters inside this syntax to further
    **abbreviate sequences of consecutive characters**; for example, we could
    abbreviate <20000 20001 20002 20003> as <20000 F802> where U+F802 would mean
    "3 consecutive code points following the previous one". (Either
    noncharacters or PUA might be appropriate.)

We have three near-identical copies of the **Unihan collation rules**, in the
ja/ko/zh collation bundles. genrb could auto-detect their large, common suffix
(starting with "&\[last regular \]"), store it in a separate coll/unihan.res
bundle and replace that suffix in the CJK bundles with something like
"\[unihan\]". **Cost:** At runtime, when someone asks for the unihan rule string
(which is rare), the string needs to be allocated/concatenated from the distinct
pieces and the common suffix.

*   Mark has a better idea: Both CLDR and ICU should add intra-rules syntax to
    **import another rule string**. It should be able to refer to a string in
    another bundle. This should be used for both European Ordering Rules (EOR)
    and the large Unihan rule string. The ICU API should return the rule string
    including that import syntax, without replacing it with the referred-to
    string value. Old code would not be able to parse this, but we don't
    guarantee not to add syntax. There would be no runtime cost because the rule
    string would still be memory-mapped. The collation rule parser could either
    temporarily build the full string, or, better, just load the substring and
    temporarily branch into it.
*   **Done in ICU 4.6**

We could use the **pre-ICU 1.8 syntax** of ';' and ',' instead of "<<" and
"<<<". (See http://userguide.icu-project.org/collation/customization)

Is it feasible to **omit the rule strings and build them at runtime**, on
demand? That would remove all of the very large strings in ICU data. (Mark has
some ideas for how this could be done. We would at least have to store some data
for the reset points. Might only be worth doing for large rules.)

We could store separate, **parallel trees** of collation resource bundles: One
only with binaries, one only with rule strings. It would be easy to not include
one or the other.

*   Large rule strings could be **zip-compressed** which could be more effective
    than the simple compression scheme outlined on the
    [Strings](strings/index.md) page; either one file for all zip-compressed
    rule strings, or zip-compress whole-bundle contents, or zip-compress
    individual, large string values (the latter is probably the simplest).
*   Results from an experiment with ICU 4.2's coll/zh.txt which mostly contains
    the tailoring rules.
    *   With original UTF-16 string storage and duplicate elimination, there is
        577kB in string values.
    *   With simple [Strings](strings/index.md) compression, these are reduced
        to 412kB.
    *   General-purpose compression of the .txt file reduces it to: zip/gzip:
        224kB, bz2: 211kB, 7zip-fast: 175kB, 7zip-normal: 126kB
    *   The more complex compression methods require several MB of heap memory
        for decompression.

### Other

The **Unihan** collation data is large, and mostly but not completely the same
in the **three CJK collation resource bundles**. Is there any way to share the
common data?

We have more than **180 empty files** "so genrb doesn't issue warnings" (like
the fallback warning). Removing these would save about 10kB. We would have to
redesign how to determine whether we support a language/locale in collation.
