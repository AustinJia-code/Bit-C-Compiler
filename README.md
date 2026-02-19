# Bit-C Compiler
An itsy Bit-C C compiler that:
* lexes
* parses
* builds AST
* Optionally optimizes constant ints, dead branches
* emits x86-64 

Supported Ops:
* integers
* \+ \- \* \/
* comparison (<, >, ==)
* if
* while
* return
* int function definitions with int params
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
    * Safety
    * Memory (deref, address)
    * Operators (|, &, <<, >>)
    * Structs, unions
    * Comments (mult/single line)
    * Types (char, float)
    * Else if and else
    * Switch statements (jump tables seem interesting...)
* Optimizations
    * Register allocation
    * Loop unrolling
* Assembler
* Linker
