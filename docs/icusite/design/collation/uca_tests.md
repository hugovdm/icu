# UCA 6.0 Tests

Bug [#8009](http://bugs.icu-project.org/trac/ticket/8009)

The goal is to make sure that the structure works as designed AND doesn't
degrade over time (like what we got in the past). So add tests for:

### Function

1. Check that FFFE sorts before all other characters, and FFFF sorts after.

2. Check that other non-characters sort as implicits

3. Check that the sorting is spaces < punctuation < symbols < currency-symbols <
numbers, for all characters < 'a'.

4. Verify that import and script reordering work, including the groups in #3

5. Verify that setting SHIFTED only now affects spaces and punctuation below
'a'. That is, symbols & currency-symbols are unaffected.

### Internal structure

1. Check for a gap between all primaries, secondaries, and tertiaries --
including the new 'final values for script reordering.

2. Check that we don't get too many 3-byte primaries in the actual tailorings.
This can be done by looking at the tailored characters, and getting the
collation elements.

3. Verify that we are getting compressed primaries for all of the scripts with
COMPRESS in FractionalUCA.
