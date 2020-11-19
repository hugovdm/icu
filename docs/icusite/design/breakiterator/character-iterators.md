# Character Iterators

There are different ways to break up a string into items that users think of as
characters, which can be used for different purposes. Here is a rough
description:

1.  Code points
2.  Spacing Units: don't break conjoining jamo or non-spacing marks
3.  Current grapheme clusters: also don't break spacing combining marks,
    prepending marks (e.g., prevowels)
4.  Akshas: also don't break between Viramas and following consonants.

We currently offer #1 and #3 in ICU (and CLDR). #1 is not through a break
iterator, but is easy.
We could also offer #2 and #4 in ICU, or we could document how people can do it
themselves.
