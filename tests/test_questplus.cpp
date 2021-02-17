#include <Corrade/TestSuite/Tester.h>
#include "Corrade/TestSuite/Compare/Container.h"
#include <vector>
#include "psydapt.hpp"

using namespace Corrade;

struct TestQuestPlus : TestSuite::Tester
{
    explicit TestQuestPlus();

    void threshold();
    void nextAndUpdate();
};

TestQuestPlus::TestQuestPlus()
{
    addTests({&TestQuestPlus::threshold});
    addBenchmarks({&TestQuestPlus::nextAndUpdate}, 10);
}

// mostly https://github.com/hoechenberger/questplus/blob/main/questplus/tests/test_qp.py#L8
void TestQuestPlus::threshold()
{
    using namespace psydapt::questplus;
    Weibull::Params p;
    p.threshold = {-40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28,
                   -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15,
                   -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2,
                   -1, 0};
    p.intensity = p.threshold;
    p.slope = {3.5};
    p.lower_asymptote = {0.5};
    p.lapse_rate = {0.02};
    p.stim_scale = psydapt::Scale::dB;

    Weibull weibull{p};

    std::vector<double> expected_contrasts{-18, -22, -25, -28, -30, -22, -13, -15, -16, -18,
                                           -19, -20, -21, -22, -23, -19, -20, -20, -18, -18,
                                           -19, -17, -17, -18, -18, -18, -19, -19, -19, -19,
                                           -19, -19};
    std::vector<int> responses{1, 1, 1, 1, 0,
                               0, 1, 1, 1, 1,
                               1, 1, 1, 1, 0,
                               1, 1, 0, 1, 1,
                               0, 1, 1, 1, 1,
                               1, 1, 1, 1, 1,
                               1, 1};
    std::vector<double> pred_contrasts;
    for (std::size_t i = 0; i < responses.size(); i++)
    {
        pred_contrasts.push_back(weibull.next());
        weibull.update(responses[i]);
    }
    CORRADE_COMPARE_AS(pred_contrasts, expected_contrasts, TestSuite::Compare::Container);
}

void TestQuestPlus::nextAndUpdate()
{
    {
        using namespace psydapt::questplus;
        Weibull::Params p;
        p.threshold = {-40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28,
                       -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15,
                       -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2,
                       -1, 0};
        p.intensity = p.threshold;
        p.slope = {3.5};
        p.lower_asymptote = {0.5};
        p.lapse_rate = {0.02};
        p.stim_scale = psydapt::Scale::dB;

        Weibull weibull{p};

        double a{};
        CORRADE_BENCHMARK(10)
        {
            for (std::size_t i = 0; i < 100; i++)
            {
                a += weibull.next();
                weibull.update(i % 2);
            }
        }

        CORRADE_VERIFY(a);
    }
}
CORRADE_TEST_MAIN(TestQuestPlus)
