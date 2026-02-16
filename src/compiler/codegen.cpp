/**
 * @file codegen.hpp
 * @brief Codegen implementation
 */

#include "codegen.hpp"
#include <iostream>
#include <string>

Codegen::Codegen (const Program& prog)
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

    // TODO: 
    if (!found_main)
        throw GenError ("No entry found");
}

std::string Codegen::get_assembly () const
{
    std::string assembly_str {};

    for (size_t line_num = 0; line_num < assembly_lines_.size (); ++line_num)
    {
        assembly_str.append (assembly_lines_[line_num]);
        if (line_num < assembly_lines_.size () - 1)
            assembly_str.append ("\n");
    }

    return assembly_str;
}