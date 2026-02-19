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
    src/compiler/parser.cpp
    src/compiler/codegen.cpp
    src/compiler/optimizer.cpp
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

add_executable (parser_tests tests/compiler/parser_tests.cpp)
target_link_libraries (parser_tests PRIVATE compiler_core test_core)

add_executable (codegen_tests tests/compiler/codegen_tests.cpp)
target_link_libraries (codegen_tests PRIVATE compiler_core test_core)

add_executable (compiler_tests tests/compiler/compiler_tests.cpp)
target_link_libraries (compiler_tests PRIVATE compiler_core test_core)

add_executable (optimizer_tests tests/compiler/optimizer_tests.cpp)
target_link_libraries (optimizer_tests PRIVATE compiler_core test_core)

add_executable (file_utils_tests tests/common/file_utils_tests.cpp)
target_include_directories (file_utils_tests PRIVATE src/common)
target_link_libraries (file_utils_tests PRIVATE test_core)