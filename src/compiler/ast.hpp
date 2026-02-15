/**
 * @file ast.hpp
 * @brief Abstract Syntax Tree node types.
 * 
 * program
 * └── functions
 *      └── block of statements [declaration | return | if | while | block]
 *           └── expressions
 */

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Forward declarations for recursive node types
struct Expr;            // Code that distills into a value
struct Stmt;            // Code that controls execution, non-computational
struct Block;           // Code to be run in order

/********** EXPRESSION NODES **********/
struct IntLiteral
{
    int value;
};

struct Identifier
{
    std::string name;
};

struct UnaryOp
{
    enum class Op
    {
        NEGATE,
        NOT
    };
    
    Op op;
    std::unique_ptr<Expr> operand;
};

struct BinaryOp
{
    enum class Op
    {
        ADD, SUB, MUL, DIV,
        EQ, NE, LT, GT,
        AND, OR
    };
    Op op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Expr
{
    std::variant<IntLiteral, Identifier, UnaryOp, BinaryOp> node;
};

/********** STATEMENT NODES **********/
struct VarDecl
{
    std::string name;
    std::optional<std::unique_ptr<Expr>> init;
};

struct Assignment
{
    std::string name;
    std::unique_ptr<Expr> value;
};

struct ReturnStmt
{
    std::unique_ptr<Expr> value;
};

struct IfStmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> then_block;
};

struct WhileStmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Block> body;
};

struct ExprStmt         // Expression, but don't care about value
{
    std::unique_ptr<Expr> expression;
};

struct Block
{
    std::vector<Stmt> statements;
};

struct Stmt
{
    std::variant<VarDecl, Assignment, ReturnStmt,
                 IfStmt, WhileStmt, Block, ExprStmt> node;
};

/********** Top-Level **********/
struct Function
{
    std::string name;
    Block body;
};

struct Program
{
    std::vector<Function> functions;
};
