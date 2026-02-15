/**
 * @file parser.cpp
 * @brief Parser implementation
 */

#include "parser.hpp"
#include <sstream>

Parser::Parser (const std::vector<Token>& tokens)
    : tokens_ (tokens) {}

/********** TOKEN NAVIGATION **********/
const Token& Parser::peek () const
{
    return tokens_[current_];
}

const Token& Parser::next ()
{
    if (!is_at_end ())
        current_++;

    return prev ();
}

const Token& Parser::prev () const
{
    return tokens_[current_ - 1];
}

bool Parser::is_at_end () const
{
    return peek ().type == TokenType::END_OF_FILE;
}

bool Parser::check (TokenType type) const
{
    if (is_at_end ())
        return false;
        
    return peek ().type == type;
}

bool Parser::match (TokenType type)
{
    if (check (type))
    {
        next ();
        return true;
    }
    return false;
}

const Token& Parser::expect (TokenType type, const std::string& msg)
{
    if (check (type))
        return next ();

    throw ParseError (msg, peek ().start);
}

/********** EXPRESSIONS **********/
std::unique_ptr<Expr> Parser::expression ()
{
    return logic_or ();     // Precedence climbing
}

std::unique_ptr<Expr> Parser::logic_or ()
{
    auto left = logic_and ();

    while (match (TokenType::OR_CMP))
    {
        auto right = logic_and ();
        auto bin = std::make_unique<Expr> ();
        bin->node = BinaryOp {BinaryOp::Op::OR,
                              std::move (left), std::move (right)};
        left = std::move (bin);
    }

    return left;
}

std::unique_ptr<Expr> Parser::logic_and ()
{
    auto left = comparison ();

    while (match (TokenType::AND_CMP))
    {
        auto right = comparison ();
        auto bin = std::make_unique<Expr> ();
        bin->node = BinaryOp {BinaryOp::Op::AND,
                              std::move (left), std::move (right)};
        left = std::move (bin);
    }

    return left;
}

std::unique_ptr<Expr> Parser::comparison ()
{
    auto left = addition ();

    while (true)
    {
        BinaryOp::Op op;
        if (match (TokenType::EQ_CMP))       op = BinaryOp::Op::EQ;
        else if (match (TokenType::NE_CMP))  op = BinaryOp::Op::NE;
        else if (match (TokenType::LT_CMP))  op = BinaryOp::Op::LT;
        else if (match (TokenType::GT_CMP))  op = BinaryOp::Op::GT;
        else break;

        auto right = addition ();
        auto bin = std::make_unique<Expr> ();
        bin->node = BinaryOp {op, std::move (left), std::move (right)};
        left = std::move (bin);
    }

    return left;
}

std::unique_ptr<Expr> Parser::addition ()
{
    auto left = multiplication ();

    while (true)
    {
        BinaryOp::Op op;
        if (match (TokenType::ADD_OP))       op = BinaryOp::Op::ADD;
        else if (match (TokenType::SUB_OP))  op = BinaryOp::Op::SUB;
        else break;

        auto right = multiplication ();
        auto bin = std::make_unique<Expr> ();
        bin->node = BinaryOp {op, std::move (left), std::move (right)};
        left = std::move (bin);
    }

    return left;
}

std::unique_ptr<Expr> Parser::multiplication ()
{
    auto left = unary ();

    while (true)
    {
        BinaryOp::Op op;
        if (match (TokenType::MULT_OP))      op = BinaryOp::Op::MUL;
        else if (match (TokenType::DIV_OP))  op = BinaryOp::Op::DIV;
        else break;

        auto right = unary ();
        auto bin = std::make_unique<Expr> ();
        bin->node = BinaryOp {op, std::move (left), std::move (right)};
        left = std::move (bin);
    }

    return left;
}

std::unique_ptr<Expr> Parser::unary ()
{
    if (match (TokenType::SUB_OP))
    {
        auto operand = unary ();
        auto expr = std::make_unique<Expr> ();
        expr->node = UnaryOp {UnaryOp::Op::NEGATE, std::move (operand)};
        return expr;
    }

    if (match (TokenType::NOT_OP))
    {
        auto operand = unary ();
        auto expr = std::make_unique<Expr> ();
        expr->node = UnaryOp {UnaryOp::Op::NOT, std::move (operand)};
        return expr;
    }

    return primary ();
}

