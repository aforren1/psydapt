#ifndef PSYDAPT_QUESTPLUS_HPP
#define PSYDAPT_QUESTPLUS_HPP

#include <cstddef>
#include <vector>
#include <random>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"

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
        struct BaseParams
        {
            StimSelectionMethod stim_selection_method;
            ParamEstimationMethod param_estimation_method;
            unsigned int n = 5;
            unsigned int max_consecutive_reps = 2;
            unsigned int random_seed = 1;
        };
        template <std::size_t DimStim, std::size_t DimParam>
        class QuestPlusBase : public Base<DimStim>
        {
        public:
            QuestPlusBase(unsigned int seed) : rng{seed} {}
            using stim_type = typename Base<DimStim>::stim_type;
            // TODO: fill in next & update
            stim_type next()
            {
                return 0;
            }
            bool update(int response, std::optional<stim_type> stimulus = std::nullopt)
            {
                // TODO: sanitize input? response can be any int...
                stimulus_history.push_back(stimulus ? *stimulus : next_stimulus);
                response_history.push_back(response);
                // find nearest matching response
                auto likelihood = xt::view(likelihoods, response, xt::all());

                if constexpr (std::is_scalar<stim_type>)
                {
                    // find index of nearest stimulus input, and take a view from there
                }
                else
                {
                    // need to loop over all of DimStim/whatever the stim length is,
                    // taking progressively more views
                }

                return true;
            }

        protected:
            // derived classes must tell us how to calc prior & likelihood
            virtual xt::xtensor<double, DimParam> generate_prior() = 0;
            // +1 for response dimension
            virtual xt::xtensor<double, DimParam + DimStim + 1> generate_likelihoods() = 0;
            static constexpr std::size_t dim_stim = DimStim;
            static constexpr std::size_t dim_param = DimParam;
            xt::xtensor<double, DimParam> prior;
            xt::xtensor<double, DimParam> posterior;
            xt::xtensor<double, DimParam + DimStim + 1> likelihoods;
            std::mt19937 rng; // for 'min_n_entropy'

            void setup()
            {
                // everything else for init, post-assigning settings
                prior = generate_prior();
                posterior = prior;
                likelihoods = generate_likelihoods();
                //
            }

            xt::xtensor<double, DimParam> prior_helper(const std::vector<double> &param,
                                                       const std::optional<std::vector<double>> &prior = std::nullopt,
                                                       const std::size_t index = 0)
            {
                const auto param_size = param.size();
                std::array<std::size_t, DimParam> prior_shape;
                prior_shape.fill(1);
                prior_shape[index] = param_size;
                xt::xtensor<double, DimParam> out_prior;
                if (prior)
                {
                    auto &tp = *prior;
                    if (tp.size() != param_size)
                    {
                        throw std::invalid_argument("The prior and parameter domain sizes must match.");
                    }
                    out_prior = xt::adapt(tp, prior_shape);
                }
                else
                {
                    out_prior = xt::ones<double>(prior_shape);
                }
                return out_prior;
            }
        };
    } // namespace questplus
} // namespace psydapt

#endif
