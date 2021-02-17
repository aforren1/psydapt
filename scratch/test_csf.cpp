#include <iostream>
#include <vector>

#include "psydapt.hpp"

int main()
{
    using namespace psydapt::questplus;
    CSF::Params p;
    p.contrast = {-50, -48, -46, -44, -42, -40, -38, -36, -34, -32, -30, -28, -26,
                  -24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -4, -2, 0};
    p.spatial_freq = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
                      34, 36, 38, 40};
    p.temporal_freq = {0};

    p.min_thresh = {-50, -48, -46, -44, -42, -40, -38, -36, -34, -32, -30};
    p.c0 = {-60, -58, -56, -54, -52, -50, -48, -46, -44, -42, -40};
    p.cf = {0.8, 1., 1.2, 1.4, 1.6};
    p.cw = {0};
    p.slope = {3};
    p.lower_asymptote = {0.5};
    p.lapse_rate = {0.01};

    p.stim_scale = psydapt::Scale::dB;

    CSF csf{p};

    std::vector<int> resps = {1, 0, 1, 1, 1,
                              1, 0, 1, 1, 1,
                              1, 1, 0, 1, 1,
                              1, 1, 1, 0, 0,
                              1, 1, 1, 1, 1,
                              1, 1, 0, 0, 1,
                              1, 1};

    for (int i = 0; i < resps.size(); i++)
    {
        auto n = csf.next();
        std::cout << "vals: [" << n[0] << ", " << n[1] << ", " << n[2];
        std::cout << "], resp: " << resps[i] << std::endl;
        csf.update(resps[i]);
    }
}