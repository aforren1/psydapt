#ifndef PSYDAPT_QUESTPLUS_HPP
#define PSYDAPT_QUESTPLUS_HPP
/*
This file is part of psydapt.

Copyright © 2021 Alexander Forrence <alex.forrence@gmail.com>

psydapt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

psydapt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with psydapt.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstddef>
#include <vector>
#include <random>
#include <array>
#include <tuple>
#include <stdexcept>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xsort.hpp"
#include "xtensor/xmath.hpp"
#include "xtensor/xutils.hpp"
#include "xtensor/xnoalias.hpp"

#include "../../config.hpp"
#include "../base.hpp"

/** @file
 * @brief Class @ref psydapt::questplus::QuestPlusBase
 * 
 * Implementation based on https://github.com/hoechenberger/questplus
 */
namespace psydapt::questplus
{

    namespace detail
    {
        // https://stackoverflow.com/a/27568312/2690232
        template <std::size_t Num>
        bool increment(std::array<std::size_t, Num> &v, std::array<std::size_t, Num> &upper)
        {
            for (auto i = v.size(); i-- != 0;)
            {
                ++v[i];
                if (v[i] != upper[i])
                {
                    return true;
                }
                v[i] = 0;
            }
            return false;
        }
    }
    /** @brief Stimulus selection method.
         *  
         * `MinNEntropy` is currently ignored.
         */
    enum class StimSelectionMethod
    {
        MinEntropy,
        MinNEntropy
    };
    /** @brief Parameter estimation method.
         * 
         * Currently ignored.
         */
    enum class ParamEstimationMethod
    {
        Mean,
        Median,
        Mode
    };
    struct MinNEntropyParams
    {
        unsigned int n = 5;                    /// Number of smallest entropies considered if `MinNEntropy` selected.
        unsigned int max_consecutive_reps = 2; /// Number of times stimulus will be presented (`MinNEntropy` only).
        unsigned int random_seed = 1;          /// Random seed used by `StimSelectionMethod`.
    };
    struct BaseParams
    {
        StimSelectionMethod stim_selection_method = StimSelectionMethod::MinEntropy; /// Method used to select next stimulus.
        ParamEstimationMethod param_estimation_method = ParamEstimationMethod::Mean; /// Method used to estimate parameters (ignored).
        MinNEntropyParams min_n_entropy_params;
    };
    template <class T, std::size_t DimStim, std::size_t DimParam, std::size_t NResp = 2>
    class QuestPlusBase : public Base<QuestPlusBase<T, DimStim, DimParam, NResp>, DimStim>
    {
        friend Base<QuestPlusBase, DimStim>;

    public:
        QuestPlusBase(unsigned int seed) : rng{seed} {}
        using stim_type = typename Base<QuestPlusBase, DimStim>::stim_type;

