#include <iostream>
#include <vector>

#include "psydapt.hpp"

int main()
{
    using namespace psydapt;
    Staircase<std::vector<double>>::StairParams<std::vector<double>> params;
    params.start_val = 0.5;
    params.n_reversals = 3;
    params.step_sizes = {0.1, 0.01};
    params.n_trials = 10;
    params.n_up = 2;
    params.n_down = 2;
    params.apply_initial_rule = true;
    params.step_type = Scale::Linear;
    params.min_val = 0.1;

    Staircase<std::vector<double>> stare{params};

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
