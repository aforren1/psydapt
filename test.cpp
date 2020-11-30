#include <iostream>

#include "psydapt.hpp"

int main()
{
    using namespace psydapt;
    Staircase::StairParams params;
    params.apply_initial_rule = false;

    std::cout << "success.";
    return 0;
}
