#include <vector>
#include <iostream>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xadapt.hpp"
int main(int argc, char *argv[])
{
    std::vector<double> foo{1, 2, 3};
    xt::xtensor<double, 3> arr1 = foo;
    arr1.reshape({-1, 1, 1});

    xt::xtensor<double, 3> arr2 = xt::zeros<double>({1, 3, 1});

    xt::xtensor<double, 3> res = arr1 * arr2;

    std::cout << xt::adapt(res.shape()) << std::endl;

    return 0;
}
