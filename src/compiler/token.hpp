/***
 * @file token.h
 * @brief Defines tokens.
 */

#pragma once

#include <cstdlib>
#include <array>
#include <string_view>

/**
 * Token types
 */
enum class TokenType
{
    // Special
    END_OF_FILE,
    UNKNOWN,

    // Literals
    INT_LITERAL,

    // Identifiers
    IDENTIFIER,

    // Keywords
    INT_TYPE,
    RETURN,
    IF,
    WHILE,

    // Operators
    ADD_OP,    // +
    SUB_OP,    // -
    MULT_OP,   // *
    DIV_OP,    // /

    // Set/Comparison
    EQ_OP,     // =
    EQ_CMP,    // ==
    NE_CMP,    // !=
    LT_CMP,    // <
    GT_CMP,    // >

    // Logical Operators
    AND_CMP,   // &&
    OR_CMP,    // ||
    NOT_OP,    // !

    // Punctuation
    SEMICOLON, // ;
    L_PAREN,   // (
    R_PAREN,   // )
    L_BRACE,   // {
    R_BRACE,   // }
    COMMA      // ,
};


/**
 * Location for debugger
 */
struct Location
{
    size_t line;
    size_t col;
};

/**
 * Token class
 */
struct Token
{
    TokenType type;
    Location start;
    std::string_view lexeme;
};