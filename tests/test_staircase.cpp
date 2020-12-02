#include <Corrade/TestSuite/Tester.h>
#include <vector>
#include "psydapt.hpp"

struct TestStaircase : Corrade::TestSuite::Tester
{
    explicit TestStaircase();

    void communtativity();
    void prepend1kItemsVector();
};

TestStaircase::TestStaircase()
{
    addTests({&TestStaircase::communtativity});
    addBenchmarks({&TestStaircase::prepend1kItemsVector}, 100);
}

void TestStaircase::communtativity()
{
    double a = 5.0;
    double b = 3.0;

    CORRADE_VERIFY(a * b == b * a);
    CORRADE_VERIFY(a / b == b / a);
}

void TestStaircase::prepend1kItemsVector()
{
    double a{};
    CORRADE_BENCHMARK(100)
    {
        std::vector<double> container;
        for (std::size_t i = 0; i != 1000; ++i)
            container.insert(container.begin(), 1.0);
        a += container.back();
    }
    CORRADE_VERIFY(a); // to avoid the benchmark loop being optimized out
}

CORRADE_TEST_MAIN(TestStaircase)
