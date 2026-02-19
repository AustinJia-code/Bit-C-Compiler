/**
 * @file optimizer.hpp
 * @brief Optional AST optimization passes.
 */

#pragma once

#include "ast.hpp"
#include <optional>
#include <vector>

class Optimizer
{
public:
    void optimize (Program& program);

private:
    void opt_function (Function& func);
    void opt_block    (Block& block);

    // Returns replacement stmts: empty = remove, one/more = substitute
    std::vector<Stmt> opt_stmt (Stmt& stmt);

    // Folds constants in-place and returns constant value if whole expr is constant
    std::optional<int> fold_expr (std::unique_ptr<Expr>& expr);
};
