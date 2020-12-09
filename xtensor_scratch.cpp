#include <vector>
#include <iostream>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xadapt.hpp"

int main(int argc, char *argv[])
{
    std::vector<double> foo{1, 2, 3};
    auto arr1 = xt::adapt(foo, std::vector<std::size_t>{foo.size()});
    arr1.reshape({-1, 1, 1});

    xt::xtensor<double, 3> arr2 = xt::zeros<double>({1, 3, 1});

    xt::xtensor<double, 3> res = arr1 * arr2;

    std::cout << arr1 << std::endl;

    return 0;
}
