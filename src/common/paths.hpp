/**
 * @file paths.hpp
 * @brief Global paths
 */

#pragma once

#include <string>

inline const char* root = "/home/osten/Projects/Compiler/";

/**
 * Get global path with extension of root
 */
inline std::string get_full_path (const std::string& extension)
{
    return std::string {root} + extension;
}