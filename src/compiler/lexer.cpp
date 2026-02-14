/**
 * @file lexer.cpp
 * @brief lexer implementation
 */

#include <lexer.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <algorithm>
#include <cctype>
#include <set>
#include "paths.hpp"
#include "token.hpp"

std::string Lexer::file_to_string (const std::string& file_path) const
{
    std::ifstream file (get_full_path (file_path));
    if (!file.is_open ())
    {
        std::cerr << "Error: could not open " << file_path << std::endl;
        return std::string {};
    }
        
    return {std::istreambuf_iterator<char> {file},
            std::istreambuf_iterator<char> {}};
}

std::vector<Token> Lexer::string_to_tokens (const std::string& input) const
{
    std::vector<Token> tokens;
    size_t i = 0;
    size_t line = 1;
    size_t col = 1;

    while (i < input.size ())
    {
        char c = input[i];

        // Skip whitespace
        if (std::isspace (c))
        {
            if (c == '\n')
            {
                line++;
                col = 1;
            }
            else
            {
                col++;
            }
            i++;
            continue;
        }

        // Scan number
        if (std::isdigit (c))
        {
            size_t start = i;
            size_t start_col = col;
            while (i < input.size () && std::isdigit (input[i]))
            {
                i++;
                col++;
            }
            tokens.push_back ({TokenType::INT_LITERAL,
                               {line, start_col},
                               std::string_view {input.data () + start,
                                                 i - start}});
            continue;
        }

        // Scan identifier or keyword
        if (std::isalpha (c) || c == '_')
        {
            size_t start = i;
            size_t start_col = col;
            while (i < input.size () && (std::isalnum (input[i]) || input[i] == '_'))
            {
                i++;
                col++;
            }
            std::string_view lexeme {input.data () + start, i - start};

            TokenType type = TokenType::IDENTIFIER;
            if (lexeme == "int")         type = TokenType::INT_TYPE;
            else if (lexeme == "return") type = TokenType::RETURN;
            else if (lexeme == "if")     type = TokenType::IF;
            else if (lexeme == "while")  type = TokenType::WHILE;

            tokens.push_back ({type, {line, start_col}, lexeme});
            continue;
        }

        // Two-character operators
        if (i + 1 < input.size ())
        {
            char next = input[i + 1];
            TokenType token_type = TokenType::UNKNOWN;

            if (c == '=' && next == '=') token_type = TokenType::EQ_CMP;
            if (c == '&' && next == '&') token_type = TokenType::AND_CMP;
            if (c == '|' && next == '|') token_type = TokenType::OR_CMP;
            if (c == '!' && next == '=') token_type = TokenType::NE_CMP;

            if (token_type != TokenType::UNKNOWN)
            {
                tokens.push_back ({token_type,
                                   {line, col},
                                   std::string_view {input.data () + i, 2}});
                i += 2;
                col += 2;
                continue;
            }
        }

        // Single-character operators and punctuation
        TokenType type = TokenType::UNKNOWN;
        switch (c)
        {
            case '+': type = TokenType::ADD_OP;    break;
            case '-': type = TokenType::SUB_OP;    break;
            case '*': type = TokenType::MULT_OP;   break;
            case '/': type = TokenType::DIV_OP;    break;
            case '=': type = TokenType::EQ_OP;     break;
            case '<': type = TokenType::LT_CMP;    break;
            case '>': type = TokenType::GT_CMP;    break;
            case '!': type = TokenType::NOT_OP;    break;
            case ';': type = TokenType::SEMICOLON; break;
            case '(': type = TokenType::L_PAREN;   break;
            case ')': type = TokenType::R_PAREN;   break;
            case '{': type = TokenType::L_BRACE;   break;
            case '}': type = TokenType::R_BRACE;   break;
            default: break;
        }

        tokens.push_back ({type,
                           {line, col},
                           std::string_view {input.data () + i, 1}});
        ++i;
        ++col;
    }

    tokens.push_back ({TokenType::END_OF_FILE,
                       {line, col},
                       {}});
    return tokens;
}