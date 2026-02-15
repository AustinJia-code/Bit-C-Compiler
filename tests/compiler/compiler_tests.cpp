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

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
    std::cout << "\n=======================" << std::endl;
}