        stim_type next()
        {
            // new_posterior = xt::eval(posterior * likelihoods);
            // this way allows us to appeal to the fact that all latter dimensions
            // match, so we can use the fast broadcasting
            const auto shp = likelihoods.shape();
            if constexpr (std::is_scalar_v<stim_type>)
            {
                for (std::size_t i = 0; i < shp[0]; i++)
                {
                    for (std::size_t j = 0; j < shp[1]; j++)
                    {
                        xt::noalias(xt::view(new_posterior, i, j)) = xt::view(likelihoods, i, j) * posterior;
                    }
                }
            }
            else
            {
                std::array<std::size_t, DimStim + 1> upper;
                std::array<std::size_t, DimStim + 1> idx{0};
                upper[0] = shp[0];
                for (std::size_t i = 0; i < DimStim; i++)
                {
                    upper[i + 1] = shp[i + 1];
                }
                do
                {
                    auto tmp_posterior = std::apply([&np = this->new_posterior](auto &&...xs)
                                                    { return xt::view(np, xs...); },
                                                    idx);
                    auto tmp_likelihood = std::apply([&lk = this->likelihoods](auto &&...xs)
                                                     { return xt::view(lk, xs...); },
                                                     idx);
                    xt::noalias(tmp_posterior) = tmp_likelihood * posterior;
                } while (detail::increment<DimStim + 1>(idx, upper));
            }
            std::array<std::size_t, DimParam> param_idx;
            std::iota(param_idx.begin(), param_idx.end(), 1 + DimStim);
            pk = xt::sum(new_posterior, param_idx, xt::evaluation_strategy::immediate);
            const auto pkt = xt::transpose(pk);
            auto postt = xt::transpose(new_posterior);
            xt::noalias(postt) = postt / pkt;
            // xt::transpose(new_posterior) /= xt::transpose(pk);

            // entropy
            H = -xt::nansum(new_posterior * xt::log(new_posterior), param_idx, xt::evaluation_strategy::immediate);
            // expected entropies for stimuli
            EH = xt::sum(pk * H, 0, xt::evaluation_strategy::immediate);
            // TODO: just do min_entropy by default until figure out retrieving settings
            // find index of minimum entropy, then figure out which stimuli are there
            const auto settings = static_cast<T *>(this)->settings;
            if (settings.stim_selection_method == StimSelectionMethod::MinEntropy)
            {

                if constexpr (std::is_scalar_v<stim_type>)
                {
                    this->next_stimulus = stimuli[0][xt::argmin(EH)];
                }
                else
                {
                    auto indices = xt::unravel_index(xt::argmin(EH)(), EH.shape(), xt::layout_type::row_major);
                    for (std::size_t i = 0; i < DimStim; i++)
                    {
                        this->next_stimulus[i] = stimuli[i][indices[i]];
                    }
                }
            }
            else if (settings.stim_selection_method == StimSelectionMethod::MinNEntropy)
            {
            }
            return this->next_stimulus;
        }
        bool update(int response, std::optional<stim_type> stimulus = std::nullopt)
        {
            if (response < 0 || static_cast<std::size_t>(response) >= n_resp)
            {
                PSYDAPT_THROW(std::invalid_argument, "The response is outside the valid range.");
            }
            this->stimulus_history.push_back(stimulus ? *stimulus : this->next_stimulus);
            this->response_history.push_back(response);
            // find nearest matching response
            xt::xtensor<double, DimParam + DimStim> likelihood2 = xt::view(likelihoods, response, xt::all());
            const auto last_stim = this->stimulus_history.back();

            xt::xtensor<double, DimParam> likelihood;
            if constexpr (std::is_scalar_v<stim_type>)
            {
                // find index of nearest stimulus input, and take a view from there
                std::size_t idx = xt::argmin(xt::abs(stimuli[0] - last_stim))[0];
                likelihood = xt::view(likelihood2, idx, xt::all());
            }
            else
            {
                // need to loop over all of DimStim/whatever the stim length is,
                // taking progressively more views
                std::array<std::size_t, DimStim> idx;
                for (std::size_t i = 0; i < DimStim; i++)
                {
                    idx[i] = xt::argmin(xt::abs(stimuli[i] - last_stim[i]))[0];
                }
                likelihood = std::apply([&likelihood2](auto &&...xs)
                                        { return xt::view(likelihood2, xs...); },
                                        idx);
            }
            xt::noalias(posterior) = posterior * likelihood;
            xt::noalias(posterior) = posterior / xt::sum(posterior, xt::evaluation_strategy::immediate);

            return true; // unconditionally continue for now
        }

    protected:
        // derived classes must tell us how to calc prior & likelihood
        xt::xtensor<double, DimParam> generate_prior()
        {
            return static_cast<T *>(this)->generate_prior();
        }
        // +1 for response dimension
        xt::xtensor<double, DimParam + DimStim + 1> generate_likelihoods()
        {
            return static_cast<T *>(this)->generate_likelihoods();
        }
        void make_stimuli()
        {
            return static_cast<T *>(this)->make_stimuli();
        }
        static constexpr std::size_t dim_stim = DimStim;
        static constexpr std::size_t dim_param = DimParam;
        static constexpr std::size_t n_resp = NResp;
        xt::xtensor<double, DimParam> posterior;
        xt::xtensor<double, DimParam + DimStim + 1> likelihoods;
        std::array<xt::xtensor<double, 1>, DimStim> stimuli;
        xt::xtensor<double, DimParam + DimStim + 1> new_posterior;
        xt::xtensor<double, 1 + DimStim> pk;
        xt::xtensor<double, 1 + DimStim> H;
        xt::xtensor<double, DimStim> EH;
        std::mt19937 rng; // for 'min_n_entropy'

        void setup()
        {
            // everything else for init, post-assigning settings
            posterior = generate_prior();
            likelihoods = generate_likelihoods();
            new_posterior = xt::xtensor<double, DimParam + DimStim + 1>::from_shape(likelihoods.shape());
            make_stimuli();
            // TODO: pick something smarter, once we incorporate termination conditions
            this->response_history.reserve(500);
            this->stimulus_history.reserve(500);
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
                    PSYDAPT_THROW(std::invalid_argument, "The prior and parameter domain sizes must match.");
                }
                out_prior = xt::adapt<xt::layout_type::row_major>(tp, prior_shape);
            }
            else
            {
                out_prior = xt::ones<double>(prior_shape);
            }
            return out_prior;
        }
    };
} // namespace psydapt::questplus

#endif
