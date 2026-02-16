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
TestResult parse_int_literal ()
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
    bool pass = expr_is<IntLiteral> (*ret.value)
             && std::get<IntLiteral> (ret.value->node).value == 42;

    return TestResult {.name = "parse int literal", .pass = pass};
}

/**
 * Binary arithmetic with correct precedence: 1 + 2 * 3 === 1 + (2 * 3)
 */
TestResult parse_precedence ()
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
    bool pass = expr_is<BinaryOp> (*ret.value);
    if (pass)
    {
        auto& add = std::get<BinaryOp> (ret.value->node);
        pass = add.op == BinaryOp::Op::ADD
            && expr_is<IntLiteral> (*add.left)
            && expr_is<BinaryOp> (*add.right);
        if (pass)
        {
            auto& mul = std::get<BinaryOp> (add.right->node);
            pass = mul.op == BinaryOp::Op::MUL;
        }
    }

    return TestResult {.name = "parse precedence (* before +)", .pass = pass};
}

/**
 * Unary negate: -5
 */
TestResult parse_unary_negate ()
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

    bool pass = expr_is<UnaryOp> (*ret.value);
    if (pass)
    {
        auto& un = std::get<UnaryOp> (ret.value->node);
        pass = un.op == UnaryOp::Op::NEGATE
            && expr_is<IntLiteral> (*un.operand)
            && std::get<IntLiteral> (un.operand->node).value == 5;
    }

    return TestResult {.name = "parse unary negate", .pass = pass};
}

/**
 * Comparison operators: a < b
 */
TestResult parse_comparison ()
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

    bool pass = expr_is<BinaryOp> (*ret.value);
    if (pass)
    {
        auto& cmp = std::get<BinaryOp> (ret.value->node);
        pass = cmp.op == BinaryOp::Op::LT;
    }

    return TestResult {.name = "parse comparison (<)", .pass = pass};
}

/**
 * Logical operators: a && b || c
 * Should parse as (a && b) || c
 */
TestResult parse_logical_ops ()
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
    bool pass = expr_is<BinaryOp> (*ret.value);
    if (pass)
    {
        auto& orOp = std::get<BinaryOp> (ret.value->node);
        pass = orOp.op == BinaryOp::Op::OR
            && expr_is<BinaryOp> (*orOp.left);
        if (pass)
        {
            auto& andOp = std::get<BinaryOp> (orOp.left->node);
            pass = andOp.op == BinaryOp::Op::AND;
        }
    }

    return TestResult {.name = "parse logical ops (&& before ||)",
                       .pass = pass};
}

/**
 * Parenthesized expression: (1 + 2) * 3
 */
TestResult parse_parens ()
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
    bool pass = expr_is<BinaryOp> (*ret.value);
    if (pass)
    {
        auto& mul = std::get<BinaryOp> (ret.value->node);
        pass = mul.op == BinaryOp::Op::MUL
            && expr_is<BinaryOp> (*mul.left);
        if (pass)
        {
            auto& add = std::get<BinaryOp> (mul.left->node);
            pass = add.op == BinaryOp::Op::ADD;
        }
    }

    return TestResult {.name = "parse parenthesized expr", .pass = pass};
}

/********* STATEMENT TESTS **********/
/**
 * Variable declaration without init: int x;
 */
TestResult parse_var_decl_no_init ()
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
    bool pass = stmt_is<VarDecl> (stmt);
    if (pass)
    {
        auto& decl = std::get<VarDecl> (stmt.node);
        pass = decl.name == "x" && !decl.init.has_value ();
    }

    return TestResult {.name = "parse var decl (no init)", .pass = pass};
}

/**
 * Variable declaration with init: int x = 10;
 */
TestResult parse_var_decl_with_init ()
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
    bool pass = stmt_is<VarDecl> (stmt);
    if (pass)
    {
        auto& decl = std::get<VarDecl> (stmt.node);
        pass = decl.name == "x"
            && decl.init.has_value ()
            && expr_is<IntLiteral> (*decl.init.value ())
            && std::get<IntLiteral> (decl.init.value ()->node).value == 10;
    }

    return TestResult {.name = "parse var decl (with init)", .pass = pass};
}

