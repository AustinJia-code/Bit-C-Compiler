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
bool pip_no_main ()
{
    Lexer lexer {"int bad_func () { return 1; }", false};
    Parser parser {lexer.get_tokens ()};

    try
    {
        Codegen cg {parser.parse ()};
        return false;
    }
    catch (const GenError&)
    {
        return true;
    }
}

/********** Return tests **********/
bool com_return_literal ()
{
    return run_source ("int main () { return 42; }") == 42;
}

bool com_return_zero ()
{
    return run_source ("int main () { return 0; }") == 0;
}

/********** Arithmetic tests **********/
bool com_add ()
{
    return run_source ("int main () { return 10 + 20; }") == 30;
}

bool com_sub ()
{
    return run_source ("int main () { return 50 - 8; }") == 42;
}

bool com_mul ()
{
    return run_source ("int main () { return 6 * 7; }") == 42;
}

bool com_div ()
{
    return run_source ("int main () { return 84 / 2; }") == 42;
}

bool com_precedence ()
{
    return run_source ("int main () { return 2 + 3 * 4; }") == 14;
}

bool com_nested_arith ()
{
    return run_source ("int main () { return (2 + 3) * (10 - 4); }") == 30;
}

/********** Comparison tests **********/
bool com_lt_true ()
{
    return run_source ("int main () { return 1 < 5; }") == 1;
}

bool com_lt_false ()
{
    return run_source ("int main () { return 5 < 1; }") == 0;
}

bool com_gt_true ()
{
    return run_source ("int main () { return 5 > 1; }") == 1;
}

bool com_eq_true ()
{
    return run_source ("int main () { return 42 == 42; }") == 1;
}

bool com_eq_false ()
{
    return run_source ("int main () { return 42 == 13; }") == 0;
}

/********** If tests **********/
bool com_if_true ()
{
    return run_source
    (
        "int main () {"
        "    if (1 < 5) { return 42; }"
        "    return 13;"
        "}"
    ) == 42;
}

bool com_if_false ()
{
    return run_source
    (
        "int main () {"
        "    if (5 < 1) { return 42; }"
        "    return 13;"
        "}"
    ) == 13;
}

/********** Variable tests **********/
bool com_var_decl ()
{
    return run_source
    (
        "int main () {"
        "    int x = 42;"
        "    return x;"
        "}"
    ) == 42;
}

bool com_var_assign ()
{
    return run_source
    (
        "int main () {"
        "    int x = 1;"
        "    x = 42;"
        "    return x;"
        "}"
    ) == 42;
}

bool com_var_arith ()
{
    return run_source
    (
        "int main () {"
        "    int a = 10;"
        "    int b = 32;"
        "    return a + b;"
        "}"
    ) == 42;
}

/********** While tests **********/
bool com_while ()
{
    return run_source
    (
        "int main () {"
        "    int x = 0;"
        "    while (x < 10) { x = x + 1; }"
        "    return x;"
        "}"
    ) == 10;
}

bool com_while_sum ()
{
    return run_source
    (
        "int main () {"
        "    int i = 0;"
        "    int sum = 0;"
        "    while (i < 5) {"
        "        sum = sum + i;"
        "        i = i + 1;"
        "    }"
        "    return sum;"
        "}"
    ) == 10;
}

/********** Function call tests **********/
bool com_call_one_param ()
{
    return run_source
    (
        "int id (int x) { return x; }"
        "int main () { return id (42); }"
    ) == 42;
}

bool com_call_two_params ()
{
    return run_source
    (
        "int add (int a, int b) { return a + b; }"
        "int main () { return add (10, 32); }"
    ) == 42;
}

bool com_call_expr_args ()
{
    return run_source
    (
        "int add (int a, int b) { return a + b; }"
        "int main () { return add (2 + 3, 7 * 5); }"
    ) == 40;
}

bool com_nested_calls ()
{
    return run_source
    (
        "int double_it (int x) { return x + x; }"
        "int inc (int x) { return x + 1; }"
        "int main () { return double_it (inc (20)); }"
    ) == 42;
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("pipeline",
    {
        {pip_no_main,           "no main"},
    }, {"file_utils", "lexer", "parser", "compiler"});

    tb.add_family ("return",
    {
        {com_return_literal,    "return literal"},
        {com_return_zero,       "return zero"},
    }, {"pipeline"});

    tb.add_family ("arithmetic",
    {
        {com_add,               "add"},
        {com_sub,               "sub"},
        {com_mul,               "mul"},
        {com_div,               "div"},
        {com_precedence,        "precedence"},
        {com_nested_arith,      "nested arith"},
    }, {"return"});

    tb.add_family ("comparison",
    {
        {com_lt_true,           "lt true"},
        {com_lt_false,          "lt false"},
        {com_gt_true,           "gt true"},
        {com_eq_true,           "eq true"},
        {com_eq_false,          "eq false"},
    }, {"return"});

    tb.add_family ("conditional",
    {
        {com_if_true,           "if true"},
        {com_if_false,          "if false"},
    }, {"comparison"});

    tb.add_family ("variables",
    {
        {com_var_decl,          "var decl"},
        {com_var_assign,        "var assign"},
        {com_var_arith,         "var arith"},
    }, {"return"});

    tb.add_family ("loops",
    {
        {com_while,             "while loop"},
        {com_while_sum,         "while sum"},
    }, {"variables"});

    tb.add_family ("functions",
    {
        {com_call_one_param,    "call one param"},
        {com_call_two_params,   "call two params"},
        {com_call_expr_args,    "call expr args"},
        {com_nested_calls,      "nested calls"},
    }, {"return"});

    tb.run_tests ();
    tb.print_results ();
}
