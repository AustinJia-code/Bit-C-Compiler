cmake_minimum_required (VERSION 3.10)
project (Compiler)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Libs
add_library (test_core INTERFACE)
target_include_directories (test_core INTERFACE
    tests/
)

add_library (compiler_core
    src/compiler/lexer.cpp
)
target_include_directories (compiler_core PUBLIC
    src/common
    src/compiler
)

# Exe
add_executable (compiler src/compiler/main.cpp)
target_link_libraries (compiler PRIVATE compiler_core)

# Tests
add_executable (lexer_tests tests/compiler/lexer_tests.cpp)
target_link_libraries (lexer_tests PRIVATE compiler_core test_core)