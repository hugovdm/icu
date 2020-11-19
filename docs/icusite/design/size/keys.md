# Resource Bundle Keys

The number of bytes we use to store keys is actually a significant part of the
data, as you can see by the information on the right. So it would pay us to slim
those down. Markus and I brainstormed about this, and have the following
thoughts.

**Status: Much of this is implemented in ICU 4.4.**

## Ideas for Size Reduction

### Common Key Storage

We create a common KEY res file with the most common keys. ✓

We could introduce a new TABLE type, or simply increment the formatVersion and
use the top bit in each of the two existing table types to distinguish between
local and pooled keys. (It would be good to be able to use 16-bit key offsets as
much as possible; tests show that the size of this offset makes a significant
difference.) ✓ (4.4: threshold value to distinguish local vs. pool keys)

The key offsets in this table type can either come from the file or from a
common KEY res file. If the offset is less than MAX_KEY, then it comes out of
the KEY file; if not, then offset-MAX_KEY is used to get the key from the
current file. MAX_KEY is found in the KEY file. ✓

Run genrb twice. The first time we supply a special parameter to build the KEY
file. We gather all the key data, and build the KEY res file, containing just
unique keys. The KEY res file also contains MAX_KEY value. ✓ (pool.res)

*   We might restrict this, by running over just the common files, or not
    storing keys that only occur in a single file. But I don't think we actually
    need to worry about this.

The second time we run genrb, we supply the KEY res file as an input parameter.
Whenever a key in a TABLE occurs in that KEY file, we use the new TABLE type. ✓

The KEY res file could be either root.res itself or a separate, new file. With
root.res, we would not need to load yet another file, but one could not update
the root bundle without updating all of the other bundles in the locale
hierarchy. ✓ (pool.res separate from root.res)

Note that the KEY file is required to be part of ICU whenever any other res file
is optimized to use it. Old res files, or res files that are not optimized using
it, work as before.

Judging from a prototype, this will **save about 420kB** (on top of
suffix-sharing, see below) when using two pool.res files, one for locale data
and one for collation data.

### Issue

While swapping resource bundle data between ASCII and EBCDIC, the KEY file must
be present because the **keys in a table are sorted in binary string order**,
which changes between ASCII and EBCDIC. This might be a problem.

One way out would be to always sort strings in ASCII order. On an EBCDIC
platform, the lookup could swap the key into a stack buffer before doing the
binary search. For pathologically long keys, there are a few options:

*   this might require a heap allocation, or
*   per-character swapping during each string comparison, or
*   a limitation of the maximum key length (in both genrb and runtime code) to
    some reasonable number like 63 or 127 characters. If the key is too large it
    could return just *not found*; and genrb could generate an error if someone
    tries to compile a key that was too long.

✓ (4.4: ASCII order, on EBCDIC with per-character swapping during string
comparison)

Theoretically, we could impose a charset-independent key sort order for all
platforms, but it's probably not worth using an order different from ASCII.

More theoretically, we could require the runtime code to read all keys in a
table and copy-and-sort or build a hashtable — and not sort the keys in the
binary data at all. That would always require some overhead and heap allocation
similar to the string value compression.

Using the same sort order (or none) for all platforms would greatly simplify
swapping of resource bundles by avoiding the resorting of tables. ✓

#### Options

The key file should already be so small (17Kish - see right column) that it
isn't worth going to great efforts to reduce it. But here are some ideas in case
we want to think about them.

1.  When we build the Key file, we can have suffix string sharing. That is, if
    "abc<null>" is at offset 57, we don't need to also store "bc<null>"; we can
    just use offset 58. This saves about 7.5% of the size of the file: 1.3Kb. ✓
2.  ...

### Single File Key Storage

Alternatively, try to compress within a single file.

1.  Avoid duplicate keys ✓
2.  Use suffix sharing (see above) (includes duplicate elimination).
    *   ✓ Implemented, **saves some 220kB** over 660 bundles!
