#include "xtensor/xtensor.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xrandom.hpp"
#include <array>
#include <chrono>
#include <iostream>

// numpy is 100us
// matching shapes is 500us
// naive is 2-5ms
using namespace std::chrono;

int main()
{
    constexpr unsigned int reps = 100;
    std::array<std::size_t, 8> shp1 = {2, 10, 5, 4, 2, 5, 5, 8};
    std::array<std::size_t, 6> shp2 = {5, 4, 2, 5, 5, 8};

    xt::xtensor<double, 8> f1 = xt::xtensor<double, 8>::from_shape(shp1);
    f1 = xt::eval(xt::random::randn<double>(shp1));
    xt::xtensor<double, 6> f2 = xt::xtensor<double, 6>::from_shape(shp2);
    f2 = xt::eval(xt::random::randn<double>(shp2));

    xt::xtensor<double, 8> f3 = xt::xtensor<double, 8>::from_shape(shp1);
    xt::xtensor<double, 8> f4 = xt::xtensor<double, 8>::from_shape(shp1);

    //xt::xtensor<double, 6> slc;
    auto shp = f1.shape();
    auto start = high_resolution_clock::now();
    for (auto n = 0; n < reps; n++)
    {
        for (auto i = 0; i < shp[0]; i++)
        {
            for (auto j = 0; j < shp[1]; j++)
            {
                //auto slc = xt::view(f1, i, j);
                //auto out = xt::view(f3, i, j);
                xt::view(f3, i, j) = xt::view(f1, i, j) * f2;
            }
        }
    }
    auto stop = high_resolution_clock::now();

    auto start2 = high_resolution_clock::now();
    for (auto n = 0; n < reps; n++)
    {
        f4 = xt::eval(f1 * f2);
    }
    auto stop2 = high_resolution_clock::now();

    std::cout << (f3 == f4) << std::endl;

    std::cout << "new: " << (duration_cast<microseconds>(stop - start).count() / reps) << std::endl;
    std::cout << "old: " << (duration_cast<microseconds>(stop2 - start2).count() / reps) << std::endl;
}