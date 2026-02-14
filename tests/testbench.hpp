/**
 * @file test_result.h
 * @brief Helper for tests
 */

#include <string>
#include <iostream>
#include <ostream>
#include <vector>
#include <functional>

/**
 * Result struct
 */
struct TestResult
{
    std::string name;
    bool pass;
};

/**
 * Overload in
 */
std::ostream& operator << (std::ostream& os, const TestResult& obj)
{
    std::string status_str = obj.pass ? "\033[32mPASS" :
                                        "\033[31mFAIL";
    os << status_str << " --- " << obj.name << "\033[0m";
    
    return os;
}

/**
 * Unified test harness
 */
class Testbench
{
private:
    std::vector <std::function<TestResult ()>> tests {};
    std::vector <TestResult> results {};

public:
    /**
     * Add new test function
     */
    void add_test (std::function<TestResult ()> test)
    {
        tests.push_back (test);
    }

    /**
     * Run all tests
     */
    void run_tests ()
    {
        results.clear ();
        for (auto test : tests)
            results.push_back (test ());
    }

    /**
     * Print results
     */
    void print_results ()
    {
        for (int i = 0; i < results.size () ; ++i)
        {
            std::cout << results[i];
            if (i < results.size () - 1)
                 std::cout << std::endl;
        }
    }
};