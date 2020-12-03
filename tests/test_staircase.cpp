#include <Corrade/TestSuite/Tester.h>
#include <vector>
#include "psydapt.hpp"
#include "common.hpp"

struct TestStaircase : Corrade::TestSuite::Tester
{
    explicit TestStaircase();

    void linear();
    void prepend1kItemsVector();
};

TestStaircase::TestStaircase()
{
    addTests({&TestStaircase::linear});
    addBenchmarks({&TestStaircase::prepend1kItemsVector}, 100);
}

void TestStaircase::linear()
{
    using namespace psydapt;
    Staircase::StairParams params;
    params.start_val = 0.8;
    params.n_reversals = 4;
    params.step_size = 0.1;
    params.step_sizes = std::vector<double>{0.1, 0.01, 0.001};
    params.n_trials = 20;
    params.n_up = 1;
    params.n_down = 3;
    params.apply_initial_rule = true;
    params.step_type = Scale::Linear;
    params.min_val = 0;
    params.max_val = 1;

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
        std::cout << pred_vals.end()[-1] << " " << sim_resp[counter] << "\n";
        counter++;
    }

    CORRADE_VERIFY(pred_vals == true_vals);
}

void TestStaircase::prepend1kItemsVector()
{
    double a{};
    CORRADE_BENCHMARK(100)
    {
        std::vector<double> container;
        for (std::size_t i = 0; i != 1000; ++i)
            container.insert(container.begin(), 1.0);
        a += container.back();
    }
    CORRADE_VERIFY(a); // to avoid the benchmark loop being optimized out
}

CORRADE_TEST_MAIN(TestStaircase)
