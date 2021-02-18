#include <Corrade/TestSuite/Tester.h>
#include "Corrade/TestSuite/Compare/Container.h"
#include <vector>
#include "psydapt.hpp"

using namespace Corrade;

struct TestQPCSF : TestSuite::Tester
{
    explicit TestQPCSF();

    void correctness();
    void nextAndUpdate();
};

TestQPCSF::TestQPCSF()
{
    addTests({&TestQPCSF::correctness});
    addBenchmarks({&TestQPCSF::nextAndUpdate}, 10);
}

void TestQPCSF::correctness()
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

    std::vector<double> expected_contrasts{0, -4, 0, 0, -38, 0, -40, 0, -26, -26,
                                           0, -36, -36, 0, -26, -26, -2, -26, -6, -26,
                                           0, -26, 0, -26, -32, -32, -34, -34, 0, -26,
                                           0, -26};
    std::vector<double> expected_spat_freqs{40, 40, 34, 36, 0, 38, 0, 38, 18, 18, 40, 0,
                                            0, 40, 20, 20, 40, 22, 40, 20, 40, 18, 40, 18,
                                            0, 0, 0, 0, 40, 18, 38, 18};
    std::vector<double> pred_contrasts;
    std::vector<double> pred_spat_freqs;
    for (std::size_t i = 0; i < resps.size(); i++)
    {
        auto n = csf.next();
        pred_contrasts.push_back(n[0]);
        pred_spat_freqs.push_back(n[1]);
        csf.update(resps[i]);
    }
    CORRADE_COMPARE_AS(pred_contrasts, expected_contrasts, TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(pred_spat_freqs, expected_spat_freqs, TestSuite::Compare::Container);
}

void TestQPCSF::nextAndUpdate()
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

    double a{};
    CORRADE_BENCHMARK(10)
    {
        for (std::size_t i = 0; i < 10; i++)
        {
            a += csf.next()[0];
            csf.update(i % 2);
        }
    }
    CORRADE_VERIFY(a);
}

CORRADE_TEST_MAIN(TestQPCSF)
