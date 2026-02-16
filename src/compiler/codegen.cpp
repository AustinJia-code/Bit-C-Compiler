/**
 * @file codegen.cpp
 * @brief Stack-based codegen implementation
 *
 * All expression evaluation uses the stack:
 *   - Literals/variables: push value
 *   - Binary ops: pop two, compute, push result
 *   - Return: pop into eax
 */

#include "codegen.hpp"
#include <iostream>
#include <string>

Codegen::Codegen (const Program& prog)
    : label_counter_ {2}, next_var_offset_ {0}
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
    // Reset variable state per function
    var_offsets_.clear ();
    next_var_offset_ = 0;

    // Label
    std::string name = func.name == "main" ? "main" : func.name + "()";
    emit (name + ":");

    // Prologue
    emit ("    push rbp");
    emit ("    mov rbp, rsp");

    // Body
    gen_block (func.body);

    // Epilogue
    emit ("    mov rsp, rbp");
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
        using T = std::decay_t<decltype (node)>;

        // Return statement
        if constexpr (std::is_same_v<T, ReturnStmt>)
        {
            gen_expr (*node.value);
            emit ("    pop rax");
            emit ("    mov rsp, rbp");
            emit ("    pop rbp");
            emit ("    ret");
        }

        // Variable declaration
        else if constexpr (std::is_same_v<T, VarDecl>)
        {
            next_var_offset_ -= 8;
            var_offsets_[node.name] = next_var_offset_;
            emit ("    sub rsp, 8");

            if (node.init.has_value ())
            {
                gen_expr (*node.init.value ());
                emit ("    pop rax");
                emit ("    mov DWORD PTR [rbp +" +
                            std::to_string (next_var_offset_) + "], eax");
            }
        }

        // Variable assignment
        else if constexpr (std::is_same_v<T, Assignment>)
        {
            gen_expr (*node.value);
            emit ("    pop rax");

            int offset = var_offsets_.at (node.name);
            emit ("    mov DWORD PTR [rbp +" + std::to_string (offset) + "], eax");
        }

        // If statement
        else if constexpr (std::is_same_v<T, IfStmt>)
        {
            std::string else_label = ".L" + std::to_string (label_counter_++);
            std::string end_label = ".L" + std::to_string (label_counter_++);

            gen_expr (*node.condition);
            emit ("    pop rax");
            emit ("    test eax, eax");
            emit ("    je " + else_label);

            gen_block (*node.then_block);
            emit ("    jmp " + end_label);

            emit (else_label + ":");
            emit (end_label + ":");
        }

        // While statement
        else if constexpr (std::is_same_v<T, WhileStmt>)
        {
            std::string loop_label = ".L" + std::to_string (label_counter_++);
            std::string end_label = ".L" + std::to_string (label_counter_++);

            emit (loop_label + ":");
            gen_expr (*node.condition);
            emit ("    pop rax");
            emit ("    test eax, eax");
            emit ("    je " + end_label);

            gen_block (*node.body);
            emit ("    jmp " + loop_label);

            emit (end_label + ":");
        }

        // Block
        else if constexpr (std::is_same_v<T, Block>)
        {
            gen_block (node);
        }

        // Expression
        else if constexpr (std::is_same_v<T, ExprStmt>)
        {
            gen_expr (*node.expression);
            emit ("    pop rax");
        }

        // Default
        else
        {
            throw GenError ("Unsupported statement type");
        }
    }, stmt.node);
}

void Codegen::gen_expr (const Expr& expr)
{
    std::visit ([this] (const auto& node)
    {
        using T = std::decay_t<decltype (node)>;

        // Int literal
        if constexpr (std::is_same_v<T, IntLiteral>)
        {
            emit ("    push " + std::to_string (node.value));
        }
        
        // Identifier
        else if constexpr (std::is_same_v<T, Identifier>)
        {
            int offset = var_offsets_.at (node.name);
            emit ("    mov eax, DWORD PTR [rbp +" + std::to_string (offset) + "]");
            emit ("    push rax");
        }

        // Unary op
        else if constexpr (std::is_same_v<T, UnaryOp>)
        {
            gen_expr (*node.operand);
            emit ("    pop rax");

            if (node.op == UnaryOp::Op::NEGATE)
                emit ("    neg eax");
            else if (node.op == UnaryOp::Op::NOT)
            {
                emit ("    test eax, eax");
                emit ("    sete al");
                emit ("    movzx eax, al");
            }

            emit ("    push rax");
        }

        // Binary op
        else if constexpr (std::is_same_v<T, BinaryOp>)
        {
            // Left operand pushed first, then right
            gen_expr (*node.left);
            gen_expr (*node.right);

            emit ("    pop rbx");
            emit ("    pop rax");

            switch (node.op)
            {
                case BinaryOp::Op::ADD:
                    emit ("    add eax, ebx");
                    break;
                case BinaryOp::Op::SUB:
                    emit ("    sub eax, ebx");
                    break;
                case BinaryOp::Op::MUL:
                    emit ("    imul eax, ebx");
                    break;
                case BinaryOp::Op::DIV:
                    emit ("    cdq");
                    emit ("    idiv ebx");
                    break;
                case BinaryOp::Op::EQ:
                    emit ("    cmp eax, ebx");
                    emit ("    sete al");
                    emit ("    movzx eax, al");
                    break;
                case BinaryOp::Op::NE:
                    emit ("    cmp eax, ebx");
                    emit ("    setne al");
                    emit ("    movzx eax, al");
                    break;
                case BinaryOp::Op::LT:
                    emit ("    cmp eax, ebx");
                    emit ("    setl al");
                    emit ("    movzx eax, al");
                    break;
                case BinaryOp::Op::GT:
                    emit ("    cmp eax, ebx");
                    emit ("    setg al");
                    emit ("    movzx eax, al");
                    break;
                case BinaryOp::Op::AND:
                    emit ("    test eax, eax");
                    emit ("    setne al");
                    emit ("    test ebx, ebx");
                    emit ("    setne bl");
                    emit ("    and al, bl");
                    emit ("    movzx eax, al");
                    break;
                case BinaryOp::Op::OR:
                    emit ("    or eax, ebx");
                    emit ("    test eax, eax");
                    emit ("    setne al");
                    emit ("    movzx eax, al");
                    break;
            }

            emit ("    push rax");
        }

        // Default
        else
        {
            throw GenError ("Unsupported expression type");
        }

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
