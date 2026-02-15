/**
 * @file compiler_tests.cpp
 * @brief Full compiler pipeline (file -> assembly) tests
 */

#include "testbench.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.run_tests ();
    tb.print_results ();
}