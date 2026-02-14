/**
 * @file lexer_tests.cpp
 * @brief Tests for the Lexer class
 */

#include <vector>
#include <iostream>
#include <testbench.hpp>
#include <lexer.hpp>
#include <string>

/**
 * file_to_string: basic functionality
 */
TestResult fts_1 ()
{
    Lexer lexer {};

    std::string out = lexer.file_to_string ("tests/compiler/data/sentence.txt");
    
    return TestResult {.name = "fts basic functionality",
                       .pass = (out == "This is a \nnew line.")};
}

/**
 * file_to_string: proper error handling with bad path
 */
TestResult fts_2 ()
{
    Lexer lexer {};
    std::string out = lexer.file_to_string ("bad/path");

    return TestResult {.name = "fts bad path",
                       .pass = out.empty ()};
}

/**
 * string_to_tokens: empty input produces only EOF
 */
TestResult stt_empty ()
{
    Lexer lexer {};
    std::string input = "";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 1
             && tokens[0].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt empty input", .pass = pass};
}

/**
 * string_to_tokens: whitespace-only input produces only EOF
 */
TestResult stt_whitespace ()
{
    Lexer lexer {};
    std::string input = "   \n\t  \n  ";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 1
             && tokens[0].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt whitespace only", .pass = pass};
}

/**
 * string_to_tokens: integer literal
 */
TestResult stt_int_literal ()
{
    Lexer lexer {};
    std::string input = "42";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 2
             && tokens[0].type == TokenType::INT_LITERAL
             && tokens[0].lexeme == "42"
             && tokens[0].start.line == 1
             && tokens[0].start.col == 1
             && tokens[1].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt int literal", .pass = pass};
}

/**
 * string_to_tokens: keywords
 */
TestResult stt_keywords ()
{
    Lexer lexer {};
    std::string input = "int return if while";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 5
             && tokens[0].type == TokenType::INT_TYPE
             && tokens[1].type == TokenType::RETURN
             && tokens[2].type == TokenType::IF
             && tokens[3].type == TokenType::WHILE
             && tokens[4].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt keywords", .pass = pass};
}

/**
 * string_to_tokens: identifiers (including keyword prefixes)
 */
TestResult stt_identifiers ()
{
    Lexer lexer {};
    std::string input = "foo _bar integer returning";
    auto tokens = lexer.string_to_tokens (input);

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

    return TestResult {.name = "stt identifiers", .pass = pass};
}

/**
 * string_to_tokens: single-character operators and punctuation
 */
TestResult stt_single_char_ops ()
{
    Lexer lexer {};
    std::string input = "+ - * / = < > ! ; ( ) { }";
    auto tokens = lexer.string_to_tokens (input);

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

    return TestResult {.name = "stt single char operators", .pass = pass};
}

/**
 * string_to_tokens: two-character operators
 */
TestResult stt_two_char_ops ()
{
    Lexer lexer {};
    std::string input = "== != && ||";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 5
             && tokens[0].type == TokenType::EQ_CMP  && tokens[0].lexeme == "=="
             && tokens[1].type == TokenType::NE_CMP  && tokens[1].lexeme == "!="
             && tokens[2].type == TokenType::AND_CMP && tokens[2].lexeme == "&&"
             && tokens[3].type == TokenType::OR_CMP  && tokens[3].lexeme == "||"
             && tokens[4].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt two char operators", .pass = pass};
}

/**
 * string_to_tokens: line and column tracking across newlines
 */
TestResult stt_location_tracking ()
{
    Lexer lexer {};
    std::string input = "int x\nreturn";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 4
             && tokens[0].start.line == 1 && tokens[0].start.col == 1 // int
             && tokens[1].start.line == 1 && tokens[1].start.col == 5 // x
             && tokens[2].start.line == 2 && tokens[2].start.col == 1 // return
             && tokens[3].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt location tracking", .pass = pass};
}

/**
 * string_to_tokens: unknown character produces UNKNOWN token
 */
TestResult stt_unknown ()
{
    Lexer lexer {};
    std::string input = "@";
    auto tokens = lexer.string_to_tokens (input);

    bool pass = tokens.size () == 2
             && tokens[0].type == TokenType::UNKNOWN
             && tokens[0].lexeme == "@"
             && tokens[1].type == TokenType::END_OF_FILE;

    return TestResult {.name = "stt unknown token", .pass = pass};
}

/**
 * string_to_tokens: full statement
 */
TestResult stt_full_statement ()
{
    Lexer lexer {};
    std::string input = "int x = 5;";
    auto tokens = lexer.string_to_tokens (input);

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

    return TestResult {.name = "stt full statement", .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_test (fts_1);
    tb.add_test (fts_2);
    tb.add_test (stt_empty);
    tb.add_test (stt_whitespace);
    tb.add_test (stt_int_literal);
    tb.add_test (stt_keywords);
    tb.add_test (stt_identifiers);
    tb.add_test (stt_single_char_ops);
    tb.add_test (stt_two_char_ops);
    tb.add_test (stt_location_tracking);
    tb.add_test (stt_unknown);
    tb.add_test (stt_full_statement);

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
    std::cout << "\n=======================" << std::endl;

}