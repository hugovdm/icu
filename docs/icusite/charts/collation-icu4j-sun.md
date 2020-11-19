# Collation: ICU4J (53.1) vs Java 7

The performance test takes a locale and creates a RuleBasedCollator with default
options. A large list of names is used as data in each test, where the names
vary according to language. Each Collation operation over the whole list is
repeated 1000 times. The percentage values in the final column are the most
useful. They measure differences, where positive is better for ICU4J, and
negative is better for the compared implementation.

### Key

Operation Units Description strcoll nanosecs Timing for string collation, an
incremental compare of strings. keygen nanosecs Timing for generation of sort
keys, used to 'precompile' information so that subsequent operations can use
binary comparison. keylen bytes/char The average length of the generated sort
keys, in bytes per character (Unicode/ISO 10646 code point). Generally this is
the important field for sort key performance, since it directly impacts the time
necessary for binary comparison, and the overhead of memory usage and retrieval
time for sort keys.

### Data

-------- ICU -------- ------ JAVA ------- (JAVA - ICU)/ICU
Locale Data file strcoll keygen keylen strcoll keygen keylen coll keygen keylen
------------------------------------------------------------------------------------------------------------
en_US TestNames_Latin.txt | 37 668 1.53 | 626 3323 5.92 | 1592% 397% 287%
da_DK TestNames_Latin.txt | 43 844 1.53 | 1464 5366 5.91 | 3305% 536% 286%
de_DE TestNames_Latin.txt | 35 630 1.53 | 631 3280 5.92 | 1703% 421% 287%
de__PHONEB TestNames_Latin.txt | 42 744 1.53 | 621 3269 5.92 | 1379% 339% 287%
fr_FR TestNames_Latin.txt | 36 636 1.53 | 618 3770 5.92 | 1617% 493% 287%
ja_JP TestNames_Latin.txt | 35 651 1.53 | 653 3333 5.92 | 1766% 412% 287%
ja_JP TestNames_Japanese_h.txt | 76 593 3.11 | 348 1931 6.68 | 358% 226% 115%
ja_JP TestNames_Japanese_k.txt | 76 625 3.11 | 915 3915 6.68 | 1104% 526% 115%
ja_JP TestNames_Asian.txt | 84 562 3.21 | 473 2364 6.81 | 463% 321% 112%
zh_CN TestNames_Latin.txt | 49 819 1.53 | 615 3274 5.92 | 1155% 300% 287%
zh_CN TestNames_Chinese.txt | 64 370 3.68 | 355 1079 9.43 | 455% 192% 156%
zh_TW TestNames_Latin.txt | 47 825 1.53 | 616 3319 5.92 | 1211% 302% 287%
zh_TW TestNames_Chinese.txt | 67 355 3.67 | 435 1017 7.34 | 549% 186% 100%
zh__PINYIN TestNames_Latin.txt | 50 852 1.53 | 623 3339 5.92 | 1146% 292% 287%
zh__PINYIN TestNames_Chinese.txt | 63 353 3.68 | 354 1118 9.43 | 462% 217% 156%
ko_KR TestNames_Latin.txt | 35 636 1.53 | 653 3373 5.92 | 1766% 430% 287%
ko_KR TestNames_Korean.txt | 115 506 4.66 | 414 995 7.33 | 260% 97% 57%
ru_RU TestNames_Latin.txt | 35 649 1.53 | 618 3281 5.92 | 1666% 406% 287%
ru_RU TestNames_Russian.txt | 67 1209 1.55 | 618 6381 5.85 | 822% 428% 277%
th TestNames_Latin.txt | 39 689 1.38 | 620 3316 5.90 | 1490% 381% 328%
th TestNames_Thai.txt | 164 1041 1.42 | 635 3307 5.76 | 287% 218% 306%

### *Notes*

1.  The tests are performed on a Core i7 machine, 3.4GHz with 16GB RAM, running
    Windows 8.1.
2.  The Java version is Oracle Java SE 7u51 (Windows x64).
3.  We were responsible for the original implementation of collation in Java, so
    we've learned quite a bit since then.
4.  For more information on incremental vs. sort key comparison, the importance
    of multi-level sorting, and other features of collation, see [Unicode
    Collation (UCA)](http://www.unicode.org/reports/tr10/).
5.  For general information on ICU collation see [User
    Guide](http://userguide.icu-project.org/collation).
6.  For information on APIs, see
    [C](http://icu-project.org/apiref/icu4c/ucol_8h.html),
    [C++](http://icu-project.org/apiref/icu4c/classCollator.html), or
    [Java](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/Collator.html).
