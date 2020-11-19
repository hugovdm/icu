# Transliteration Resource Bundles

translit/root.txt is unusual because a few years ago we pulled the separate
transliterator source files all into root.

If we pull them apart again, then

*   Per-bundle **string value compression** will work much better (because the
    top 3 "windows" per bundle will cover a larger share of the characters).
*   It would be easy again to **pick and choose transliterator files**, rather
    than editing translit/root.txt.
*   **It would save duplication** from where translit/root.txt includes a number
    of rules twice in the file:

Arabic-Latin {

file {

resource:process(transliterator) {"Arabic_Latin.txt"}

direction {"FORWARD"}

}

}

Latin-Arabic {

file {

resource:process(transliterator) {"Arabic_Latin.txt"}

direction {"REVERSE"}

}

}
