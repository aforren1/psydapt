#include <iostream>
#include <array>
#include <tuple>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xstrided_view.hpp"
int main()
{
    xt::xarray<double> arr1{{{1.0, 2.0, 3.0},
                             {2.0, 5.0, 7.0},
                             {2.0, 5.0, 8.0}}};

    std::array<std::size_t, 2> foo{0, 2};
    auto bah = std::apply([arr1](auto &&...xs) { return xt::view(arr1, xs...); }, foo);
    // auto bum = xt::view(arr1, 0, 0, 1);

    // auto bar = xt::view(arr1, 0, xt::all());
    // bar = xt::view(bar, 0, xt::all());
    // bar = xt::view(bar, 1, xt::all());

    // xt::xstrided_slice_vector sv({0, 1});

    // auto baz = xt::strided_view(bar, sv);

    std::cout << bah << std::endl;
    // std::cout << bum << std::endl;
    // std::cout << xt::view(bar, 0) << std::endl;
    // std::cout << baz << std::endl;
}