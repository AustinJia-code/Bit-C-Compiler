/**
 * @file codegen.cpp
 * @brief Register-based codegen implementation with spill to stack
 */

#include "codegen.hpp"
#include <string>

/**
 * Callee saved scratch registers
 * Keep at 3 for now, handles binary op and dont feel like polluting stack
 */
static const char* regs_str_64[] = {"rbx", "r12", "r13"};

/**
 * 64-bit register name to its 32-bit version
 * Empty on failure
 */
static std::string reg32 (const std::string& reg64)
{
    if (reg64 == "rbx") return "ebx";
    if (reg64 == "r12") return "r12d";
    if (reg64 == "r13") return "r13d";
    return "";
}

/**
 * 64-bit register name to its 8-bit version
 * Empty on failure
 */
static std::string reg8 (const std::string& reg64)
{
    if (reg64 == "rbx") return "bl";
    if (reg64 == "r12") return "r12b";
    if (reg64 == "r13") return "r13b";
    return "";
}

Codegen::Codegen (const Program& prog)
    : label_counter_ {2}, next_var_offset_ {0}, reg_used_ {}
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

/**
 * Return first free scratch register and mark it used, or "" if all busy
 */
std::string Codegen::alloc_reg ()
{
    for (int i = 0; i < 3; ++i)
    {
        if (!reg_used_[i])
        {
            reg_used_[i] = true;
            return regs_str_64[i];
        }
    }
    return "";
}

/**
 * Mark a scratch register as free (no-op for empty string)
 */ 
void Codegen::free_reg (const std::string& reg)
{
    for (int i = 0; i < 3; ++i)
    {
        if (reg == regs_str_64[i])
        {
            reg_used_[i] = false;
            return;
        }
    }
}

void Codegen::gen_function (const Function& func)
{
    // Reset per-function state
    var_offsets_.clear ();
    next_var_offset_ = -24;   // rbx/r12/r13 in [-8, -24]
    reg_used_[0] = reg_used_[1] = reg_used_[2] = false;
    epilogue_label_ = ".Lfunc_" + std::to_string (label_counter_++);

    // Label
    emit (func.name + ":");

    // Prologue: save fp, and callee-saved regs
    emit ("    push rbp");
    emit ("    mov rbp, rsp");
    emit ("    push rbx");
    emit ("    push r12");
    emit ("    push r13");

    // Move parameters from ABI registers into local stack slots
    if (func.params.size () > 6)
        throw GenError ("Function '" + func.name + "' has more than 6 parameters");

    static const char* arg_regs[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    for (size_t i = 0; i < func.params.size (); ++i)
    {
        next_var_offset_ -= 8;
        var_offsets_[func.params[i].name] = next_var_offset_;
        emit ("    sub rsp, 8");
        emit ("    mov DWORD PTR [rbp +" +
              std::to_string (next_var_offset_) + "], " + arg_regs[i]);
    }

    // Body
    gen_block (func.body);

    // Epilogue: common return label jumped to by all ReturnStmt nodes
    emit (epilogue_label_ + ":");
    emit ("    lea rsp, [rbp - 24]");   // restore rsp above saved scratch regs
    emit ("    pop r13");
    emit ("    pop r12");
    emit ("    pop rbx");
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
            std::string reg = gen_expr (*node.value);
            if (!reg.empty ())
            {
                emit ("    mov eax, " + reg32 (reg));
                free_reg (reg);
            }
            else
                emit ("    pop rax");
            emit ("    jmp " + epilogue_label_);
        }

        // Variable declaration
        else if constexpr (std::is_same_v<T, VarDecl>)
        {
            next_var_offset_ -= 8;
            var_offsets_[node.name] = next_var_offset_;
            emit ("    sub rsp, 8");

            if (node.init.has_value ())
            {
                std::string reg = gen_expr (*node.init.value ());
                if (!reg.empty ())
                {
                    emit ("    mov DWORD PTR [rbp +" +
                               std::to_string (next_var_offset_) + "], " +
                               reg32 (reg));
                    free_reg (reg);
                }
                else
                {
                    emit ("    pop rax");
                    emit ("    mov DWORD PTR [rbp +" +
                               std::to_string (next_var_offset_) + "], eax");
                }
            }
        }

        // Variable assignment
        else if constexpr (std::is_same_v<T, Assignment>)
        {
            std::string reg = gen_expr (*node.value);
            int offset = var_offsets_.at (node.name);
            if (!reg.empty ())
            {
                emit ("    mov DWORD PTR [rbp +" + std::to_string (offset) +
                           "], " + reg32 (reg));
                free_reg (reg);
            }
            else
            {
                emit ("    pop rax");
                emit ("    mov DWORD PTR [rbp +" + std::to_string (offset)
                           + "], eax");
            }
        }

        // If statement
        else if constexpr (std::is_same_v<T, IfStmt>)
        {
            std::string else_label = ".L" + std::to_string (label_counter_++);
            std::string end_label  = ".L" + std::to_string (label_counter_++);

            std::string reg = gen_expr (*node.condition);
            if (!reg.empty ())
            {
                emit ("    test " + reg32 (reg) + ", " + reg32 (reg));
                free_reg (reg);
            }
            else
            {
                emit ("    pop rax");
                emit ("    test eax, eax");
            }
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
            std::string end_label  = ".L" + std::to_string (label_counter_++);

            emit (loop_label + ":");
            std::string reg = gen_expr (*node.condition);
            if (!reg.empty ())
            {
                emit ("    test " + reg32 (reg) + ", " + reg32 (reg));
                free_reg (reg);
            }
            else
            {
                emit ("    pop rax");
                emit ("    test eax, eax");
            }
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

        // Expression statement (result discarded)
        else if constexpr (std::is_same_v<T, ExprStmt>)
        {
            std::string reg = gen_expr (*node.expression);
            if (reg.empty ())
                emit ("    pop rax");
            else
                free_reg (reg);
        }

        // Default
        else
        {
            throw GenError ("Unsupported statement type");
        }
    }, stmt.node);
}

