# Splitter

<http://bugs.icu-project.org/trac/ticket/8839>

It is much faster to split with a UnicodeSet than with a regex. Here are some
figures:

> String.split Timing /\[,;\]/ 767ns

> Pattern.split Timing /\[,;\]/ 482ns

> UnicodeSet.split Timing /\[,;\]/ 282ns

> String.split Timing /\\p{P}/ 1,114ns

> Pattern.split Timing /\\p{P}/ 847ns

> UnicodeSet.split Timing /\\p{P}/ 281ns

> 10,000,000 iterations, with warmup.

This discusses a draft for an API. Feedback is welcome.

---

The idea is to have a Splitter base class, and two concrete implementations.

*   UnicodeSetSplitter
*   RegexSplitter

*Questions:*

*   *use a builder, or have freeze(), or leave mutable?*
*   *for UnicodeSetSplitter, we could freeze the input, clone and freeze, or
    leave alone (faster if client supplies frozen set).*

The return values (depending on language and which method you call) would be:

*   Java: List<String> or String\[\] (input could be a CharSequence anyway)
    *   Or an Iterator<String> which would expose the most natural
        implementation
    *   A Java Iterator and a C++ StringEnumeration could split the input string
        on the fly. If the caller stops iterating, then no further boundary
        search and object creation is wasted. Only when
        StringEnumeration::count() is called would it have to split and cache to
        the end.
*   C++: StringEnumeration or array+count (input parameter: array of
    UnicodeString pointers to be filled in)

**Settings**

Examples for illustrating settings.

> **pattern =** \[,;\]

> **text =** "ab ; cd ,"

1.  Normal
    *   \["ab ", " cd ", ""\]
2.  Maximum number of resulting items:
    *   maximumCount=1 results in \["ab ", " cd ,"\]
    *   *Question: return the rest of the string in an extra element?*
3.  Should each string be trimmed or not (supplying a UnicodeSet for the
    trimming)
    *   trim=\[:space:\] results in \["ab", "cd", ""\]
4.  Omit trailing strings (Perl/Java compat mode)
    *   omitTrailing=true results in \["ab ", " cd "\]
5.  Include separators
    1.  includeSeparators=true results in \["ab ", ";", " cd ", ",", ""\]
    2.  odd elements are the separators found.

The longest match is used. So with the UnicodeSet \[< = > {<=} {>=}\], then
"a<=b" results in \["a", "b"\]

**Possible future settings**

**(but would need use cases).**

UnicodeSet only (can already do with Regex)

1.  match shortest : "a<=b" => \["a", "", "b"\] // using above example
2.  match
    "[SIMPLE](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/UnicodeSet.SpanCondition.html#SIMPLE)"
    sequence of separators: \[,;\] with "a;,;b" results in \["a", "b"\]
3.  match
    "[CONTAINED](http://icu-project.org/apiref/icu4j/com/ibm/icu/text/UnicodeSet.SpanCondition.html#CONTAINED)"
    sequence of separators: \[{ab}, {abc}, {cd}\] with "xabcdy" results in
    \["x", "y"\]
