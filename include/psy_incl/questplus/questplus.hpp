#ifndef PSYDAPT_QUESTPLUS_HPP
#define PSYDAPT_QUESTPLUS_HPP

#include <vector>
#include <type_traits>
#include <random>
#include <cmath>
#include "xtensor/xarray.hpp"

#include "fn_base.hpp"

namespace psydapt
{
    namespace questplus
    {
        enum class StimSelectionMethod
        {
            MinEntropy,
            MinNEntropy
        };
        enum class ParamEstimationMethod
        {
            Mean,
            Median,
            Mode
        };
    } // namespace questplus

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<questplus::PsychometricFunction, T>>>
    class QuestPlus : public Base
    {
    public:
        struct Params
        {
            StimSelectionMethod stim_selection_method;
            ParamEstimationMethod param_estimation_method;
            int n = 5;
            int max_consecutive_reps = 2;
            int random_seed = 1;
        };

        QuestPlus(const Params &params, const typename T::Params &psychometric_params) : psychometric_fn{psychometric_params}, rng{params.random_seed}
        {
            settings = params;
            posterior = psychometric_fn.generate_prior();
            likelihoods = psychometric_fn.generate_likelihoods();
            responses.reserve(500); // TODO: these are made up vals
            intensities.reserve(500);
        }

    private:
        T psychometric_fn;
        Params settings;
        xt::xarray<double> posterior;
        xt::xarray<double> likelihoods;
        std::mt19937 rng;
        std::vector<int> responses;      // history of user responses
        std::vector<double> intensities; // history of intensities
        double entropy = NAN;
    };

} // namespace psydapt
#endif
