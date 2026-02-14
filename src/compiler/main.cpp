/**
 * @file /compiler/main.cpp
 * @brief Compiler entry point.
 */

#include <cstdlib>
#include <iostream>
#include <array>
#include <optional>
#include <string.h>

static constexpr size_t MAX_PATH_LEN = 64;

/**
 * Input args
 */
struct Args
{
    std::array<char, MAX_PATH_LEN> in_path;
};

/**
 * Parses args and writes to std::cerr if failure
 * @return Input args if valid, else nullopt
 */
std::optional<Args> parse_args (int argc, char* argv[])
{
    // Check arg count
    if (argc != 2)
    {
        std::cerr << "Usage: ./compiler <file>" << std::endl;
        return std::nullopt;
    }

    // Check file path len
    std::string fp {argv[1]};
    if (fp.length () > MAX_PATH_LEN - 1)
    {
        std::cerr << "File path cannot exceed " << MAX_PATH_LEN << "chars"
                  << std::endl;
        return std::nullopt;
    }

    // Return
    Args ret {};
    memcpy (ret.in_path.data (), fp.c_str (), fp.size () + 1);

    return ret;
}

/**
 * Runner
 */
int main (int argc, char* argv[])
{
    std::optional<Args> args = parse_args (argc, argv);
    if (!args)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}