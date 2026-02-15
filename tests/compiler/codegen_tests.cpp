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

    tb.run_tests ();
    tb.print_results ();
}