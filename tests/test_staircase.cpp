#include <Corrade/TestSuite/Tester.h>
#include "Corrade/TestSuite/Compare/Container.h"
#include <vector>
#include "psydapt.hpp"
#include "common.hpp"

using namespace Corrade;

struct TestStaircase : TestSuite::Tester
{
    explicit TestStaircase();

    void linear();
    void log();
    void nextAndUpdate();
};

TestStaircase::TestStaircase()
{
    addTests({&TestStaircase::linear, &TestStaircase::log});
    addBenchmarks({&TestStaircase::nextAndUpdate}, 100);
}

void TestStaircase::linear()
{
    using namespace psydapt;
    Staircase::StairParams params;
    params.n_trials = 20;
    params.start_val = 0.8;
    params.min_val = 0;
    params.max_val = 1;
    params.step_sizes = std::vector<double>{0.1, 0.01, 0.001};
    params.n_up = 1;
    params.n_down = 3;
    params.n_reversals = 4;

    params.apply_initial_rule = true;
    params.step_type = Scale::Linear;

    Staircase stare{params};

    std::vector<int> sim_resp = makeBasicResponseCycles(3, 4, 4, 20);

    bool cont = true;
    std::vector<double> pred_vals;
    std::vector<double> true_vals{0.8, 0.7, 0.6, 0.5, 0.4, 0.41, 0.42, 0.43, 0.44, 0.44, 0.44,
                                  0.439, 0.439, 0.44, 0.441, 0.442, 0.443, 0.443, 0.443, 0.442};
    int counter = 0;
    while (cont)
    {
        pred_vals.push_back(stare.next());
        cont = stare.update(sim_resp[counter]);
        // std::cout << pred_vals.end()[-1] << " " << sim_resp[counter] << "\n";
        counter++;
    }

    CORRADE_COMPARE_AS(pred_vals, true_vals, TestSuite::Compare::Container);
}

void TestStaircase::log()
{
    using namespace psydapt;
    Staircase::StairParams params;
    params.n_trials = 20;
    params.start_val = 0.8;
    params.min_val = 0;
    params.max_val = 1;
    params.step_sizes = std::vector<double>{0.4 / 20, 0.2 / 20, 0.2 / 20, 0.1 / 20};
    params.n_up = 1;
    params.n_down = 3;
    params.n_reversals = 4;

    params.apply_initial_rule = true;
    params.step_type = Scale::Log;

    Staircase stare{params};

    std::vector<int> sim_resp = makeBasicResponseCycles(3, 4, 4, 20);

    bool cont = true;
    // used floats b/c th
    std::vector<float> pred_vals;
    std::vector<float> true_vals{0.8, 0.763994069, 0.729608671, 0.696770872, 0.665411017,
                                 0.680910431, 0.696770872, 0.713000751, 0.729608671,
                                 0.729608671, 0.729608671, 0.713000751, 0.713000751,
                                 0.72125691, 0.729608671, 0.738057142, 0.746603441,
                                 0.746603441, 0.746603441, 0.738057142};
    int counter = 0;
    while (cont)
    {
        pred_vals.push_back(stare.next());
        cont = stare.update(sim_resp[counter]);
        // std::cout << pred_vals.end()[-1] << " " << sim_resp[counter] << "\n";
        counter++;
    }

    CORRADE_COMPARE_AS(pred_vals, true_vals, TestSuite::Compare::Container);
}

void TestStaircase::nextAndUpdate()
{
    using namespace psydapt;
    Staircase::StairParams params;
    params.n_trials = 1000;
    params.start_val = 0.8;
    params.min_val = 0;
    params.max_val = 1;
    params.step_sizes = std::vector<double>{0.1, 0.01, 0.001};
    params.n_up = 1;
    params.n_down = 3;
    params.n_reversals = 4;

    params.apply_initial_rule = true;
    params.step_type = Scale::Linear;

    Staircase stare{params};

    std::vector<int> sim_resp = makeBasicResponseCycles(30, 4, 4, 100);
    double a{};
    CORRADE_BENCHMARK(100)
    {
        for (std::size_t i = 0; i != 100; i++)
        {
            a += stare.next();
            stare.update(sim_resp[i]);
        }
    }
    CORRADE_VERIFY(a);
}

CORRADE_TEST_MAIN(TestStaircase)
