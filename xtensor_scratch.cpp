#include <vector>
#include <iostream>
#include <optional>
#include <utility>

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xadapt.hpp"

#include "psydapt.hpp"

int main(int argc, char *argv[])
{
    std::vector<double> foo{1, 2, 3};
    auto arr1 = xt::adapt(foo, std::vector<std::size_t>{foo.size()});
    arr1.reshape({-1, 1, 1});

    xt::xtensor<double, 3> arr2 = xt::zeros<double>({1, 3, 1});

    xt::xtensor<double, 3> res = arr1 * arr2;

    std::cout << arr1 << std::endl;

    std::vector<double> intensity{1, 2, 3, 4};
    std::vector<double> threshold{1, 2, 3, 4};
    std::vector<double> slope{3.5};
    std::vector<double> lower_asymptote{0.01};
    std::vector<double> lapse_rate{0.01};

    using shape_t = xt::xtensor<double, 2>::shape_type;
    xt::xtensor<double, 2> fob;
    fob = xt::adapt(intensity, shape_t{4, 1});
    xt::xtensor<double, 2> bar(shape_t{1, 4}, 2.0);

    std::cout << fob * xt::sum(bar) << std::endl;

    //

    using namespace psydapt;
    questplus::Weibull::Params p;
    p.intensity = intensity;
    p.threshold = threshold;
    p.slope = slope;
    p.lower_asymptote = lower_asymptote;
    p.lapse_rate = lapse_rate;

    questplus::Weibull weibull{p};

    // std::cout << weibull.generate_likelihoods();

    // xt::xarray<double> arr3{{1.0, 2.0, 3.0},
    //                         {2.0, 5.0, 7.0},
    //                         {2.0, 5.0, 7.0}};

    // std::array<std::size_t, 2> fooo{1, 2};
    // auto slc = [](auto input) { return xt::view(foob, input); };
    // auto bah = std::apply(slc, &arr3, fooo);
    // // std::cout << bah;

    return 0;
}
