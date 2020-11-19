# Collation: ICU4C (4.8) vs. GLIBC (2.13-1)

## Collation: ICU (4.8) vs GLIBC (2.13-1)

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

**Locale** **Data file** **strcoll** *(ICU)* **keygen** *(ICU)* **keylen**
*(ICU)* **strcoll** *(GLIBC)* **keygen** *(GLIBC)* **keylen** *(GLIBC)*
**strcoll** *(GLIBC-ICU)/ICU)* **keygen** *(GLIBC-ICU)/ICU)* **keylen**
*(GLIBC-ICU)/ICU)*
en_USTestNames_Latin.txt485511.4703751275993.043689164.583%8.71143%107.000%
da_DKTestNames_Latin.txt1747451.4714141326203-24.137%-16.778%103.885%
de_DETestNames_Latin.txt495601.4703751246193153.061%10.5357%104.029%
fr_FRTestNames_Latin.txt495621.4703751266003157.142%6.76156%104.029%
ja_JPTestNames_Latin.txt485641.4703751251801.008595160.416%-68.085%-31.405%
ja_JPTestNames_Japanese_h.txt1704713.0666824224884.235949148.235%3.60934%38.1280%
ja_JPTestNames_Japanese_k.txt1785013.0666824084383.872655129.213%-12.574%26.2815%
ja_JPTestNames_Asian.txt1983853.1165342663844.08716934.3434%-0.2597%31.1446%
zh_CNTestNames_Latin.txt1848491.4735161245843.034630-32.608%-31.213%105.944%
zh_CNTestNames_Chinese.txt1532093.3244502853606.15759586.2745%72.2488%85.2214%
zh_CNTestNames_Simplified_Chinese.txt1562203.3249342923606.23267087.1794%63.6363%87.4524%
zh_TWTestNames_Latin.txt1838231.4735161235963.034630-32.786%-27.582%105.944%
zh_TWTestNames_Chinese.txt1532103.3232922893766.24017088.8888%79.0476%87.7707%
ko_KRTestNames_Latin.txt505511.4703751221801.008453144%-67.332%-31.415%
ko_KRTestNames_Korean.txt2513894.3199562401623.325273-4.3824%-58.354%-23.025%
ru_RUTestNames_Latin.txt485681.4703751235933156.25%4.40140%104.029%
ru_RUTestNames_Russian.txt17312231.6827162121801522.5433%47.2608%197.138%
th_THTestNames_Latin.txt486681.4703751235413.764057156.25%-19.011%155.992%
th_THTestNames_Thai.txt2909231.61509638613227.40252033.1034%43.2286%358.333%

### *Notes*

1.  As with all performance measurements, the results will vary according to the
    hardware and compiler. The strcoll operation is particularly sensitive; we
    have found that even slight changes in code alignment can produce 10%
    differences.
2.  For more information on incremental vs. sort key comparison, the importance
    of multi-level sorting, and other features of collation, see [Unicode
    Collation (UCA)](http://www.unicode.org/reports/tr10/).
3.  For general information on ICU collation see [User
    Guide](http://userguide.icu-project.org/).
4.  For information on APIs, see
    [C](http://icu-project.org/apiref/icu4c/ucol_8h.html),
    [C++](http://icu-project.org/apiref/icu4c/classCollator.html), or
    [Java](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/Collator.html).
