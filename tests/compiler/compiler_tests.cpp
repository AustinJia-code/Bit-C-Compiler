/**
 * @file compiler_tests.cpp
 * @brief Full compiler pipeline (file -> assembly) tests
 */

#include "testbench.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "file_utils.hpp"
#include <sys/wait.h>

/**
 * Compiles and runs asm in output folder, returns exit code
 */
int compile_and_run_out (const std::string& asm_file)
{
    // Compile
    std::string in_path = get_full_path ("out/" + asm_file + ".s");
    std::string out_path = get_full_path ("out/" + asm_file);
    std::string cmd = "g++ " + in_path + " -o " + out_path;

    std::cout << cmd << std::endl;
    int compile_status = system (cmd.data ());
    if (WEXITSTATUS (compile_status) != 0)
        return -1;

    // Run
    int run_status = system (out_path.data ());
    return WEXITSTATUS (run_status);
}

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

    std::string assembly_str = cg.get_assembly ();
    string_to_file (assembly_str, "out/minimal.s");

    // Check both assembly and output
    bool pass = assembly_str == file_to_string ("examples/minimal/minimal.s")
             && compile_and_run_out ("minimal") == 42;

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