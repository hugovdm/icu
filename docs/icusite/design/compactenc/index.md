# Compact Encodings

It is sometimes useful to use a compact encoding of strings to save some storage
space. General-purpose data compression (e.g., zip) can be more effective,
except for when the data consists of short strings or when there are other
limitations (small code, small heap memory usage, limited library dependencies).

ICU converters implement the SCSU and BOCU-1 encoding schemes.

ICU internally implements the Punycode ASCII-Compatible Encoding as part of its
IDNA implementation.

For more specialized compact encodings, see the subpages.

See also [A Survey of Unicode Compression](http://www.unicode.org/notes/tn14/)
