/**
 * @file /compiler/main.cpp
 * @brief Compiler entry point.
 */

#include <cstdlib>
#include <iostream>
#include <array>
#include <optional>
#include <string.h>
#include <cstring>
#include "lexer.hpp"
#include "parser.hpp"

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

    // Get tokens
    Lexer lexer (std::string {args->in_path.data ()});
    auto tokens = lexer.get_tokens ();

    // Parse tokens
    try
    {
        Parser parser {tokens};
        Program program = parser.parse ();
        std::cout << "Parsing successful: "
                  << program.functions.size () << " function(s)" << std::endl;
    }
    catch (const ParseError& e)
    {
        std::cerr << "Parse error [" << e.loc.line << ":"
                  << e.loc.col << "]: " << e.what () << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}