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
    * lowercase ascii and underscores
    * Max 32 chars

## Build & Run
1. Replace path in ```src/common/paths.hpp```
2. Build:
```
cd build
cmake ..
make
```
3. Run:
```
./compiler <PATH_TO_FILE>
```

## Future Work
* Assembler
* Linker