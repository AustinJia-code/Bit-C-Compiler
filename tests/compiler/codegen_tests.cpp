/**
 * @file codegen_tests.cpp
 * @brief Isolated tests for the Codegen class
 */

#include "testbench.hpp"
#include "codegen.hpp"

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    // Too lazy to do these... full compiler tests should be enough

    tb.run_tests ();
    tb.print_results ();
}