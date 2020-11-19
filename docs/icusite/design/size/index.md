# Size Reduction

The current goal is to reduce the size of ICU data, and make it more modular.
Look at each of the subpages below for current ideas on how to do this.

## Some Comments

Steven Loomis wrote in an email on 20090501:

*   many of these thoughts correspond at least in a vague way to extant ICU bugs
    - it would be good to collect and note these. String pooling, compression.
*   'so genrb doesn't complain' has to do with validSubLocales, fallback, and
    dependency tracking. These could be handled in an 'out of band' way perhaps,
    but might make the bundle a bit less configurable/separable Of course,
    having one shared .res file with keys that all locales depend on, would do
    this as well.
*   we might also consider supporting jar=zip/tar format for the .dat files.
