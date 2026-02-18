/**
 * @file parser_tests.cpp
 * @brief Isolated tests for the Parser class
 */

#include <vector>
#include <iostream>
#include <testbench.hpp>
#include <parser.hpp>
#include <string>

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

/********** EXPRESSION TESTS **********/
/**
 * Parse a single integer literal
 */
bool parse_int_literal ()
{
    // int main () { return 42; }
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "main"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, "42"),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    return expr_is<IntLiteral> (*ret.value)
        && std::get<IntLiteral> (ret.value->node).value == 42;
}

/**
 * Binary arithmetic with correct precedence: 1 + 2 * 3 === 1 + (2 * 3)
 */
bool parse_precedence ()
{
    // int f () { return 1 + 2 * 3; }
    std::string s1 = "1", s2 = "2", s3 = "3";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::MULT_OP, "*"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    // Top should be ADD
    if (!expr_is<BinaryOp> (*ret.value))
        return false;

    auto& add = std::get<BinaryOp> (ret.value->node);
    if (!(add.op == BinaryOp::Op::ADD
        && expr_is<IntLiteral> (*add.left)
        && expr_is<BinaryOp> (*add.right)))
        return false;

    auto& mul = std::get<BinaryOp> (add.right->node);
    return mul.op == BinaryOp::Op::MUL;
}

/**
 * Unary negate: -5
 */
bool parse_unary_negate ()
{
    // int f () { return -5; }
    std::string s5 = "5";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::SUB_OP, "-"),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    if (!expr_is<UnaryOp> (*ret.value))
        return false;

    auto& un = std::get<UnaryOp> (ret.value->node);
    return un.op == UnaryOp::Op::NEGATE
        && expr_is<IntLiteral> (*un.operand)
        && std::get<IntLiteral> (un.operand->node).value == 5;
}

/**
 * Comparison operators: a < b
 */
bool parse_comparison ()
{
    // int f () { return a < b; }
    std::string sa = "a", sb = "b";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::LT_CMP, "<"),
        tok (TokenType::IDENTIFIER, sb),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    if (!expr_is<BinaryOp> (*ret.value))
        return false;

    auto& cmp = std::get<BinaryOp> (ret.value->node);
    return cmp.op == BinaryOp::Op::LT;
}

/**
 * Logical operators: a && b || c
 * Should parse as (a && b) || c
 */
bool parse_logical_ops ()
{
    // int f () { return a && b || c; }
    std::string sa = "a", sb = "b", sc = "c";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::AND_CMP, "&&"),
        tok (TokenType::IDENTIFIER, sb),
        tok (TokenType::OR_CMP, "||"),
        tok (TokenType::IDENTIFIER, sc),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    // Top should be OR, left should be AND
    if (!expr_is<BinaryOp> (*ret.value))
        return false;

    auto& orOp = std::get<BinaryOp> (ret.value->node);
    if (!(orOp.op == BinaryOp::Op::OR
        && expr_is<BinaryOp> (*orOp.left)))
        return false;

    auto& andOp = std::get<BinaryOp> (orOp.left->node);
    return andOp.op == BinaryOp::Op::AND;
}

/**
 * Parenthesized expression: (1 + 2) * 3
 */
bool parse_parens ()
{
    // int f () { return (1 + 2) * 3; }
    std::string s1 = "1", s2 = "2", s3 = "3";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::R_PAREN),
        tok (TokenType::MULT_OP, "*"),
        tok (TokenType::INT_LITERAL, s3),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    // Top should be MUL, left should be ADD
    if (!expr_is<BinaryOp> (*ret.value))
        return false;

    auto& mul = std::get<BinaryOp> (ret.value->node);
    if (!(mul.op == BinaryOp::Op::MUL
        && expr_is<BinaryOp> (*mul.left)))
        return false;

    auto& add = std::get<BinaryOp> (mul.left->node);
    return add.op == BinaryOp::Op::ADD;
}

