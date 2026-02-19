/**
 * @file codegen.hpp
 * @brief Emits assembly (.s).
 */

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "ast.hpp"

/**
 * Codegen error
 */
class GenError : public std::runtime_error
{
public:
    GenError (const std::string& msg)
        : std::runtime_error (msg) {}
};

/**
 * Codegen (register-based expression evaluation with spill to stack)
 */
class Codegen
{
private:
    size_t label_counter_;
    std::vector<std::string> assembly_lines_;

    // Local variable tracking (name to rbp offset)
    std::unordered_map<std::string, int> var_offsets_;
    int next_var_offset_;

    // Scratch register pool: rbx (0), r12 (1), r13 (2)
    bool reg_used_[3];
    std::string epilogue_label_;

    void emit (const std::string& line);
    std::string alloc_reg ();
    void free_reg (const std::string& reg);
    void gen_function (const Function& func);
    void gen_block (const Block& block);
    void gen_stmt (const Stmt& stmt);
    std::string gen_expr (const Expr& expr);

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
