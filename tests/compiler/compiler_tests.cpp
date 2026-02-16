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
bool check_compiler (const std::string& source, const std::string& asm_dest,
                     const std::string& bin_dest, const std::string& sol_bin)
{
    std::string compile_cmd {"./compiler " + source + " -o " + asm_dest};
    std::cout << compile_cmd << std::endl;
    system (compile_cmd.data ());

    std::string assemble_cmd {"g++ " + asm_dest + " -o " + bin_dest};
    system (assemble_cmd.data ());

    int run_status = system (bin_dest.data ());
    int exit_code = WEXITSTATUS (run_status);

    run_status = system (sol_bin.data ());
    int sol_code = WEXITSTATUS (run_status);

    // Check both assembly and output
    return file_to_string (asm_dest) == file_to_string ("examples/return/return.s")      
        && exit_code == sol_code;
}

/**
 * pip: no main
 */
TestResult pip_no_main ()
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
 * com: return only c file
 */
TestResult com_return ()
{
    std::string source = get_full_path ("examples/return/return.c");
    std::string asm_dest = get_full_path ("out/return.s");
    std::string bin_dest = get_full_path ("out/return");
    std::string sol_bin = get_full_path ("examples/return/return");

    return TestResult
    {
        .name = "gen return",
        .pass = check_compiler (source, asm_dest, bin_dest, sol_bin)
    };
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("pipeline",
    {
        pip_no_main,
    }, {"lexer", "parser", "codegen", "file_utils"});

    tb.add_family ("compiler",
    {
        com_return,
    }, {"lexer", "parser", "codegen", "file_utils"});


    tb.run_tests ();
    tb.print_results ();
}