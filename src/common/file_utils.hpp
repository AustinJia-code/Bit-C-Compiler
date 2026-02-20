/**
 * @file file_utils.hpp
 * @brief File utility functions
 */

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

inline const char* root = "/home/osten/Projects/Compiler/";

/**
 * Get global path with extension of root
 */
inline std::string get_full_path (const std::string& extension)
{
    return std::string {root} + extension;
}

/**
 * Read file contents into a string
 * Returns empty string on failure
 * 
 * Assumes path starting with '/' is global, else appends to project root
 */
inline std::string file_to_string (const std::string& path)
{
    if (path.size () < 1)
        return "";

    std::string file_path = path[0] == '/' ? path : get_full_path (path);

    std::ifstream file (file_path);

    if (!file.is_open ())
    {
        std::cerr << "Error: could not open " << path << std::endl;
        return {};
    }

    return {std::istreambuf_iterator<char> {file},
            std::istreambuf_iterator<char> {}};
}

/**
 * Overwrites file with string
 * 
 * Assumes path starting with '/' is global, else appends to project root
 */
inline void string_to_file (const std::string& str, const std::string& path)
{
    std::string file_path = path[0] == '/' ? path : get_full_path (path);

    std::ofstream file (file_path);

    if (!file.is_open ())
    {
        std::cerr << "Error: could not open " << path << std::endl;
        return;
    }

    file << str;
}