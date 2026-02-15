/**
 * @file test_result.h
 * @brief Helper for tests
 */

#include <string>
#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_set>
#include <unordered_map>

/**
 * Result struct
 */
struct TestResult
{
    std::string name;
    bool pass;
};

/**
 * Overload TestResult stream insertion
 */
std::ostream& operator << (std::ostream& os, const TestResult& obj)
{
    const char* status_str = obj.pass ? "\033[32mPASS" :
                                        "\033[31mFAIL";
    os << status_str << " --- " << obj.name << "\033[0m";

    return os;
}

/**
 * A named group of tests with optional dependencies on other families
 */
struct TestFamily
{
    std::string name;
    std::vector <std::function<TestResult ()>> tests {};
    std::vector <std::string> depends_on {};
    std::vector <TestResult> results {};
    bool all_passed = true;
};

/**
 * Unified test harness
 */
class Testbench
{
private:
    std::vector <TestFamily> families {};

    /**
     * Find family index by name, or -1
     */
    int find_family (const std::string& name) const
    {
        for (int i = 0; i < families.size (); ++i)
            if (families[i].name == name)
                return i;
        return -1;
    }

public:
    /**
     * Add a test to the default (unnamed) family
     */
    void add_test (std::function<TestResult ()> test)
    {
        int idx = find_family ("");
        if (idx < 0)
        {
            families.push_back (TestFamily {.name = ""});
            idx = families.size () - 1;
        }
        families[idx].tests.push_back (test);
    }

    /**
     * Add a named test family with optional dependencies
     */
    void add_family (const std::string& name,
                     std::vector <std::function<TestResult ()>> tests,
                     std::vector <std::string> depends_on = {})
    {
        families.push_back (TestFamily {
            .name       = name,
            .tests      = std::move (tests),
            .depends_on = std::move (depends_on),
        });
    }

    /**
     * Validate dependency graph before running tests.
     * Warns about missing dependencies and circular dependencies.
     */
    void validate_dependencies () const
    {
        std::unordered_set <std::string> known;
        for (const auto& family : families)
            if (!family.name.empty ())
                known.insert (family.name);

        // Check for missing dependencies
        for (const auto& family : families)
            for (const auto& dep : family.depends_on)
                if (!known.count (dep))
                    std::cerr << "\033[33mWARN --- family \""
                              << family.name
                              << "\" depends on unknown family \""
                              << dep << "\"\033[0m" << std::endl;

        // Check for circular dependencies via DFS
        // Build adjacency: family -> its dependencies
        std::unordered_map <std::string, std::vector <std::string>> adj;
        for (const auto& family : families)
            if (!family.name.empty ())
                adj[family.name] = family.depends_on;

        // 0 = unvisited, 1 = in-stack, 2 = done
        std::unordered_map <std::string, int> state;
        std::vector <std::string> path;

        std::function <bool (const std::string&)> has_cycle =
            [&] (const std::string& node) -> bool
        {
            state[node] = 1;
            path.push_back (node);

            for (const auto& dep : adj[node])
            {
                if (!known.count (dep)) continue;
                if (state[dep] == 1)
                {
                    // Found cycle — build the cycle string
                    std::string cycle;
                    auto it = std::find (path.begin (), path.end (), dep);
                    for (; it != path.end (); ++it)
                    {
                        if (!cycle.empty ()) cycle += " -> ";
                        cycle += "\"" + *it + "\"";
                    }
                    cycle += " -> \"" + dep + "\"";
                    std::cerr << "\033[33mWARN --- circular dependency: "
                              << cycle << "\033[0m" << std::endl;
                    path.pop_back ();
                    return true;
                }
                if (state[dep] == 0 && has_cycle (dep))
                {
                    path.pop_back ();
                    return true;
                }
            }

            path.pop_back ();
            state[node] = 2;
            return false;
        };

        for (const auto& name : known)
            if (state[name] == 0)
                has_cycle (name);
    }

    /**
     * Run all families in order, checking dependencies
     */
    void run_tests ()
    {
        validate_dependencies ();
        std::unordered_set <std::string> failed_families {};

        for (auto& family : families)
        {
            family.results.clear ();
            family.all_passed = true;

            // Check dependencies
            std::vector <std::string> failed_deps {};
            for (const auto& dep : family.depends_on)
                if (failed_families.count (dep))
                    failed_deps.push_back (dep);

            if (!failed_deps.empty ())
            {
                std::cerr << "\033[33mWARN --- family \""
                          << family.name
                          << "\" depends on failed: ";
                for (size_t i = 0; i < failed_deps.size (); ++i)
                {
                    if (i > 0) std::cerr << ", ";
                    std::cerr << "\"" << failed_deps[i] << "\"";
                }
                std::cerr << "\033[0m" << std::endl;
            }

            // Run tests
            for (auto& test : family.tests)
            {
                auto result = test ();
                if (!result.pass)
                    family.all_passed = false;
                family.results.push_back (result);
            }

            if (!family.all_passed && !family.name.empty ())
                failed_families.insert (family.name);
        }
    }

    /**
     * Print results grouped by family
     */
    void print_results ()
    {
        for (size_t f = 0; f < families.size (); ++f)
        {
            const auto& family = families[f];

            if (!family.name.empty ())
            {
                if (f > 0)
                    std::cout << "\n";

                std::cout << "--- " << family.name << " ---"
                          << std::endl;
            }

            for (size_t i = 0; i < family.results.size (); ++i)
            {
                std::cout << family.results[i];
                if (i < family.results.size () - 1)
                    std::cout << std::endl;
            }

            if (f < families.size () - 1)
                std::cout << std::endl;
        }
    }
};
