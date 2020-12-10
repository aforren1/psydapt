#include <iostream>
#include <vector>
#include <type_traits>

namespace psydapt
{
    namespace questplus
    {
        class PsychometricFunction
        {
        };

        class Weibull : PsychometricFunction
        {
        public:
            struct Params
            {
                int mean = 3;
            };

            Weibull(const Params &params)
            {
                settings = params;
            }
            Params settings;
        };
    } // namespace questplus
} // namespace psydapt

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
    class QuestPlus
    {
    public:
        struct Params
        {
            questplus::StimSelectionMethod stim_selection_method;
            questplus::ParamEstimationMethod param_estimation_method;
            int n = 5;
            int max_consecutive_reps = 2;
            int random_seed = 1;
        };

        QuestPlus(const Params &params, const typename T::Params &psychometric_params) : psychometric_fn{psychometric_params}
        {
        }

        void get_it()
        {
            std::cout << psychometric_fn.settings.mean;
        }

    private:
        T psychometric_fn;
        Params settings;
    };

} // namespace psydapt

int main()
{
    using namespace psydapt;
    using qp = QuestPlus<questplus::Weibull>;
    questplus::Weibull::Params p2;
    qp::Params p{questplus::StimSelectionMethod::MinEntropy, questplus::ParamEstimationMethod::Mean};
    qp questp{p, p2};

    questp.get_it();
    return 0;
}
