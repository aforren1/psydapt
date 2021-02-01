#include <iostream>
#include <vector>

#include "psydapt.hpp"

int main()
{
    using namespace psydapt::questplus;
    Weibull::Params p;
    p.intensity = {-3.5, -3.25, -3., -2.75, -2.5, -2.25, -2., -1.75, -1.5,
                   -1.25, -1., -0.75, -0.5};
    p.threshold = p.intensity;
    p.slope = {0.5, 4.125, 7.75, 11.375, 15.};
    p.lower_asymptote = {0.01, 0.1325, 0.255, 0.3775, 0.5};
    p.lapse_rate = {0.01};
    p.stim_scale = psydapt::Scale::Log10;

    Weibull weibull{p};
    bool cont = true;
    while (cont)
    {
        std::cout << "Stim val: " << weibull.next() << std::endl;
        std::cout << "Correct? ";
        int corr;
        std::cin >> corr;
        cont = weibull.update(corr);
    }
    return 0;
}