#ifndef PSYDAPT_QUESTPLUS_HPP
#define PSYDAPT_QUESTPLUS_HPP

#include "base.hpp"

enum class Method
{
    Quantile,
    Mean,
    Mode
};

struct Weibull
{
    double beta;
    double delta;
    double gamma;
};

enum class PsychometricFn
{
    Weibull
};

struct QuestParams
{
    double start_val;
    double start_val_sd;
    double p_threshold;
    int n_trials;
    double grain;
    double min_val;
    double max_val;
};
#endif