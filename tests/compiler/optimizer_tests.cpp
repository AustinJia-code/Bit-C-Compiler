/**
 * @file optimizer_tests.cpp
 * @brief Isolated tests for the Optimizer class
 */

#include <testbench.hpp>
#include <parser.hpp>
#include <optimizer.hpp>
#include <string>
#include <vector>

/**
 * Helper: create a minimal token
 */
Token tok (TokenType type, std::string_view lexeme = {},
           size_t line = 1, size_t col = 1)
{
    return Token {type, {line, col}, lexeme};
}

/**
 * Helper: get Expr variant node type
 */
template <typename T>
bool expr_is (const Expr& e)
{
    return std::holds_alternative<T> (e.node);
}

/**
 * Helper: get Stmt variant node type
 */
template <typename T>
bool stmt_is (const Stmt& s)
{
    return std::holds_alternative<T> (s.node);
}

/**
 * Helper: parse tokens then run optimizer, returns resulting Program
 */
Program parse_and_optimize (std::vector<Token>& tokens)
{
    Parser parser {tokens};
    Program prog = parser.parse ();
    Optimizer opt;
    opt.optimize (prog);
    return prog;
}

/********** CONSTANT FOLDING - ARITHMETIC **********/

/**
 * 2 + 3 = 5
 */
bool fold_add ()
{
    std::string s2 = "2", s3 = "3";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 5;
}

/**
 * 10 - 4 = 6
 */
bool fold_sub ()
{
    std::string s10 = "10", s4 = "4";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s10),
        tok (TokenType::SUB_OP, "-"),
        tok (TokenType::INT_LITERAL, s4),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 6;
}

/**
 * 3 * 7 = 21
 */
bool fold_mul ()
{
    std::string s3 = "3", s7 = "7";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::MULT_OP, "*"),
        tok (TokenType::INT_LITERAL, s7),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 21;
}

/**
 * 12 / 4 = 3
 */
bool fold_div ()
{
    std::string s12 = "12", s4 = "4";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s12),
        tok (TokenType::DIV_OP, "/"),
        tok (TokenType::INT_LITERAL, s4),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 3;
}

/**
 * 5 / 0 special case, avoid folding divide by zero.
 * Kind of hacky... may need to fix if doing linker and assembler
 */
bool fold_div_by_zero_skipped ()
{
    std::string s5 = "5", s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::DIV_OP, "/"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<BinaryOp> (*ret.value);
}

/**
 * 2 + 3 * 4 = 14  (precedence test)
 */
bool fold_nested ()
{
    std::string s2 = "2", s3 = "3", s4 = "4";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::MULT_OP, "*"),
        tok (TokenType::INT_LITERAL, s4),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 14;
}

/********** CONSTANT FOLDING - UNARY **********/
/**
 * -8 = -8
 */
bool fold_negate ()
{
    std::string s8 = "8";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::SUB_OP, "-"),
        tok (TokenType::INT_LITERAL, s8),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == -8;
}

/**
 * !0 = 1
 */
bool fold_not_zero ()
{
    std::string s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::NOT_OP, "!"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 1;
}

/**
 * !5 = 0
 */
bool fold_not_nonzero ()
{
    std::string s5 = "5";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::NOT_OP, "!"),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 0;
}

/********** CONSTANT FOLDING - COMPARISONS & LOGICAL **********/
/**
 * 1 < 5 = 1
 */
bool fold_lt_true ()
{
    std::string s1 = "1", s5 = "5";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::LT_CMP, "<"),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 1;
}

/**
 * 3 == 3   =   1
 */
bool fold_eq_true ()
{
    std::string sa = "3", sb = "3";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, sa),
        tok (TokenType::EQ_CMP, "=="),
        tok (TokenType::INT_LITERAL, sb),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 1;
}

/**
 * 1 && 0 = 0
 */
bool fold_and_false ()
{
    std::string s1 = "1", s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::AND_CMP, "&&"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 0;
}

/**
 * 0 || 1 = 1
 */
bool fold_or_true ()
{
    std::string s0 = "0", s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::OR_CMP, "||"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 1;
}

/********** CONSTANT FOLDING - NO-FOLD CASES **********/
/**
 * x + 1
 */
bool fold_no_fold_identifier ()
{
    std::string sx = "x", s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& ret = std::get<ReturnStmt> (prog.functions[0].body.statements[0].node);
    return expr_is<BinaryOp> (*ret.value);
}

/********** CONSTANT FOLDING - STATEMENTS **********/
/**
 * int x = 2 * 5;   =   x = 10
 */