/**
 * Evaluate expr
 * Return the reg holding the result or empty if all registers were busy (val
 * pushed to stack)
 */
std::string Codegen::gen_expr (const Expr& expr)
{
    return std::visit ([this] (const auto& node) -> std::string
    {
        using T = std::decay_t<decltype (node)>;

        // Int literal
        if constexpr (std::is_same_v<T, IntLiteral>)
        {
            std::string dest = alloc_reg ();
            if (dest.empty ())
                emit ("    push " + std::to_string (node.value));
            else
                emit ("    mov " + reg32 (dest) + ", " + std::to_string (node.value));
            return dest;
        }

        // Identifier
        else if constexpr (std::is_same_v<T, Identifier>)
        {
            int offset = var_offsets_.at (node.name);
            std::string dest = alloc_reg ();
            if (dest.empty ())
            {
                emit ("    mov eax, DWORD PTR [rbp +" + std::to_string (offset) + "]");
                emit ("    push rax");
            }
            else
                emit ("    mov " + reg32 (dest) + ", DWORD PTR [rbp +" +
                      std::to_string (offset) + "]");
            return dest;
        }

        // Unary op
        else if constexpr (std::is_same_v<T, UnaryOp>)
        {
            std::string operand_reg = gen_expr (*node.operand);
            if (!operand_reg.empty ())
            {
                // Operate in place on the scratch register
                std::string r  = reg32 (operand_reg);
                std::string r8 = reg8 (operand_reg);
                if (node.op == UnaryOp::Op::NEGATE)
                    emit ("    neg " + r);
                else if (node.op == UnaryOp::Op::NOT)
                {
                    emit ("    test " + r + ", " + r);
                    emit ("    sete " + r8);
                    emit ("    movzx " + r + ", " + r8);
                }
                return operand_reg;
            }

            // Spilled path: use stack
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
            return "";
        }

        // Function call
        else if constexpr (std::is_same_v<T, FuncCall>)
        {
            static const char* arg_regs_64[] = {"rdi", "rsi", "rdx",
                                                 "rcx", "r8",  "r9"};

            if (node.args.size () > 6)
                throw GenError ("Call to '" + node.name + "' has more than 6 arguments");

            // Evaluate each param and immediately free their regs
            for (auto& arg : node.args)
            {
                std::string arg_reg = gen_expr (*arg);
                if (!arg_reg.empty ())
                {
                    emit ("    push " + arg_reg);
                    free_reg (arg_reg);
                }
            }

            // Pop args into argument registers in reverse
            // Was having issues passing straight in with nested funcs?
            for (int i = static_cast<int> (node.args.size ()) - 1; i >= 0; --i)
                emit ("    pop " + std::string {arg_regs_64[i]});

            emit ("    call " + node.name);

            // Store the return value (rax/eax) into a scratch register or spill
            std::string dest = alloc_reg ();
            if (dest.empty ())
                emit ("    push rax");
            else
                emit ("    mov " + reg32 (dest) + ", eax");
            return dest;
        }

        // Binary op
        else if constexpr (std::is_same_v<T, BinaryOp>)
        {
            // Evaluate left, then right (right eval may use registers left holds)
            std::string left_reg  = gen_expr (*node.left);
            std::string right_reg = gen_expr (*node.right);

            // Use scratch registers directly wherever available.
            // Only fall back to eax/ecx when the value was spilled to the stack.
            // right is popped before left to respect push order (right was pushed last).
            if (right_reg.empty ())
                emit ("    pop rcx");
            std::string r_val = right_reg.empty () ? "ecx" : reg32 (right_reg);

            if (left_reg.empty ())
                emit ("    pop rax");
            std::string l_val = left_reg.empty () ? "eax" : reg32 (left_reg);

            std::string l8 = left_reg.empty () ? "al" : reg8 (left_reg);
            std::string r8 = right_reg.empty () ? "cl" : reg8 (right_reg);

            switch (node.op)
            {
                case BinaryOp::Op::ADD:
                    emit ("    add " + l_val + ", " + r_val);
                    break;
                case BinaryOp::Op::SUB:
                    emit ("    sub " + l_val + ", " + r_val);
                    break;
                case BinaryOp::Op::MUL:
                    emit ("    imul " + l_val + ", " + r_val);
                    break;
                case BinaryOp::Op::DIV:
                    // idiv requires the dividend in eax, bounce left in
                    if (l_val != "eax") emit ("    mov eax, " + l_val);
                    emit ("    cdq");
                    emit ("    idiv " + r_val);
                    if (l_val != "eax") emit ("    mov " + l_val + ", eax");
                    break;
                case BinaryOp::Op::EQ:
                    emit ("    cmp " + l_val + ", " + r_val);
                    emit ("    sete " + l8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
                case BinaryOp::Op::NE:
                    emit ("    cmp " + l_val + ", " + r_val);
                    emit ("    setne " + l8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
                case BinaryOp::Op::LT:
                    emit ("    cmp " + l_val + ", " + r_val);
                    emit ("    setl " + l8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
                case BinaryOp::Op::GT:
                    emit ("    cmp " + l_val + ", " + r_val);
                    emit ("    setg " + l8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
                case BinaryOp::Op::AND:
                    emit ("    test " + l_val + ", " + l_val);
                    emit ("    setne " + l8);
                    emit ("    test " + r_val + ", " + r_val);
                    emit ("    setne " + r8);
                    emit ("    and " + l8 + ", " + r8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
                case BinaryOp::Op::OR:
                    emit ("    or " + l_val + ", " + r_val);
                    emit ("    test " + l_val + ", " + l_val);
                    emit ("    setne " + l8);
                    emit ("    movzx " + l_val + ", " + l8);
                    break;
            }

            // Prefer ret in left_reg, then right_reg, then alloc/spill.
            // After the op, result is in l_val (left_reg's register or eax).
            if (!left_reg.empty ())
            {
                // Result already sits in left_reg.
                free_reg (right_reg);
                return left_reg;
            }
            if (!right_reg.empty ())
            {
                // Left was spilled so result is in eax. Reuse right_reg.
                emit ("    mov " + r_val + ", eax");
                return right_reg;
            }
            // Both were spilled. Result in eax.
            std::string dest = alloc_reg ();
            if (dest.empty ())
                emit ("    push rax");
            else
                emit ("    mov " + reg32 (dest) + ", eax");
            return dest;
        }

        // Default
        else
        {
            throw GenError ("Unsupported expression type");
            return "";
        }

    }, expr.node);
}

std::string Codegen::get_assembly () const
{
    std::string assembly_str {};

    assembly_str.append (".intel_syntax noprefix\n.global main\n\n");

    for (size_t line_num = 0; line_num < assembly_lines_.size (); ++line_num)
    {
        assembly_str.append (assembly_lines_[line_num]);
        if (line_num < assembly_lines_.size () - 1)
            assembly_str.append ("\n");
    }

    return assembly_str;
}
