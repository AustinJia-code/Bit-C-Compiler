/***
 * @file token.h
 * @brief Defines tokens.
 */

#include <cstdlib>
#include <array>
#include <string_view>

/**
 * Token types
 */
enum class TokenType
{
    RETURN,
    INT_LITERAL,
    ADD_OP,
    SUB_OP,
    DIV_OP,
    DEREF_OP,
    SEMICOLON,
    L_PAREN,
    R_PAREN,
    IDENTIFIER,
    END_OF_FILE
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