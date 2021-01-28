#include <iostream>
#include <vector>

#include "psydapt.hpp"

int main()
{
    using namespace psydapt::staircase;
    Staircase::Params params;
    params.start_val = 0.5;
    params.n_reversals = 3;
    params.step_sizes = {0.1, 0.01};
    params.n_trials = 10;
    params.n_up = 2;
    params.n_down = 2;
    params.apply_initial_rule = true;
    params.scale = psydapt::Scale::Linear;
    params.min_val = 0.1;

    Staircase stare{params};

    bool cont = true;
    while (cont)
    {
        std::cout << "Stim val: " << stare.next() << std::endl;
        std::cout << "Correct? ";
        int corr;
        std::cin >> corr;
        cont = stare.update(corr);
    }
    return 0;
}
