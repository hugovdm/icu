# Collation: ICU4C 3.8 vs. glibc-2.12.3-2

The performance test takes a locale and creates a RuleBasedCollator with default
options. A large list of names is used as data in each test, where the names
vary according to language. Each Collation operation over the whole list is
repeated 1000 times. The percentage values in the final column are the most
useful. They measure differences, where positive is better for ICU4C, and
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

-------- ICU -------- ------ glibc ------- (GLIBC - ICU)/ICU
Locale Data file strcoll keygen keylen strcoll keygen keylen coll keygen keylen
------------------------------------------------------------------------------------------------------------
en_US TestNames_Latin.txt | 84 665 1.52 | 251 1175 3.04 | 199% 77% 99%
da_DK TestNames_Latin.txt | 155 936 1.53 | 263 1194 3.00 | 70% 28% 96%
de_DE TestNames_Latin.txt | 84 662 1.52 | 251 1175 3.00 | 199% 77% 96%
fr_FR TestNames_Latin.txt | 84 1012 1.52 | 252 1178 3.00 | 200% 16% 96%
ja_JP TestNames_Latin.txt | 84 668 1.52 | 248 440 1.00 | 195% -34% -34%
ja_JP TestNames_Japanese_h.txt | 159 512 3.19 | 829 981 4.23 | 421% 92% 33%
ja_JP TestNames_Japanese_k.txt | 164 505 3.19 | 798 901 3.87 | 387% 78% 21%
ja_JP TestNames_Asian.txt | 175 426 3.23 | 524 769 4.08 | 199% 81% 26%
zh_CN TestNames_Latin.txt | 103 1048 1.53 | 251 1171 3.03 | 144% 12% 98%
zh_CN TestNames_Chinese.txt | 172 297 3.79 | 516 644 6.20 | 200% 117% 64%
zh_CN TestNames_Simplified_Chin| 170 295 3.79 | 534 642 6.26 | 214% 118% 65%
zh_TW TestNames_Latin.txt | 103 1055 1.53 | 251 1171 3.03 | 144% 11% 98%
zh_TW TestNames_Chinese.txt | 137 231 3.32 | 527 649 6.24 | 285% 181% 88%
ko_KR TestNames_Latin.txt | 85 665 1.52 | 248 440 1.00 | 192% -34% -34%
ko_KR TestNames_Korean.txt | 240 426 4.32 | 457 318 3.32 | 90% -25% -23%
ru_RU TestNames_Latin.txt | 84 663 1.52 | 250 1171 3.00 | 198% 77% 96%
ru_RU TestNames_Russian.txt | 149 1228 1.62 | 440 3423 5.00 | 195% 179% 208%
th_TH TestNames_Latin.txt | 85 994 1.61 | 251 1196 3.76 | 195% 20% 133%
th_TH TestNames_Thai.txt | 311 1444 1.61 | 792 2912 7.40 | 155% 102% 359%

### *Notes*

1.  As with all performance measurements, the results will vary according to the
    hardware and compiler. The strcoll operation is particularly sensitive; we
    have found that even slight changes in code alignment can produce 10%
    differences.
2.  For more information on incremental vs. sort key comparison, the importance
    of multi-level sorting, and other features of collation, see [Unicode
    Collation (UCA)](http://www.unicode.org/reports/tr10/).
3.  For general information on ICU collation see the [User
    Guide.](http://userguide.icu-project.org/)
4.  For information on APIs, see
    [C](http://icu-project.org/apiref/icu4c/ucol_8h.html),
    [C++](http://icu-project.org/apiref/icu4c/classCollator.html), or
    [Java](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/Collator.html).
