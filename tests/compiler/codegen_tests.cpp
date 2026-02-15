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

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
    std::cout << "\n=======================" << std::endl;
}