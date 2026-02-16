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
private:
    std::string file_path_;
    std::string input_;
    std::vector<Token> tokens_;

    /**
     * Converts input string into tokens
     * Sets empty vector on failure
     */
    void string_to_tokens ();
    
public:
    /**
     * Constructor, unset file_flag if input is a raw string
     */
    Lexer (const std::string& in_str, bool file_flag = 1);

    /**
     * Parse input and return tokens
     */
    std::vector<Token> get_tokens ();
};