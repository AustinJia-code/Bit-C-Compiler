/**
 * @file file_utils.hpp
 * @brief File utility functions
 */

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include "paths.hpp"

/**
 * Read file contents into a string
 * Returns empty string on failure
 */
inline std::string file_to_string (const std::string& path)
{
    std::ifstream file (get_full_path (path));

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
 */
inline void string_to_file (const std::string& str, const std::string& path)
{
    std::ofstream file (get_full_path (path));

    if (!file.is_open ())
    {
        std::cerr << "Error: could not open " << path << std::endl;
        return;
    }

    file << str;
}