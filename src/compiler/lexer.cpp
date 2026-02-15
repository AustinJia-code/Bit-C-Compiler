/**
 * @file lexer.cpp
 * @brief lexer implementation
 */

#include <lexer.hpp>
#include <string>
#include <algorithm>
#include <cctype>
#include <set>
#include "file_utils.hpp"
#include "token.hpp"

void Lexer::string_to_tokens ()
{
    this->tokens = {};
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
            this->tokens.push_back ({TokenType::INT_LITERAL,
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
            while (i < input.size ()
               && (std::isalnum (input[i]) || input[i] == '_'))
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

            this->tokens.push_back ({type, {line, start_col}, lexeme});
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
                this->tokens.push_back ({token_type,
                                        {line, col},
                                        std::string_view {input.data () + i,
                                                          2}});
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

        this->tokens.push_back ({type,
                                {line, col},
                                std::string_view {input.data () + i, 1}});
        ++i;
        ++col;
    }

    this->tokens.push_back ({TokenType::END_OF_FILE,
                            {line, col},
                            {}});
}

Lexer::Lexer (const std::string& in_str, bool file_flag)
    : tokens {}
{
    if (file_flag)
    {
        this->file_path = in_str;
        this->input = file_to_string (in_str);
    }
    else
    {
        this->file_path = {};
        this->input = in_str;
    }
}

std::vector<Token> Lexer::get_tokens ()
{
    if (this->tokens.empty ())
        this->string_to_tokens ();
    
    return tokens;
}