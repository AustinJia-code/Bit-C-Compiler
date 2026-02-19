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
#include "codegen.hpp"
#include "optimizer.hpp"
#include "file_utils.hpp"

static constexpr size_t MAX_PATH_LEN = 128;

/**
 * Input args
 */
struct Args
{
    std::array<char, MAX_PATH_LEN> in_path;
    std::array<char, MAX_PATH_LEN> out_path;
    bool optimize = false;
};

/**
 * Parses args and writes to std::cerr if failure
 * @return Input args if valid, else nullopt
 */
std::optional<Args> parse_args (int argc, char* argv[])
{
    // Check arg count: required 4, optional -O flag makes 5
    if (argc < 4 || argc > 5)
    {
        std::cerr << "Usage: ./compiler <in_path> -o <out_path> [-O]" << std::endl;
        return std::nullopt;
    }

    // Check file path len
    std::string in_path {argv[1]};
    if (in_path.length () > MAX_PATH_LEN - 1)
    {
        std::cerr << "File path cannot exceed " << MAX_PATH_LEN << " chars"
                  << std::endl;
        return std::nullopt;
    }

    std::string out_path {argv[3]};
    if (out_path.length () > MAX_PATH_LEN - 1)
    {
        std::cerr << "File path cannot exceed " << MAX_PATH_LEN << " chars"
                  << std::endl;
        return std::nullopt;
    }

    // Return
    Args ret {};
    memcpy (ret.in_path.data (), in_path.c_str (), in_path.size () + 1);
    memcpy (ret.out_path.data (), out_path.c_str (), out_path.size () + 1);

    if (argc == 5)
    {
        if (std::string {argv[4]} == "-O")
            ret.optimize = true;
        else
        {
            std::cerr << "Unknown flag: " << argv[4] << std::endl;
            return std::nullopt;
        }
    }

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
    Program program;
    try
    {
        Parser parser {tokens};
        program = parser.parse ();
        std::cout << "Parsing successful: "
                  << program.functions.size () << " function(s)" << std::endl;
    }
    catch (const ParseError& e)
    {
        std::cerr << "Parse error [" << e.loc.line << ":"
                  << e.loc.col << "]: " << e.what () << std::endl;
        return EXIT_FAILURE;
    }

    // Optimize (optional, enabled with -O)
    if (args->optimize)
    {
        Optimizer optimizer;
        optimizer.optimize (program);
        std::cout << "Optimization applied" << std::endl;
    }

    // Generate assembly
    try
    {
        Codegen codegen {program};
        string_to_file (codegen.get_assembly (), args->out_path.data ());
    }
    catch(const std::exception& e)
    {
        std::cerr << "Codegen error: " << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}