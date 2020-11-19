# Transforms

Issues found:

Replaceable should have: extends CharSequence

Transliterator needs to clean up the threading issues. See
[#8234](http://bugs.icu-project.org/trac/ticket/8234) [Thread-safety hole in
Transliterator](http://bugs.icu-project.org/trac/ticket/8234)

getSource/TargetSet fail. See
[#8227](http://bugs.icu-project.org/trac/ticket/8227) [Transliterator failures
with getSource/TargetSet](http://bugs.icu-project.org/trac/ticket/8227)
