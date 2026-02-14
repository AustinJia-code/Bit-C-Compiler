/**
 * @file lexer.cpp
 * @brief lexer implementation
 */

#include <lexer.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "paths.hpp"

std::string Lexer::file_to_string (const std::string& file_path) const
{
    std::ifstream file (get_full_path (file_path));
    if (!file.is_open ())
    {
        std::cerr << "Error: could not open " << file_path << std::endl;
        return std::string {};
    }

    std::string out {std::istreambuf_iterator<char> {file},
                     std::istreambuf_iterator<char> {}};

    return out;
}

std::vector<Token> Lexer::string_to_tokens (const std::string& input) const
{
    return std::vector<Token> {};
}