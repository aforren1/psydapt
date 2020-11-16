#ifndef PSYDAPT_STAIRCASE_HPP
#define PSYDAPT_STAIRCASE_HPP

#include "base.hpp"

enum class Step
{
    dB,
    Linear,
    Log
};
struct StairParams
{
    double start_val;
    int n_reversals;
    int step_sizes;
    int n_trials;
    int n_up;
    int n_down;
    bool apply_initial_rule;
    Step step_type;
    double min_val;
    double max_val;
};
class Staircase : public psydapt::Base
{
public:
    Staircase(const StairParams &)
    {
    }
    double next()
    {
    }
};

#endif