std::unique_ptr<Expr> Parser::primary ()
{
    // Integer literal
    if (match (TokenType::INT_LITERAL))
    {
        auto expr = std::make_unique<Expr> ();
        int value = std::stoi (std::string {prev ().lexeme});
        expr->node = IntLiteral {value};
        return expr;
    }

    // Identifier
    if (match (TokenType::IDENTIFIER))
    {
        auto expr = std::make_unique<Expr> ();
        expr->node = Identifier {std::string {prev ().lexeme}};
        return expr;
    }

    // Parenthesized expression
    if (match (TokenType::L_PAREN))
    {
        auto expr = expression ();
        expect (TokenType::R_PAREN, "expected ')' after expression");
        return expr;
    }

    throw ParseError ("expected expression", peek ().start);
}

/********** STATEMENTS **********/
Stmt Parser::statement ()
{
    if (check (TokenType::INT_TYPE))
        return declaration ();

    if (check (TokenType::RETURN))
        return return_statement ();

    if (check (TokenType::IF))
        return if_statement ();

    if (check (TokenType::WHILE))
        return while_statement ();

    if (check (TokenType::L_BRACE))
    {
        auto b = block ();
        return Stmt {std::move (b)};
    }

    return assignment_or_expr_stmt ();
}

Stmt Parser::declaration ()
{
    expect (TokenType::INT_TYPE, "expected 'int'");

    const Token& name_tok = expect (TokenType::IDENTIFIER,
                                    "expected variable name");
    std::string name {name_tok.lexeme};

    if (name.length () > MAX_ID_LEN)
        throw ParseError ("identifier exceeds maximum length", name_tok.start);

    std::optional<std::unique_ptr<Expr>> init;

    if (match (TokenType::EQ_OP))
        init = expression ();

    expect (TokenType::SEMICOLON, "expected ';' after declaration");

    return Stmt {VarDecl {std::move (name), std::move (init)}};
}

Stmt Parser::assignment_or_expr_stmt ()
{
    // Lookahead: IDENTIFIER followed by '=' means assignment
    if (check (TokenType::IDENTIFIER)
        && current_ + 1 < tokens_.size ()
        && tokens_[current_ + 1].type == TokenType::EQ_OP)
    {
        const Token& name_tok = next ();
        std::string name {name_tok.lexeme};
        next (); // consume '='
        auto value = expression ();
        expect (TokenType::SEMICOLON, "expected ';' after assignment");
        return Stmt {Assignment {std::move (name), std::move (value)}};
    }

    // Otherwise it's an expression statement
    auto expr = expression ();
    expect (TokenType::SEMICOLON, "expected ';' after expression");
    return Stmt {ExprStmt {std::move (expr)}};
}

Stmt Parser::return_statement ()
{
    expect (TokenType::RETURN, "expected 'return'");
    auto value = expression ();
    expect (TokenType::SEMICOLON, "expected ';' after return value");
    return Stmt {ReturnStmt {std::move (value)}};
}

Stmt Parser::if_statement ()
{
    expect (TokenType::IF, "expected 'if'");
    expect (TokenType::L_PAREN, "expected '(' after 'if'");
    auto condition = expression ();
    expect (TokenType::R_PAREN, "expected ')' after if condition");
    auto then_block = std::make_unique<Block> (block ());
    return Stmt {IfStmt {std::move (condition), std::move (then_block)}};
}

Stmt Parser::while_statement ()
{
    expect (TokenType::WHILE, "expected 'while'");
    expect (TokenType::L_PAREN, "expected '(' after 'while'");
    auto condition = expression ();
    expect (TokenType::R_PAREN, "expected ')' after while condition");
    auto body = std::make_unique<Block> (block ());
    return Stmt {WhileStmt {std::move (condition), std::move (body)}};
}

Block Parser::block ()
{
    expect (TokenType::L_BRACE, "expected '{'");

    std::vector<Stmt> stmts;
    while (!check (TokenType::R_BRACE) && !is_at_end ())
        stmts.push_back (statement ());

    expect (TokenType::R_BRACE, "expected '}'");

    return Block {std::move (stmts)};
}

/********** FUNCTIONS **********/
Function Parser::function ()
{
    expect (TokenType::INT_TYPE, "expected 'int' return type");

    const Token& name_tok = expect (TokenType::IDENTIFIER,
                                     "expected function name");
    std::string name {name_tok.lexeme};

    if (name.length () > MAX_ID_LEN)
        throw ParseError ("function name exceeds maximum length",
                           name_tok.start);

    expect (TokenType::L_PAREN, "expected '(' after function name");
    expect (TokenType::R_PAREN, "expected ')' after parameters");

    auto body = block ();

    return Function {std::move (name), std::move (body)};
}

Program Parser::parse ()
{
    Program program;

    while (!is_at_end ())
        program.functions.push_back (function ());

    return program;
}
