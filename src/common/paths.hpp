/**
 * @file paths.hpp
 * @brief Global paths
 */

#pragma once

#include <string>

const std::string root = "/home/osten/Projects/Compiler/";

/**
 * Get global path with extension of root
 */
std::string get_full_path (const std::string& extension)
{
    return std::string (root) + extension;
}