/********* STATEMENT TESTS **********/
/**
 * Variable declaration without init: int x;
 */
bool parse_var_decl_no_init ()
{
    // int f () { int x; }
    std::string sx = "x";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& stmt = prog.functions[0].body.statements[0];
    if (!stmt_is<VarDecl> (stmt))
        return false;

    auto& decl = std::get<VarDecl> (stmt.node);
    return decl.name == "x" && !decl.init.has_value ();
}

/**
 * Variable declaration with init: int x = 10;
 */
bool parse_var_decl_with_init ()
{
    // int f () { int x = 10; }
    std::string sx = "x", s10 = "10";
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
        tok (TokenType::INT_LITERAL, s10),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& stmt = prog.functions[0].body.statements[0];
    if (!stmt_is<VarDecl> (stmt))
        return false;

    auto& decl = std::get<VarDecl> (stmt.node);
    return decl.name == "x"
        && decl.init.has_value ()
        && expr_is<IntLiteral> (*decl.init.value ())
        && std::get<IntLiteral> (decl.init.value ()->node).value == 10;
}

/**
 * Assignment: x = 5;
 */
bool parse_assignment ()
{
    // int f () { x = 5; }
    std::string sx = "x", s5 = "5";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s5),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& stmt = prog.functions[0].body.statements[0];
    if (!stmt_is<Assignment> (stmt))
        return false;

    auto& asgn = std::get<Assignment> (stmt.node);
    return asgn.name == "x"
        && expr_is<IntLiteral> (*asgn.value)
        && std::get<IntLiteral> (asgn.value->node).value == 5;
}

/**
 * If statement: if (x) { return 1; }
 */
bool parse_if_stmt ()
{
    std::string sx = "x", s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
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

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& stmt = prog.functions[0].body.statements[0];
    if (!stmt_is<IfStmt> (stmt))
        return false;

    auto& ifs = std::get<IfStmt> (stmt.node);
    return expr_is<Identifier> (*ifs.condition)
        && ifs.then_block->statements.size () == 1
        && stmt_is<ReturnStmt> (ifs.then_block->statements[0]);
}

/**
 * While statement: while (x) { x = 0; }
 */
bool parse_while_stmt ()
{
    std::string sx = "x", s0 = "0";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::WHILE, "while"),
        tok (TokenType::L_PAREN),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s0),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& stmt = prog.functions[0].body.statements[0];
    if (!stmt_is<WhileStmt> (stmt))
        return false;

    auto& ws = std::get<WhileStmt> (stmt.node);
    return expr_is<Identifier> (*ws.condition)
        && ws.body->statements.size () == 1
        && stmt_is<Assignment> (ws.body->statements[0]);
}

/********** ERROR TESTS **********/
/**
 * Missing semicolon after return
 */
bool parse_error_missing_semicolon ()
{
    std::string s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::R_BRACE),  // missing semicolon
        tok (TokenType::END_OF_FILE),
    };

    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        return msg.find ("';'") != std::string::npos;
    }

    return false;
}

/**
 * Invalid expression (bare operator)
 */
bool parse_error_invalid_expr ()
{
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::MULT_OP, "*"),  // invalid start of expression
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        return msg.find ("expression") != std::string::npos;
    }

    return false;
}

/**
 * Unclosed parenthesis
 */
bool parse_error_unclosed_paren ()
{
    std::string s1 = "1";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),  // missing ')'
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        return msg.find ("')'") != std::string::npos;
    }

    return false;
}

/********** FUNCTION PARAMETER & CALL TESTS **********/
bool parse_func_one_param ()
{
    // int f (int a) { return a; }
    std::string sa = "a";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    return prog.functions[0].params.size () == 1
        && prog.functions[0].params[0].name == "a";
}

bool parse_func_multi_params ()
{
    // int f (int a, int b, int c) { return a; }
    std::string sa = "a", sb = "b", sc = "c";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::COMMA),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sb),
        tok (TokenType::COMMA),
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sc),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sa),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    return prog.functions[0].params.size () == 3
        && prog.functions[0].params[0].name == "a"
        && prog.functions[0].params[1].name == "b"
        && prog.functions[0].params[2].name == "c";
}

