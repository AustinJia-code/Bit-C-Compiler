# Compiler
Tiny C compiler that:
* lexes
* parses
* builds AST
* emits x86-64 

Supported Ops:
* integers
* \+ \- \* \/
* comparison (<, >, ==)
* if
* while
* return
* function definitions
* local variables

## Build & Run
```
cd build
cmake ..
make
./compiler <PATH_TO_FILE>
```

## Tests
Source files compiled with:
1. ```gcc -S <test>.c -o <test>.s```
2. ```gcc <test>.s -o <test>```

View return value with ```echo $?```

## Future Work
* Assembler
* Linker