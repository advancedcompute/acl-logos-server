
#include <iostream>
#include "runner.h"

int main()
{
    cpp::TestRunner testRunner;

    // TODO: Add test sets

    std::cout << "Running all unit tests...\n";

    testRunner.run_tests();

    print_test_results(testRunner.results());

    return 0;
}
