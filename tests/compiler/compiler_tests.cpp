/**
 * @file compiler_tests.cpp
 * @brief Full compiler pipeline (file -> assembly) tests
 */

#include "testbench.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "file_utils.hpp"

/**
 * gen: no main
 */
TestResult gen_no_main ()
{
    Lexer lexer {"int bad_func () { return 1; }", false};
    Parser parser {lexer.get_tokens ()};

    try
    {
        Codegen cg {parser.parse ()};
        return TestResult {.name = "gen no main", .pass = false};
    }
    catch (const GenError&)
    {
        return TestResult {.name = "gen no main", .pass = true};
    }
}

/**
 * gen: minimal c file
 */
TestResult gen_minimal ()
{
    Lexer lexer {"examples/minimal/minimal.c"};
    Parser parser {lexer.get_tokens ()};
    Codegen cg {parser.parse ()};

    bool pass = cg.get_assembly () 
             == file_to_string ("examples/minimal/minimal.s");

    return TestResult {.name = "gen minimal",
                       .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("compiler",
    {
        gen_no_main,
        gen_minimal,
    }, {"lexer", "parser", "codegen", "file_utils"});

    tb.run_tests ();
    tb.print_results ();
}