# Bit-C Compiler
An itsy Bit-C C compiler that:
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
* int function definitions (no params)
* local variables
    * alphanumeric and underscores

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
./compiler <PATH_TO_FILE> -o <PATH_TO_OUT>
```

## Future Work
* Features
    * Memory (deref, address)
    * Operators (|, &, <<, >>)
    * Structs, unions
    * Function params
    * Function return types
    * Comments (mult/single line)
    * Types (char, float)
* Optimizations (will force me to redo all tests...)
    * Register allocation
    * Constant folding
    * Loop unrolling
<br>
* Assembler
* Linker