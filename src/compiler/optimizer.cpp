/**
 * @file optimizer.cpp
 * @brief Implementation of optional AST optimization passes.
 */

#include "optimizer.hpp"
#include <variant>

/**
 * Folds constant int expressions and cleans dead branches
 */
void Optimizer::optimize (Program& program)
{
    for (auto& func : program.functions)
        opt_function (func);
}

/********** PRIVATE HELPERS **********/
void Optimizer::opt_function (Function& func)
{
    opt_block (func.body);
}

void Optimizer::opt_block (Block& block)
{
    std::vector<Stmt> new_stmts;
    for (auto& stmt : block.statements)
    {
        auto replacements = opt_stmt (stmt);
        for (auto& s : replacements)
            new_stmts.push_back (std::move (s));
    }
    block.statements = std::move (new_stmts);
}

std::vector<Stmt> Optimizer::opt_stmt (Stmt& stmt)
{
    bool replaced = false;
    std::vector<Stmt> replacements;

    std::visit ([this, &replaced, &replacements] (auto& node)
    {
        using T = std::decay_t<decltype (node)>;

        if constexpr (std::is_same_v<T, VarDecl>)
        {
            if (node.init.has_value ())
                fold_expr (*node.init);
        }
        else if constexpr (std::is_same_v<T, Assignment>)
        {
            fold_expr (node.value);
        }
        else if constexpr (std::is_same_v<T, ReturnStmt>)
        {
            fold_expr (node.value);
        }
        else if constexpr (std::is_same_v<T, IfStmt>)
        {
            // Fold condition first, then recurse into the body
            auto val = fold_expr (node.condition);
            opt_block (*node.then_block);

            if (val.has_value ())
            {
                replaced = true;
                if (val.value () != 0)
                {
                    // If always true: inline the block
                    replacements.push_back (
                        Stmt {Block {std::move (node.then_block->statements)}});
                }
                // Always false: give no replacement
            }
        }
        else if constexpr (std::is_same_v<T, WhileStmt>)
        {
            fold_expr (node.condition);
            opt_block (*node.body);
        }
        else if constexpr (std::is_same_v<T, Block>)
        {
            opt_block (node);
        }
        else if constexpr (std::is_same_v<T, ExprStmt>)
        {
            fold_expr (node.expression);
        }
    }, stmt.node);

    if (replaced)
        return replacements;

    std::vector<Stmt> result;
    result.push_back (std::move (stmt));
    return result;
}

std::optional<int> Optimizer::fold_expr (std::unique_ptr<Expr>& expr)
{
    std::optional<int> result = std::visit ([this] (auto& node) -> std::optional<int>
    {
        using T = std::decay_t<decltype (node)>;

        if constexpr (std::is_same_v<T, IntLiteral>)
        {
            return node.value;
        }
        else if constexpr (std::is_same_v<T, Identifier>)
        {
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<T, FuncCall>)
        {
            for (auto& arg : node.args)
                fold_expr (arg);
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<T, UnaryOp>)
        {
            auto val = fold_expr (node.operand);
            if (!val) return std::nullopt;
            switch (node.op)
            {
                case UnaryOp::Op::NEGATE: return -val.value ();
                case UnaryOp::Op::NOT:    return val.value () != 0 ? 0 : 1;
            }
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<T, BinaryOp>)
        {
            auto lval = fold_expr (node.left);
            auto rval = fold_expr (node.right);
            if (!lval || !rval) return std::nullopt;
            int l = lval.value (), r = rval.value ();
            switch (node.op)
            {
                case BinaryOp::Op::ADD: return l + r;
                case BinaryOp::Op::SUB: return l - r;
                case BinaryOp::Op::MUL: return l * r;
                case BinaryOp::Op::DIV: return r != 0 ? std::optional<int> {l / r}
                                                       : std::nullopt;
                case BinaryOp::Op::EQ:  return l == r ? 1 : 0;
                case BinaryOp::Op::NE:  return l != r ? 1 : 0;
                case BinaryOp::Op::LT:  return l <  r ? 1 : 0;
                case BinaryOp::Op::GT:  return l >  r ? 1 : 0;
                case BinaryOp::Op::AND: return (l && r) ? 1 : 0;
                case BinaryOp::Op::OR:  return (l || r) ? 1 : 0;
            }
            return std::nullopt;
        }
        return std::nullopt;
    }, expr->node);

    // If value is foldable, fold it!
    if (result.has_value () && !std::holds_alternative<IntLiteral> (expr->node))
        expr = std::make_unique<Expr> (Expr {IntLiteral {result.value ()}});

    return result;
}
