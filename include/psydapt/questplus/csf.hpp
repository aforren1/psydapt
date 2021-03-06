#ifndef PSYDAPT_QUESTPLUS_CSF_HPP
#define PSYDAPT_QUESTPLUS_CSF_HPP
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
 * @brief Class @ref psydapt::questplus::CSF
 */

namespace psydapt::questplus
{
    class CSF : public QuestPlusBase<CSF, 3, 7, 2>
    {
        typedef QuestPlusBase<CSF, 3, 7, 2> QPB;
        friend QPB;

    public:
        struct Params : BaseParams
        {
            Scale stim_scale = Scale::Log10; /// Scale of the stimulus.
            // stim
            std::vector<double> contrast;      /// Array of possible contrast thresholds.
            std::vector<double> spatial_freq;  /// Array of possible spatial frequencies.
            std::vector<double> temporal_freq; /// Array of possible temporal frequencies.
            // params
            std::vector<double> c0;                    /// Array of possible values for this coefficient.
            std::vector<double> cf;                    /// Array of possible values for this coefficient.
            std::vector<double> cw;                    /// Array of possible values for this coefficient.
            std::vector<double> min_thresh;            /// Array of possible minimum thresholds.
            std::vector<double> slope{3.5};            /// Array of possible slope parameter values.
            std::vector<double> lower_asymptote{0.01}; /// Array of possible lower asymptote parameter values.
            std::vector<double> lapse_rate{0.01};      /// Array of possible lapse rate parameter values.

            std::optional<std::vector<double>> c0_prior;
            std::optional<std::vector<double>> cf_prior;
            std::optional<std::vector<double>> cw_prior;
            std::optional<std::vector<double>> min_thresh_prior;
            std::optional<std::vector<double>> slope_prior;
            std::optional<std::vector<double>> lower_asymptote_prior;
            std::optional<std::vector<double>> lapse_rate_prior;
        };
        CSF(const Params &params) : QPB(params.min_n_entropy_params.random_seed), settings(params)
        {
            QPB::setup();
        }

    protected:
        const Params settings;

        void make_stimuli()
        {
            stimuli[0] = xt::adapt<xt::layout_type::row_major>(settings.contrast, {settings.contrast.size()});
            stimuli[1] = xt::adapt<xt::layout_type::row_major>(settings.spatial_freq, {settings.spatial_freq.size()});
            stimuli[2] = xt::adapt<xt::layout_type::row_major>(settings.temporal_freq, {settings.temporal_freq.size()});
        }

        xt::xtensor<double, CSF::dim_param> generate_prior()
        {
            const auto c0_prior = prior_helper(settings.c0, settings.c0_prior, 0);
            const auto cf_prior = prior_helper(settings.cf, settings.cf_prior, 1);
            const auto cw_prior = prior_helper(settings.cw, settings.cw_prior, 2);
            const auto min_thresh_prior = prior_helper(settings.min_thresh, settings.min_thresh_prior, 3);
            const auto slope_prior = prior_helper(settings.slope, settings.slope_prior, 4);
            const auto lower_prior = prior_helper(settings.lower_asymptote, settings.lower_asymptote_prior, 5);
            const auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 6);
            xt::xtensor<double, CSF::dim_param> prior = c0_prior * cf_prior * cw_prior * min_thresh_prior * slope_prior * lower_prior * lapse_prior;
            return prior / xt::sum(prior, xt::evaluation_strategy::immediate);
        }

        xt::xtensor<double, CSF::dim_param + CSF::dim_stim + 1> generate_likelihoods()
        {
            // (7 param, 3 stim)
            const Params &set = settings;
            using sz = std::array<std::size_t, CSF::dim_param + CSF::dim_stim>;
            // const auto &row_major = xt::layout_type::row_major;
            // stim
            const auto x = xt::adapt<xt::layout_type::row_major>(set.contrast, sz{set.contrast.size(), 1, 1, 1, 1, 1, 1, 1, 1, 1});
            const auto f = xt::adapt<xt::layout_type::row_major>(set.spatial_freq, sz{1, set.spatial_freq.size(), 1, 1, 1, 1, 1, 1, 1, 1});
            const auto w = xt::adapt<xt::layout_type::row_major>(set.temporal_freq, sz{1, 1, set.temporal_freq.size(), 1, 1, 1, 1, 1, 1, 1});
            // param
            const auto c0 = xt::adapt<xt::layout_type::row_major>(set.c0, sz{1, 1, 1, set.c0.size(), 1, 1, 1, 1, 1, 1});
            const auto cf = xt::adapt<xt::layout_type::row_major>(set.cf, sz{1, 1, 1, 1, set.cf.size(), 1, 1, 1, 1, 1});
            const auto cw = xt::adapt<xt::layout_type::row_major>(set.cw, sz{1, 1, 1, 1, 1, set.cw.size(), 1, 1, 1, 1});
            const auto min_thresh = xt::adapt<xt::layout_type::row_major>(set.min_thresh, sz{1, 1, 1, 1, 1, 1, set.min_thresh.size(), 1, 1, 1});
            const auto slope = xt::adapt<xt::layout_type::row_major>(set.slope, sz{1, 1, 1, 1, 1, 1, 1, set.slope.size(), 1, 1});
            const auto lower = xt::adapt<xt::layout_type::row_major>(set.lower_asymptote, sz{1, 1, 1, 1, 1, 1, 1, 1, set.lower_asymptote.size(), 1});
            const auto lapse = xt::adapt<xt::layout_type::row_major>(set.lapse_rate, sz{1, 1, 1, 1, 1, 1, 1, 1, 1, set.lapse_rate.size()});

            const auto t = xt::maximum(min_thresh, c0 + cf * f + cw * w);
            xt::xtensor<double, CSF::dim_param + CSF::dim_stim> p;
            switch (settings.stim_scale)
            {
            case Scale::Linear:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(x / t, slope));
                break;
            case Scale::Log10:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - t)));
                break;
            case Scale::dB:
                p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - t) * 0.05));
                break;
            }
            // in this, we diverge from hoechenberger/questplus
            // store 0/incorrect as 0th element, so that we can index using the response
            return xt::stack(xt::xtuple(1.0 - p, p));
        }
    };
} // namespace psydapt::questplus

#endif