bool fold_var_decl_init ()
{
    std::string sx = "x", s2 = "2", s5 = "5";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::MULT_OP, "*"),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& decl = std::get<VarDecl> (prog.functions[0].body.statements[0].node);
    return decl.init.has_value ()
        && expr_is<IntLiteral> (*decl.init.value ())
        && std::get<IntLiteral> (decl.init.value ()->node).value == 10;
}

/**
 * x = 10 - 3;   =   x = 7
 */
bool fold_assignment_value ()
{
    std::string sx = "x", s10 = "10", s3 = "3";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s10),
        tok (TokenType::SUB_OP, "-"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& asgn = std::get<Assignment> (prog.functions[0].body.statements[0].node);
    return expr_is<IntLiteral> (*asgn.value)
        && std::get<IntLiteral> (asgn.value->node).value == 7;
}

/********** DEAD BRANCH REMOVAL **********/
/**
 * if (1) { return 42; } is Block containing ReturnStmt
 */
bool if_always_true_inlined ()
{
    std::string s1 = "1", s42 = "42";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IF, "if"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s42),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& stmts = prog.functions[0].body.statements;
    return stmts.size () == 1
        && stmt_is<Block> (stmts[0]);
}

/**
 * if (0) { return 99; } return 7; is return 7
 */
bool if_always_false_removed ()
{
    std::string scond = "0", sbody = "99", sret = "7";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IF, "if"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, scond),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, sbody),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, sret),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& stmts = prog.functions[0].body.statements;
    return stmts.size () == 1
        && stmt_is<ReturnStmt> (stmts[0]);
}

/**
 * if (2 + 2) { return 1; } return 0; is return 1; return 0;
 */
bool if_const_expr_true ()
{
    std::string s2a = "2", s2b = "2", s1 = "1", s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IF, "if"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s2a),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s2b),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& stmts = prog.functions[0].body.statements;
    // if replaced with Block, return 0 stays → 2 stmts total
    return stmts.size () == 2
        && stmt_is<Block> (stmts[0])
        && stmt_is<ReturnStmt> (stmts[1]);
}

/**
 * if (1 - 1) { return 1; } return 0; is return 0
 */
bool if_const_expr_false ()
{
    std::string s1a = "1", s1b = "1", s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IF, "if"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s1a),
        tok (TokenType::SUB_OP, "-"),
        tok (TokenType::INT_LITERAL, s1b),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& stmts = prog.functions[0].body.statements;
    // Branch removed, only return 0 remains
    return stmts.size () == 1
        && stmt_is<ReturnStmt> (stmts[0]);
}

/**
 * if (x) { return 1; } remains same
 */
bool if_non_const_preserved ()
{
    std::string sx = "x", s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IF, "if"),
        tok (TokenType::L_PAREN),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    auto prog = parse_and_optimize (tokens);
    auto& stmts = prog.functions[0].body.statements;
    return stmts.size () == 1
        && stmt_is<IfStmt> (stmts[0]);
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("Constant Folding",
    {
        {fold_add,                  "fold: 2 + 3 → 5"},
        {fold_sub,                  "fold: 10 - 4 → 6"},
        {fold_mul,                  "fold: 3 * 7 → 21"},
        {fold_div,                  "fold: 12 / 4 → 3"},
        {fold_div_by_zero_skipped,  "fold: 5 / 0 not folded (div-by-zero)"},
        {fold_nested,               "fold: 2 + 3 * 4 → 14 (precedence preserved)"},
        {fold_negate,               "fold: -8 → -8"},
        {fold_not_zero,             "fold: !0 → 1"},
        {fold_not_nonzero,          "fold: !5 → 0"},
        {fold_lt_true,              "fold: 1 < 5 → 1"},
        {fold_eq_true,              "fold: 3 == 3 → 1"},
        {fold_and_false,            "fold: 1 && 0 → 0"},
        {fold_or_true,              "fold: 0 || 1 → 1"},
        {fold_no_fold_identifier,   "fold: x + 1 stays BinaryOp"},
        {fold_var_decl_init,        "fold: var init 2 * 5 → 10"},
        {fold_assignment_value,     "fold: assignment 10 - 3 → 7"},
    });

    tb.add_family ("Dead Branch Removal",
    {
        {if_always_true_inlined,        "if (1): stmt replaced with Block"},
        {if_always_false_removed,       "if (0): stmt removed entirely"},
        {if_const_expr_true,            "if (2+2): folds to 4, branch inlined"},
        {if_const_expr_false,           "if (1-1): folds to 0, branch removed"},
        {if_non_const_preserved,        "if (x): IfStmt preserved"},
    }, {"Constant Folding"});

    tb.run_tests ();
    tb.print_results ();
}
