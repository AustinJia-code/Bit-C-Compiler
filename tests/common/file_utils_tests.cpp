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
TestResult fts_bad_path ()
{
    std::string result = file_to_string ("bad/path");

    bool pass = result.empty ();

    return TestResult {.name = "fts bad path",
                       .pass = pass};
}

/**
 * file_to_string: reads a known file correctly
 */
TestResult fts_valid_file ()
{
    std::string result = file_to_string ("/examples/txt/statement.txt");

    bool pass = result == "int x = 5;";

    return TestResult {.name = "fts valid file",
                       .pass = pass};
}

/**
 * file_to_string: reads file with newlines
 */
TestResult fts_multiline ()
{
    std::string result = file_to_string ("/examples/txt/sentence.txt");

    bool pass = result == "This is a \nnew line.";

    return TestResult {.name = "fts multiline file",
                       .pass = pass};
}

/**
 * file_to_string: nonexistent nested path returns empty
 */
TestResult fts_nested_bad_path ()
{
    std::string result = file_to_string ("/no/such/dir/txt/file.txt");

    bool pass = result.empty ();

    return TestResult {.name = "fts nested bad path",
                       .pass = pass};
}

/**
 * string_to_file: basic functionality
 */
TestResult stf_basic ()
{
    std::string str {"This is just a test..."};

    string_to_file (str, out_path);
    std::cout << file_to_string ("out/test.txt") << std::endl;
    std::cout << str << std::endl;

    bool pass = (file_to_string ("out/test.txt") == str);

    return TestResult {.name = "stf basic functionality",
                       .pass = pass};
}

/**
 * string_to_file: tabs and newlines
 */
TestResult stf_spaces ()
{
    std::string str {"This \nis\t\t just a test..."};

    string_to_file (str, out_path);
    std::cout << file_to_string ("out/test.txt") << std::endl;
    std::cout << str << std::endl;

    bool pass = (file_to_string ("out/test.txt") == str);

    return TestResult {.name = "stf spaces",
                       .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_family ("file_to_string",
    {
        fts_bad_path,
        fts_valid_file,
        fts_multiline,
        fts_nested_bad_path
    });

    tb.add_family ("string_to_file",
    {
        stf_basic,
        stf_spaces
    }, {"file_to_string"});

    // TODO: Add global file path tests

    tb.run_tests ();
    tb.print_results ();
}
