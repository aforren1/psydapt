#include <Corrade/TestSuite/Tester.h>
#include "Corrade/TestSuite/Compare/Container.h"

#include "xtensor/xtensor.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xrandom.hpp"
#include <array>

using namespace Corrade;

struct TestBroadcast : TestSuite::Tester
{
    explicit TestBroadcast();

    void naive();
    void less_naive();
    void matching_shape();
};

TestBroadcast::TestBroadcast()
{
    addBenchmarks({&TestBroadcast::naive,
                   &TestBroadcast::less_naive,
                   &TestBroadcast::matching_shape},
                  100);
}

void TestBroadcast::naive()
{
    std::array<std::size_t, 8> shp1 = {2, 10, 5, 4, 2, 5, 5, 8};
    std::array<std::size_t, 6> shp2 = {5, 4, 2, 5, 5, 8};

    xt::xtensor<double, 8> f1 = xt::xtensor<double, 8>::from_shape(shp1);
    f1 = xt::eval(xt::random::randn<double>(shp1));
    xt::xtensor<double, 6> f2 = xt::xtensor<double, 6>::from_shape(shp2);
    f2 = xt::eval(xt::random::randn<double>(shp2));

    xt::xtensor<double, 8> f3 = xt::xtensor<double, 8>::from_shape(shp1);
    int a{};
    CORRADE_BENCHMARK(100)
    {
        f3 = xt::eval(f1 * f2);
        a++;
    }
    CORRADE_VERIFY(a);
}

void TestBroadcast::less_naive()
{
    std::array<std::size_t, 8> shp1 = {2, 10, 5, 4, 2, 5, 5, 8};
    std::array<std::size_t, 6> shp2 = {5, 4, 2, 5, 5, 8};

    xt::xtensor<double, 8> f1 = xt::xtensor<double, 8>::from_shape(shp1);
    f1 = xt::eval(xt::random::randn<double>(shp1));
    xt::xtensor<double, 6> f2 = xt::xtensor<double, 6>::from_shape(shp2);
    f2 = xt::eval(xt::random::randn<double>(shp2));

    xt::xtensor<double, 8> f3 = xt::xtensor<double, 8>::from_shape(shp1);

    auto shp = f1.shape();
    int a{};
    CORRADE_BENCHMARK(100)
    {
        for (auto i = 0; i < shp[0]; i++)
        {
            for (auto j = 0; j < shp[1]; j++)
            {
                //auto slc = xt::view(f1, i, j);
                //auto out = xt::view(f3, i, j);
                xt::view(f3, i, j) = xt::view(f1, i, j) * f2;
                a++;
            }
        }
    }
    CORRADE_VERIFY(a);
}

void TestBroadcast::matching_shape()
{
    std::array<std::size_t, 8> shp1 = {2, 10, 5, 4, 2, 5, 5, 8};
    xt::xtensor<double, 8> f1 = xt::xtensor<double, 8>::from_shape(shp1);
    f1 = xt::eval(xt::random::randn<double>(shp1));
    xt::xtensor<double, 8> f2 = xt::xtensor<double, 8>::from_shape(shp1);
    f2 = xt::eval(xt::random::randn<double>(shp1));

    xt::xtensor<double, 8> f3 = xt::xtensor<double, 8>::from_shape(shp1);
    int a{};
    CORRADE_BENCHMARK(100)
    {
        f3 = xt::eval(f1 * f2);
        a++;
    }
    CORRADE_VERIFY(a);
}

CORRADE_TEST_MAIN(TestBroadcast)
