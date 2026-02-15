/**
 * @file file_utils_tests.cpp
 * @brief Tests for file_utils helper functions
 */

#include <iostream>
#include <testbench.hpp>
#include <file_utils.hpp>
#include <string>

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
    std::string result = file_to_string ("/examples/statement.txt");

    bool pass = result == "int x = 5;";

    return TestResult {.name = "fts valid file",
                       .pass = pass};
}

/**
 * file_to_string: reads file with newlines
 */
TestResult fts_multiline ()
{
    std::string result = file_to_string ("/examples/sentence.txt");

    bool pass = result == "This is a \nnew line.";

    return TestResult {.name = "fts multiline file",
                       .pass = pass};
}

/**
 * file_to_string: nonexistent nested path returns empty
 */
TestResult fts_nested_bad_path ()
{
    std::string result = file_to_string ("/no/such/dir/file.txt");

    bool pass = result.empty ();

    return TestResult {.name = "fts nested bad path",
                       .pass = pass};
}

/**
 * Entry
 */
int main ()
{
    Testbench tb {};

    tb.add_test (fts_bad_path);
    tb.add_test (fts_valid_file);
    tb.add_test (fts_multiline);
    tb.add_test (fts_nested_bad_path);

    std::cout << "===== TEST OUTPUT =====" << std::endl;
    tb.run_tests ();

    std::cout << "\n======= RESULTS =======" << std::endl;
    tb.print_results ();
    std::cout << "\n=======================" << std::endl;
}
