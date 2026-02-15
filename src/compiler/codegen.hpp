/**
 * @file codegen.hpp
 * @brief Emits assembly (.s).
 */

#pragma once

#include "ast.hpp"
#include <vector>
#include <string>

/**
 * Codegen
 */
class Codegen
{
private:
    size_t label_counter;
    size_t tab_counter;
    std::vector<std::string> assembly;

public:
    std::string gen (const Program& prog);
};