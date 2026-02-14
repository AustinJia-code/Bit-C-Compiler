/**
 * @file lexer.hpp
 * @brief Converts code into tokens.
 */

#pragma once

#include <string>
#include <vector>
#include "token.hpp"

/**
 * Converts input into tokens
 */
class Lexer
{
public:
    /**
     * Convert input file to string
     * Returns empty string on failure
     */
    std::string file_to_string (const std::string& file_path) const;

    /**
     * Converts input string into tokens
     * Returns empty vector on failure
     */
    std::vector<Token> string_to_tokens (const std::string& input) const;
};