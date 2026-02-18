/**
 * @file file_utils_tests.cpp
 * @brief Tests for file_utils helper functions
 */

#include <iostream>
#include <testbench.hpp>
#include <file_utils.hpp>
#include <string>

inline const char* out_path = "out/test.txt";

/**
 * file_to_string: bad path returns empty string
 */
bool fts_bad_path ()
{
    std::string result = file_to_string ("bad/path");

    return result.empty ();
}

/**
 * file_to_string: reads a known file correctly
 */
bool fts_valid_file ()
{
    std::string result = file_to_string ("examples/txt/statement.txt");

    return result == "int x = 5;";
}

/**
 * file_to_string: reads file with newlines
 */
bool fts_multiline ()
{
    std::string result = file_to_string ("examples/txt/sentence.txt");

    return result == "This is a \nnew line.";
}

/**
 * file_to_string: nonexistent nested path returns empty
 */
bool fts_nested_bad_path ()
{
    std::string result = file_to_string ("/no/such/dir/txt/file.txt");

    return result.empty ();
}

/**
 * string_to_file: basic functionality
 */
bool stf_basic ()
{
    std::string str {"This is just a test..."};

    string_to_file (str, out_path);
    std::cout << file_to_string ("out/test.txt") << std::endl;
    std::cout << str << std::endl;

    return (file_to_string ("out/test.txt") == str);
}

/**
 * string_to_file: tabs and newlines
 */
bool stf_spaces ()
{
    std::string str {"This \nis\t\t just a test..."};

    string_to_file (str, out_path);
    std::cout << file_to_string ("out/test.txt") << std::endl;
    std::cout << str << std::endl;

    return (file_to_string ("out/test.txt") == str);
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("file_to_string",
    {
        {fts_bad_path,          "fts bad path"},
        {fts_valid_file,        "fts valid file"},
        {fts_multiline,         "fts multiline file"},
        {fts_nested_bad_path,   "fts nested bad path"},
    });

    tb.add_family ("string_to_file",
    {
        {stf_basic,             "stf basic functionality"},
        {stf_spaces,            "stf spaces"},
    }, {"file_to_string"});

    // TODO: Add global file path tests

    tb.run_tests ();
    tb.print_results ();
}
