# C/C++ Debugging

## gdb pretty-printing

Shane wrote this gdb script in 2017: It pretty-prints UnicodeString in GDB.
Instead of seeing the raw internals of UnicodeString, you will see the length,
storage type, and content of the UnicodeString in your debugger. There are
installation instructions in the top comment on the file (it's a matter of
downloading the file and adding a line to ~/.gdbinit).

> <https://gist.github.com/sffc/7b3826fd67cb78057a9e66f2b350a647>

This also works in anything that wraps GDB, like CLion and Visual Studio Code.
