/**
 * @file lexer_tests.cpp
 * @brief Tests for the Lexer class
 */

#include <vector>
#include <iostream>
#include <testbench.hpp>
#include <lexer.hpp>
#include <string>

TestResult fts_1 ()
{
    Lexer lexer {};

    std::string out = lexer.file_to_string ("tests/compiler/data/sentence.txt");
    
    return TestResult {.name = "file_to_string",
                       .pass = (out == "This is a \nnew line.")};
}

TestResult fts_2 ()
{
    Lexer lexer {};
    std::string out = lexer.file_to_string ("bad/path");

    return TestResult {.name = "file_to_string with bad path",
                       .pass = out.empty ()};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_test (fts_1);
    tb.add_test (fts_2);

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
}