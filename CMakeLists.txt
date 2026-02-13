cmake_minimum_required (VERSION 3.10)
project (Compiler)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Lib
add_library (compiler_core INTERFACE)
target_include_directories (compiler_core INTERFACE 
    src/common
)

# Exe
add_executable (compiler src/compiler/main.cpp)
target_link_libraries (compiler PRIVATE compiler_core)