/**
 * Assignment: x = 5;
 */
TestResult parse_assignment ()
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
    bool pass = stmt_is<Assignment> (stmt);
    if (pass)
    {
        auto& asgn = std::get<Assignment> (stmt.node);
        pass = asgn.name == "x"
            && expr_is<IntLiteral> (*asgn.value)
            && std::get<IntLiteral> (asgn.value->node).value == 5;
    }

    return TestResult {.name = "parse assignment", .pass = pass};
}

/**
 * If statement: if (x) { return 1; }
 */
TestResult parse_if_stmt ()
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
    bool pass = stmt_is<IfStmt> (stmt);
    if (pass)
    {
        auto& ifs = std::get<IfStmt> (stmt.node);
        pass = expr_is<Identifier> (*ifs.condition)
            && ifs.then_block->statements.size () == 1
            && stmt_is<ReturnStmt> (ifs.then_block->statements[0]);
    }

    return TestResult {.name = "parse if statement", .pass = pass};
}

/**
 * While statement: while (x) { x = 0; }
 */
TestResult parse_while_stmt ()
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
    bool pass = stmt_is<WhileStmt> (stmt);
    if (pass)
    {
        auto& ws = std::get<WhileStmt> (stmt.node);
        pass = expr_is<Identifier> (*ws.condition)
            && ws.body->statements.size () == 1
            && stmt_is<Assignment> (ws.body->statements[0]);
    }

    return TestResult {.name = "parse while statement", .pass = pass};
}

/********** ERROR TESTS **********/
/**
 * Missing semicolon after return
 */
TestResult parse_error_missing_semicolon ()
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

    bool pass = false;
    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        pass = msg.find ("';'") != std::string::npos;
    }

    return TestResult {.name = "error: missing semicolon", .pass = pass};
}

/**
 * Invalid expression (bare operator)
 */
TestResult parse_error_invalid_expr ()
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

    bool pass = false;
    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        pass = msg.find ("expression") != std::string::npos;
    }

    return TestResult {.name = "error: invalid expression", .pass = pass};
}

/**
 * Unclosed parenthesis
 */
TestResult parse_error_unclosed_paren ()
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

    bool pass = false;
    try
    {
        Parser parser {tokens};
        parser.parse ();
    }
    catch (const ParseError& e)
    {
        std::string msg = e.what ();
        pass = msg.find ("')'") != std::string::npos;
    }

    return TestResult {.name = "error: unclosed paren", .pass = pass};
}

/********** INTEGRATION TESTS **********/
/**
 * Full program with multiple statements
 */
TestResult parse_full_program ()
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

    bool pass = prog.functions.size () == 1
             && prog.functions[0].name == "main"
             && prog.functions[0].body.statements.size () == 3
             && stmt_is<VarDecl> (prog.functions[0].body.statements[0])
             && stmt_is<VarDecl> (prog.functions[0].body.statements[1])
             && stmt_is<ReturnStmt> (prog.functions[0].body.statements[2]);

    return TestResult {.name = "parse full program", .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("Expressions",
    {
        parse_int_literal,
        parse_precedence,
        parse_unary_negate,
        parse_comparison,
        parse_logical_ops,
        parse_parens,
    });

    tb.add_family ("Statements",
    {
        parse_var_decl_no_init,
        parse_var_decl_with_init,
        parse_assignment,
        parse_if_stmt,
        parse_while_stmt,
    }, {"Expressions"});

    tb.add_family ("Errors",
    {
        parse_error_missing_semicolon,
        parse_error_invalid_expr,
        parse_error_unclosed_paren,
    });

    tb.add_family ("Integration",
    {
        parse_full_program,
    }, {"Expressions", "Statements"});

    tb.run_tests ();
    tb.print_results ();
}