bool parse_func_call ()
{
    // int main () { return f (1, 2); }
    std::string s1 = "1", s2 = "2";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, "main"),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, "f"),
        tok (TokenType::L_PAREN),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::COMMA),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::R_PAREN),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    auto& ret = std::get<ReturnStmt> (
        prog.functions[0].body.statements[0].node);

    if (!expr_is<FuncCall> (*ret.value))
        return false;

    auto& call = std::get<FuncCall> (ret.value->node);
    return call.name == "f"
        && call.args.size () == 2
        && expr_is<IntLiteral> (*call.args[0])
        && expr_is<IntLiteral> (*call.args[1]);
}

/********** INTEGRATION TESTS **********/
/**
 * Full program with multiple statements
 */
bool parse_full_program ()
{
    // int main () { int x = 1; int y = 2; return x + y; }
    std::string sm = "main", sx = "x", sy = "y";
    std::string s1 = "1", s2 = "2";
    std::vector<Token> tokens =
    {
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sm),
        tok (TokenType::L_PAREN),
        tok (TokenType::R_PAREN),
        tok (TokenType::L_BRACE),
        // int x = 1;
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s1),
        tok (TokenType::SEMICOLON),
        // int y = 2;
        tok (TokenType::INT_TYPE, "int"),
        tok (TokenType::IDENTIFIER, sy),
        tok (TokenType::EQ_OP, "="),
        tok (TokenType::INT_LITERAL, s2),
        tok (TokenType::SEMICOLON),
        // return x + y;
        tok (TokenType::RETURN, "return"),
        tok (TokenType::IDENTIFIER, sx),
        tok (TokenType::ADD_OP, "+"),
        tok (TokenType::IDENTIFIER, sy),
        tok (TokenType::SEMICOLON),
        tok (TokenType::R_BRACE),
        tok (TokenType::END_OF_FILE),
    };

    Parser parser {tokens};
    Program prog = parser.parse ();

    return prog.functions.size () == 1
        && prog.functions[0].name == "main"
        && prog.functions[0].body.statements.size () == 3
        && stmt_is<VarDecl> (prog.functions[0].body.statements[0])
        && stmt_is<VarDecl> (prog.functions[0].body.statements[1])
        && stmt_is<ReturnStmt> (prog.functions[0].body.statements[2]);
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("Expressions",
    {
        {parse_int_literal,             "parse int literal"},
        {parse_precedence,              "parse precedence (* before +)"},
        {parse_unary_negate,            "parse unary negate"},
        {parse_comparison,              "parse comparison (<)"},
        {parse_logical_ops,             "parse logical ops (&& before ||)"},
        {parse_parens,                  "parse parenthesized expr"},
    });

    tb.add_family ("Statements",
    {
        {parse_var_decl_no_init,        "parse var decl (no init)"},
        {parse_var_decl_with_init,      "parse var decl (with init)"},
        {parse_assignment,              "parse assignment"},
        {parse_if_stmt,                 "parse if statement"},
        {parse_while_stmt,              "parse while statement"},
    }, {"Expressions"});

    tb.add_family ("Errors",
    {
        {parse_error_missing_semicolon, "error: missing semicolon"},
        {parse_error_invalid_expr,      "error: invalid expression"},
        {parse_error_unclosed_paren,    "error: unclosed paren"},
    });

    tb.add_family ("Functions",
    {
        {parse_func_one_param,          "parse func one param"},
        {parse_func_multi_params,       "parse func multi params"},
        {parse_func_call,               "parse func call"},
    }, {"Expressions"});

    tb.add_family ("Integration",
    {
        {parse_full_program,            "parse full program"},
    }, {"Expressions", "Statements"});

    tb.run_tests ();
    tb.print_results ();
}
