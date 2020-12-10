#ifndef PSYDAPT_QUESTPLUS_HPP
#define PSYDAPT_QUESTPLUS_HPP

#include <vector>
#include <type_traits>

#include "base.hpp"
#include "questplus_fns.hpp"

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

        QuestPlus(const Params &params, const typename T::Params &psychometric_params)
        {
        }

    private:
        T psychometric_fn;
        Params settings;
    };

} // namespace psydapt
#endif
