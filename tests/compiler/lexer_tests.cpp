/**
 * @file lexer_tests.cpp
 * @brief Isolated tests for the Lexer class
 */

#include <vector>
#include <iostream>
#include <testbench.hpp>
#include <lexer.hpp>
#include <string>

/**
 * get_tokens: empty input produces only EOF
 */
TestResult gt_empty ()
{
    Lexer lexer {"", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 1
             && tokens[0].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt empty input", .pass = pass};
}

/**
 * get_tokens: whitespace-only input produces only EOF
 */
TestResult gt_whitespace ()
{
    Lexer lexer {"   \n\t  \n  ", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 1
             && tokens[0].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt whitespace only", .pass = pass};
}

/**
 * get_tokens: integer literal
 */
TestResult gt_int_literal ()
{
    Lexer lexer {"42", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 2
             && tokens[0].type == TokenType::INT_LITERAL
             && tokens[0].lexeme == "42"
             && tokens[0].start.line == 1
             && tokens[0].start.col == 1
             && tokens[1].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt int literal", .pass = pass};
}

/**
 * get_tokens: keywords
 */
TestResult gt_keywords ()
{
    Lexer lexer {"int return if while", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 5
             && tokens[0].type == TokenType::INT_TYPE
             && tokens[1].type == TokenType::RETURN
             && tokens[2].type == TokenType::IF
             && tokens[3].type == TokenType::WHILE
             && tokens[4].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt keywords", .pass = pass};
}

/**
 * get_tokens: identifiers (including keyword prefixes)
 */
TestResult gt_identifiers ()
{
    Lexer lexer {"foo _bar integer returning", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 5
             && tokens[0].type == TokenType::IDENTIFIER
             && tokens[0].lexeme == "foo"

             && tokens[1].type == TokenType::IDENTIFIER
             && tokens[1].lexeme == "_bar"

             && tokens[2].type == TokenType::IDENTIFIER
             && tokens[2].lexeme == "integer"

             && tokens[3].type == TokenType::IDENTIFIER
             && tokens[3].lexeme == "returning"

             && tokens[4].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt identifiers", .pass = pass};
}

/**
 * get_tokens: single-character operators and punctuation
 */
TestResult gt_single_char_ops ()
{
    Lexer lexer {"+ - * / = < > ! ; ( ) { }", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 14
             && tokens[0].type  == TokenType::ADD_OP
             && tokens[1].type  == TokenType::SUB_OP
             && tokens[2].type  == TokenType::MULT_OP
             && tokens[3].type  == TokenType::DIV_OP
             && tokens[4].type  == TokenType::EQ_OP
             && tokens[5].type  == TokenType::LT_CMP
             && tokens[6].type  == TokenType::GT_CMP
             && tokens[7].type  == TokenType::NOT_OP
             && tokens[8].type  == TokenType::SEMICOLON
             && tokens[9].type  == TokenType::L_PAREN
             && tokens[10].type == TokenType::R_PAREN
             && tokens[11].type == TokenType::L_BRACE
             && tokens[12].type == TokenType::R_BRACE
             && tokens[13].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt single char operators", .pass = pass};
}

/**
 * get_tokens: two-character operators
 */
TestResult gt_two_char_ops ()
{
    Lexer lexer {"== != && ||", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 5
             && tokens[0].type == TokenType::EQ_CMP  && tokens[0].lexeme == "=="
             && tokens[1].type == TokenType::NE_CMP  && tokens[1].lexeme == "!="
             && tokens[2].type == TokenType::AND_CMP && tokens[2].lexeme == "&&"
             && tokens[3].type == TokenType::OR_CMP  && tokens[3].lexeme == "||"
             && tokens[4].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt two char operators", .pass = pass};
}

/**
 * get_tokens: line and column tracking across newlines
 */
TestResult gt_location_tracking ()
{
    Lexer lexer {"int x\nreturn", false};
    std::string input = "int x\nreturn";
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 4
             && tokens[0].start.line == 1 && tokens[0].start.col == 1 // int
             && tokens[1].start.line == 1 && tokens[1].start.col == 5 // x
             && tokens[2].start.line == 2 && tokens[2].start.col == 1 // return
             && tokens[3].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt location tracking", .pass = pass};
}

/**
 * get_tokens: unknown character produces UNKNOWN token
 */
TestResult gt_unknown ()
{
    Lexer lexer {"@", false};
    std::string input = "@";
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 2
             && tokens[0].type == TokenType::UNKNOWN
             && tokens[0].lexeme == "@"
             && tokens[1].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt unknown token", .pass = pass};
}

/**
 * get_tokens: full statement
 */
TestResult gt_full_statement ()
{
    Lexer lexer {"int x = 5;", false};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 6
             && tokens[0].type == TokenType::INT_TYPE
             && tokens[0].lexeme == "int"

             && tokens[1].type == TokenType::IDENTIFIER
             && tokens[1].lexeme == "x"

             && tokens[2].type == TokenType::EQ_OP
             && tokens[2].lexeme == "="

             && tokens[3].type == TokenType::INT_LITERAL
             && tokens[3].lexeme == "5"

             && tokens[4].type == TokenType::SEMICOLON  
             && tokens[4].lexeme == ";"

             && tokens[5].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt full statement", .pass = pass};
}

/**
 * get_tokens: full statement from file
 */
TestResult gt_full_file ()
{
    Lexer lexer {"/examples/statement.txt"};
    auto tokens = lexer.get_tokens ();

    bool pass = tokens.size () == 6
             && tokens[0].type == TokenType::INT_TYPE
             && tokens[0].lexeme == "int"

             && tokens[1].type == TokenType::IDENTIFIER
             && tokens[1].lexeme == "x"

             && tokens[2].type == TokenType::EQ_OP
             && tokens[2].lexeme == "="

             && tokens[3].type == TokenType::INT_LITERAL
             && tokens[3].lexeme == "5"

             && tokens[4].type == TokenType::SEMICOLON  
             && tokens[4].lexeme == ";"

             && tokens[5].type == TokenType::END_OF_FILE;

    return TestResult {.name = "gt full statement", .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_test (gt_empty);
    tb.add_test (gt_whitespace);
    tb.add_test (gt_int_literal);
    tb.add_test (gt_keywords);
    tb.add_test (gt_identifiers);
    tb.add_test (gt_single_char_ops);
    tb.add_test (gt_two_char_ops);
    tb.add_test (gt_location_tracking);
    tb.add_test (gt_unknown);
    tb.add_test (gt_full_statement);

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
    std::cout << "\n=======================" << std::endl;
}