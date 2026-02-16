/**
 * @file codegen.hpp
 * @brief Emits assembly (.s).
 */

#pragma once

#include <vector>
#include <string>
#include "ast.hpp"

/**
 * Parse error with source location
 */
class GenError : public std::runtime_error
{
public:
    GenError (const std::string& msg)
        : std::runtime_error (msg) {}
};

/**
 * Codegen
 */
class Codegen
{
private:
    size_t label_counter_;
    std::vector<std::string> assembly_lines_;

    void emit (const std::string& line);
    void gen_function (const Function& func);
    void gen_block (const Block& block);
    void gen_stmt (const Stmt& stmt);
    void gen_expr (const Expr& expr);

public:
    /**
     * Program constructor
     */
    Codegen (const Program& program);

    /**
     * Get generated assembly as a string
     */
    std::string get_assembly () const;
};