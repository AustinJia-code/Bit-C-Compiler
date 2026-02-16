/**
 * @file parser.hpp
 * @brief Recursive descent parser. Converts tokens into an AST.
 */

#pragma once

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>
#include "token.hpp"
#include "ast.hpp"

static constexpr size_t MAX_ID_LEN = 32;

/**
 * Parse error with source location
 */
class ParseError : public std::runtime_error
{
public:
    Location loc;

    ParseError (const std::string& msg, Location loc)
        : std::runtime_error (msg), loc (loc) {}
};

/**
 * Recursive descent parser
 */
class Parser
{
public:
    explicit Parser (const std::vector<Token>& tokens);

    /**
     * Parse entire token stream into a Program
     */
    Program parse ();

private:
    std::vector<Token> tokens_;
    size_t current_ = 0;

    /********** TOKEN NAVIGATION **********/
    const Token& peek () const;           // Look at cur token
    const Token& next ();                 // Move to next token
    const Token& prev () const;           // Move to prev token
    bool is_at_end () const;              // Check if at end of file
    bool check (TokenType type) const;    // Check current token type
    bool match (TokenType type);          // Check cur token, next if suc
    const Token& expect (TokenType type,  // Check cur token for type, err on fail
                         const std::string& msg);

    /********** EXPRESSIONS **********/
    std::unique_ptr<Expr> expression ();
    std::unique_ptr<Expr> logic_or ();
    std::unique_ptr<Expr> logic_and ();
    std::unique_ptr<Expr> comparison ();
    std::unique_ptr<Expr> addition ();
    std::unique_ptr<Expr> multiplication ();
    std::unique_ptr<Expr> unary ();
    std::unique_ptr<Expr> primary ();

    /********** STATEMENTS **********/
    Stmt statement ();
    Stmt declaration ();
    Stmt assignment_or_expr_stmt ();
    Stmt return_statement ();
    Stmt if_statement ();
    Stmt while_statement ();
    Block block ();

    /********** FUNCTION **********/
    Function function ();
};
