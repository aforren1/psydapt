#ifndef PSYDAPT_QUESTPLUS_WEIBULL_HPP
#define PSYDAPT_QUESTPLUS_WEIBULL_HPP
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

#include <vector>
#include <optional>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"

#include "../../config.hpp"
#include "../base.hpp"
#include "questplus.hpp"

/** @file
 * @brief Class @ref psydapt::questplus::Weibull
 */
namespace psydapt::questplus
{
    class Weibull : public QuestPlusBase<Weibull, 1, 4, 2>
    {
        typedef QuestPlusBase<Weibull, 1, 4, 2> QPB;
        friend QPB;

    public:
        struct Params : BaseParams
        {
            Scale stim_scale = Scale::Log10;                          /// Scale of the stimulus.
            std::vector<double> intensity;                            /// Array of possible stimulus values.
            std::vector<double> threshold;                            /// Array of possible threshold parameter values.
            std::vector<double> slope{3.5};                           /// Array of possible slope parameter values.
            std::vector<double> lower_asymptote{0.01};                /// Array of possible lower asymptote parameter values.
            std::vector<double> lapse_rate{0.01};                     /// Array of possible lapse rate parameter values.
            std::optional<std::vector<double>> threshold_prior;       /// Prior over threshold.
            std::optional<std::vector<double>> slope_prior;           /// Prior over slope.
            std::optional<std::vector<double>> lower_asymptote_prior; /// Prior over lower asymptote.
            std::optional<std::vector<double>> lapse_rate_prior;      /// Prior over lapse rate.
        };
        Weibull(const Params &params) : QPB(params.min_n_entropy_params.random_seed), settings(params)
        {
            // we need to delay
            QPB::setup();
        }

    protected:
        const Params settings;

        void make_stimuli()
        {
            stimuli[0] = xt::adapt<xt::layout_type::row_major>(settings.intensity, {settings.intensity.size()});
        }

        xt::xtensor<double, Weibull::dim_param> generate_prior()
        {
            auto thresh_prior = prior_helper(settings.threshold, settings.threshold_prior, 0);
            auto slope_prior = prior_helper(settings.slope, settings.slope_prior, 1);
            auto lower_prior = prior_helper(settings.lower_asymptote, settings.lower_asymptote_prior, 2);
            auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 3);
            xt::xtensor<double, Weibull::dim_param> prior = thresh_prior * slope_prior * lower_prior * lapse_prior;
            return prior / xt::sum(prior, xt::evaluation_strategy::immediate);
        }

        xt::xtensor<double, Weibull::dim_param + Weibull::dim_stim + 1> generate_likelihoods()
        {
            using sz = std::array<std::size_t, Weibull::dim_param + Weibull::dim_stim>;
            auto x = xt::adapt<xt::layout_type::row_major>(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
            auto thresh = xt::adapt<xt::layout_type::row_major>(settings.threshold, sz{1, settings.threshold.size(), 1, 1, 1});
            auto slope = xt::adapt<xt::layout_type::row_major>(settings.slope, sz{1, 1, settings.slope.size(), 1, 1});
            auto lower = xt::adapt<xt::layout_type::row_major>(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
            auto lapse = xt::adapt<xt::layout_type::row_major>(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

            xt::xtensor<double, Weibull::dim_param + Weibull::dim_stim> p;
            switch (settings.stim_scale)
            {
            case Scale::Linear:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(x / thresh, slope));
                break;
            case Scale::Log10:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh)));
                break;
            case Scale::dB:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh) * 0.05));
                break;
            }
            // in this, we diverge from hoechenberger/questplus
            // store 0/incorrect as 0th element, so that we can index using the response
            return xt::stack(xt::xtuple(1.0 - p, p));
        }
    };
} // namespace psydapt::questplus

#endif
