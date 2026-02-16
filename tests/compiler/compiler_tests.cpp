/**
 * @file compiler_tests.cpp
 * @brief Full compiler pipeline (source -> asm -> run) tests
 */

#include "testbench.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "file_utils.hpp"
#include <sys/wait.h>

/**
 * Compile source string through pipeline, assemble, run, return exit code.
 * Returns -1 on failure.
 */
int run_source (const std::string& source)
{
    Lexer lexer {source, false};
    Parser parser {lexer.get_tokens ()};
    Codegen cg {parser.parse ()};

    std::string asm_path = get_full_path ("out/test.s");
    std::string bin_path = get_full_path ("out/test");

    string_to_file (cg.get_assembly (), asm_path);

    std::string assemble_cmd = "g++ " + asm_path + " -o " + bin_path + " 2>/dev/null";
    if (system (assemble_cmd.data ()) != 0)
        return -1;

    int status = system (bin_path.data ());
    return WEXITSTATUS (status);
}

/********** Pipeline error tests **********/
TestResult pip_no_main ()
{
    Lexer lexer {"int bad_func () { return 1; }", false};
    Parser parser {lexer.get_tokens ()};

    try
    {
        Codegen cg {parser.parse ()};
        return TestResult {.name = "no main", .pass = false};
    }
    catch (const GenError&)
    {
        return TestResult {.name = "no main", .pass = true};
    }
}

/********** Return tests **********/
TestResult com_return_literal ()
{
    int code = run_source ("int main () { return 42; }");
    return TestResult {.name = "return literal", .pass = code == 42};
}

TestResult com_return_zero ()
{
    int code = run_source ("int main () { return 0; }");
    return TestResult {.name = "return zero", .pass = code == 0};
}

/********** Arithmetic tests **********/
TestResult com_add ()
{
    int code = run_source ("int main () { return 10 + 20; }");
    return TestResult {.name = "add", .pass = code == 30};
}

TestResult com_sub ()
{
    int code = run_source ("int main () { return 50 - 8; }");
    return TestResult {.name = "sub", .pass = code == 42};
}

TestResult com_mul ()
{
    int code = run_source ("int main () { return 6 * 7; }");
    return TestResult {.name = "mul", .pass = code == 42};
}

TestResult com_div ()
{
    int code = run_source ("int main () { return 84 / 2; }");
    return TestResult {.name = "div", .pass = code == 42};
}

TestResult com_precedence ()
{
    int code = run_source ("int main () { return 2 + 3 * 4; }");
    return TestResult {.name = "precedence", .pass = code == 14};
}

TestResult com_nested_arith ()
{
    int code = run_source ("int main () { return (2 + 3) * (10 - 4); }");
    return TestResult {.name = "nested arith", .pass = code == 30};
}

/********** Comparison tests **********/
TestResult com_lt_true ()
{
    int code = run_source ("int main () { return 1 < 5; }");
    return TestResult {.name = "lt true", .pass = code == 1};
}

TestResult com_lt_false ()
{
    int code = run_source ("int main () { return 5 < 1; }");
    return TestResult {.name = "lt false", .pass = code == 0};
}

TestResult com_gt_true ()
{
    int code = run_source ("int main () { return 5 > 1; }");
    return TestResult {.name = "gt true", .pass = code == 1};
}

TestResult com_eq_true ()
{
    int code = run_source ("int main () { return 42 == 42; }");
    return TestResult {.name = "eq true", .pass = code == 1};
}

TestResult com_eq_false ()
{
    int code = run_source ("int main () { return 42 == 13; }");
    return TestResult {.name = "eq false", .pass = code == 0};
}

/********** If tests **********/
TestResult com_if_true ()
{
    int code = run_source (
        "int main () {"
        "    if (1 < 5) { return 42; }"
        "    return 13;"
        "}"
    );
    return TestResult {.name = "if true", .pass = code == 42};
}

TestResult com_if_false ()
{
    int code = run_source (
        "int main () {"
        "    if (5 < 1) { return 42; }"
        "    return 13;"
        "}"
    );
    return TestResult {.name = "if false", .pass = code == 13};
}

/********** Variable tests **********/
TestResult com_var_decl ()
{
    int code = run_source (
        "int main () {"
        "    int x = 42;"
        "    return x;"
        "}"
    );
    return TestResult {.name = "var decl", .pass = code == 42};
}

TestResult com_var_assign ()
{
    int code = run_source (
        "int main () {"
        "    int x = 1;"
        "    x = 42;"
        "    return x;"
        "}"
    );
    return TestResult {.name = "var assign", .pass = code == 42};
}

TestResult com_var_arith ()
{
    int code = run_source (
        "int main () {"
        "    int a = 10;"
        "    int b = 32;"
        "    return a + b;"
        "}"
    );
    return TestResult {.name = "var arith", .pass = code == 42};
}

/********** While tests **********/
TestResult com_while ()
{
    int code = run_source (
        "int main () {"
        "    int x = 0;"
        "    while (x < 10) { x = x + 1; }"
        "    return x;"
        "}"
    );
    return TestResult {.name = "while loop", .pass = code == 10};
}

TestResult com_while_sum ()
{
    int code = run_source (
        "int main () {"
        "    int i = 0;"
        "    int sum = 0;"
        "    while (i < 5) {"
        "        sum = sum + i;"
        "        i = i + 1;"
        "    }"
        "    return sum;"
        "}"
    );
    return TestResult {.name = "while sum", .pass = code == 10};
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
    }, {"file_utils", "lexer", "parser", "compiler"});

    tb.add_family ("return",
    {
        com_return_literal,
        com_return_zero,
    }, {"pipeline"});

    tb.add_family ("arithmetic",
    {
        com_add,
        com_sub,
        com_mul,
        com_div,
        com_precedence,
        com_nested_arith,
    }, {"return"});

    tb.add_family ("comparison",
    {
        com_lt_true,
        com_lt_false,
        com_gt_true,
        com_eq_true,
        com_eq_false,
    }, {"return"});

    tb.add_family ("conditional",
    {
        com_if_true,
        com_if_false,
    }, {"comparison"});

    tb.add_family ("variables",
    {
        com_var_decl,
        com_var_assign,
        com_var_arith,
    }, {"return"});

    tb.add_family ("loops",
    {
        com_while,
        com_while_sum,
    }, {"variables"});

    tb.run_tests ();
    tb.print_results ();
}