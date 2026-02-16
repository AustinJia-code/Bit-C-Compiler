/**
 * @file codegen.hpp
 * @brief Codegen implementation
 */

#include "codegen.hpp"
#include <iostream>
#include <string>

Codegen::Codegen (const Program& prog)
    : label_counter_ {2}
{
    // Scan for main
    bool found_main = false;
    for (const auto& func : prog.functions)
    {
        if (func.name == "main")
        {
            found_main = true;
            break;
        }
    }

    if (!found_main)
        throw GenError ("No entry found");

    // Generate assembly for each function
    for (const auto& func : prog.functions)
        gen_function (func);
}

void Codegen::emit (const std::string& line)
{
    assembly_lines_.push_back (line);
}

void Codegen::gen_function (const Function& func)
{
    // Label
    std::string name = func.name == "main" ? "main" : func.name + "()";
    emit (name + ":");

    // Prologue
    emit ("    push rbp");
    emit ("    mov rbp, rsp");

    // Body
    gen_block (func.body);

    // Epilogue
    emit ("    pop rbp");
    emit ("    ret");
}

void Codegen::gen_block (const Block& block)
{
    for (const auto& stmt : block.statements)
        gen_stmt (stmt);
}

void Codegen::gen_stmt (const Stmt& stmt)
{
    std::visit ([this] (const auto& node)
    {
        // decay_t gives naked type
        using T = std::decay_t<decltype (node)>;

        if constexpr (std::is_same_v<T, ReturnStmt>)
            gen_expr (*node.value);
        else
            throw GenError ("Unsupported statement type");
    }, stmt.node);
}

void Codegen::gen_expr (const Expr& expr)
{
    std::visit ([this] (const auto& node)
    {
        using T = std::decay_t<decltype (node)>;

        if constexpr (std::is_same_v<T, IntLiteral>)
            emit ("    mov eax, " + std::to_string (node.value));
        else
            throw GenError ("Unsupported expression type");

    }, expr.node);
}

std::string Codegen::get_assembly () const
{
    std::string assembly_str {};

    // To make g++ happy (not included in godbolt gens)
    assembly_str.append (".intel_syntax noprefix\n.global main\n\n");

    for (size_t line_num = 0; line_num < assembly_lines_.size (); ++line_num)
    {
        assembly_str.append (assembly_lines_[line_num]);
        if (line_num < assembly_lines_.size () - 1)
            assembly_str.append ("\n");
    }

    return assembly_str;
}