3.  Pack short strings into the offset. Idea is to make the offset 32 bits, and
    pack up to 5 6-bit chars into it. Top bit = 1 indicates packed format.
    *   Prototyped, *increases* data size by 33kB over 660 bundles because all
        tables go from 16-bit offsets to 32-bit offsets which outweighs the
        savings.
    *   Rejected
4.  Use shorter keys so that most of them fit into the packed form. This means
    changing LDML2ICUConverter and hardcoded keys in the runtime code. (Not yet
    implemented as of ICU 4.4)

In the packed form, we can have a-zA-Z0-9 and two punctuation characters. We
should encode colon ':' as 0 because the leading or trailing character must not
have a 0 in its 6-bit code (otherwise we would have to encode the length
explicitly), and the colon does not occur as a key-initial nor key-final
character. We can encode underscore '_' as 1 followed by the letters and digits.

Note: The ICU API can return key string pointers (although it is more common to
use a key only for lookup). For packed keys, we will have to write a cache very
similar to the string value cache discussed on the [Strings
page](strings/index.md).

Statistics for non-alnum key characters, from genrb processing all 660 resource
bundles in an almost-ICU-4.2 build:

\*\*\* non-alnum key chars (anywhere in the key):

':' = 0x3a: 12047

'_' = 0x5f: 4595

'-' = 0x2d: 543

'%' = 0x25: 248

' ' = 0x20: 208

'/' = 0x2f: 25

'+' = 0x2b: 24

'.' = 0x2e: 9

')' = 0x29: 3

'(' = 0x28: 3

\*\*\* non-alnum key chars as first key chars:

'_' = 0x5f: 154

'-' = 0x2d: 134

'%' = 0x25: 124

\*\*\* non-alnum key chars as last key chars:

'_' = 0x5f: 154

')' = 0x29: 3

## Background Information

The Keys are used for the TABLE type. There is a 16-bit offset to the actual
value, which is null terminated. They are not currently shared.
The data is significant. In ICU4J, for example, a quick test shows the
following:

Total Count: 2,409
Total Size: 728,764 bytes
Total Unique Size: 17,187 bytes

That is with a terminating null in each case.
Here are the top contenders:
Count Key
13780 0
13648 1
6910 ec
5551 other
4507 one
2698 ls
1676 ld
1324 2
1280 3
1251 few
953 4
951 5
951 6
921 many
871 lg
732 M
695 7
695 8
693 9
693 10
692 11
563 dn
539 cu
531 d
508 Version
471 y
399 wide
385 abbreviated
356 ss
317 format
313 calendar
294 sd
263 gregorian
Here are the frequencies of characters in keys:
1 43655 e
2 40497 a
3 28254 r
4 25287 o
5 24928 n
6 23643 t
7 21948 i
8 21939 1
9 19295 c
10 18541 s
11 17717 0
12 16004 m
13 15452 l
14 13024 h
15 12617 A
16 11120 M
17 10813 d
18 10215 : !Alphanum
19 10140 u
20 7780 S
21 7401 y
22 7161 g
23 6720 E
24 6498 P
25 6273 D
26 6170 B
27 6105 R
28 6104 C
29 5717 k
30 5662 T
31 5418 b
32 5374 L
33 5284 N
34 5201 G
35 4908 p
36 4891 2
37 4827 f
38 4638 K
39 4007 w
40 3997 3
41 3712 U
42 3495 I
43 3428 5
44 3383 H
45 3298 4
46 3285 F
47 3223 _ !Alphanum
48 3128 O
49 2853 6
50 2801 V
51 2702 Z
52 2682 9
53 2534 v
54 2467 7
55 2350 Y
56 2318 8
57 1891 W
58 1876 z
59 1817 X
60 1519 J
61 1197 Q
62 1126 j
63 978 x
64 700 q
65 421 - !Alphanum
66 164 % !Alphanum
