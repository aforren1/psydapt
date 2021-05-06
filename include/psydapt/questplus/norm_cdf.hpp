#ifndef PSYDAPT_QUESTPLUS_NORMCDF_HPP
#define PSYDAPT_QUESTPLUS_NORMCDF_HPP
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
#include <cmath>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"
#include "xtensor/xvectorize.hpp"

#include "../../config.hpp"
#include "../base.hpp"
#include "questplus.hpp"

/** @file
 * @brief Class @ref psydapt::questplus::NormCDF
 */
namespace psydapt::questplus
{
    namespace detail
    {
        template <typename T>
        T norm_cdf(T x)
        {
            return std::erfc(-x * std::sqrt(0.5)) * 0.5;
        }
        static auto vec_norm_cdf = xt::vectorize(norm_cdf<double>);
    } // namespace detail
    class NormCDF : public QuestPlusBase<NormCDF, 1, 4, 2>
    {
        typedef QuestPlusBase<NormCDF, 1, 4, 2> QPB;
        friend QPB;

    public:
        struct Params : BaseParams
        {
            Scale stim_scale = Scale::Linear;                         /// Scale of the stimulus.
            std::vector<double> intensity;                            /// Array of possible stimulus values.
            std::vector<double> location;                             /// Array of possible location parameter values.
            std::vector<double> scale{3.5};                           /// Array of possible scale parameter values.
            std::vector<double> lower_asymptote{0.01};                /// Array of possible lower asymptote parameter values.
            std::vector<double> lapse_rate{0.01};                     /// Array of possible lapse rate parameter values.
            std::optional<std::vector<double>> location_prior;        /// Prior over location.
            std::optional<std::vector<double>> scale_prior;           /// Prior over scale.
            std::optional<std::vector<double>> lower_asymptote_prior; /// Prior over lower asymptote.
            std::optional<std::vector<double>> lapse_rate_prior;      /// Prior over lapse rate.
        };
        NormCDF(const Params &params) : QPB(params.min_n_entropy_params.random_seed), settings(params)
        {
            QPB::setup();
        }

    protected:
        const Params settings;

        void make_stimuli()
        {
            stimuli[0] = xt::adapt<xt::layout_type::row_major>(settings.intensity, {settings.intensity.size()});
        }

        xt::xtensor<double, NormCDF::dim_param> generate_prior()
        {
            auto loc_prior = prior_helper(settings.location, settings.location_prior, 0);
            auto scale_prior = prior_helper(settings.scale, settings.scale_prior, 1);
            auto lower_prior = prior_helper(settings.lower_asymptote, settings.lower_asymptote_prior, 2);
            auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 3);
            xt::xtensor<double, NormCDF::dim_param> prior = loc_prior * scale_prior * lower_prior * lapse_prior;
            return prior / xt::sum(prior, xt::evaluation_strategy::immediate);
        }

        xt::xtensor<double, NormCDF::dim_param + NormCDF::dim_stim + 1> generate_likelihoods()
        {
            using sz = std::array<std::size_t, NormCDF::dim_param + NormCDF::dim_stim>;
            auto x = xt::adapt<xt::layout_type::row_major>(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
            auto loc = xt::adapt<xt::layout_type::row_major>(settings.location, sz{1, settings.location.size(), 1, 1, 1});
            auto scale = xt::adapt<xt::layout_type::row_major>(settings.scale, sz{1, 1, settings.scale.size(), 1, 1});
            auto lower = xt::adapt<xt::layout_type::row_major>(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
            auto lapse = xt::adapt<xt::layout_type::row_major>(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

            xt::xtensor<double, NormCDF::dim_param + NormCDF::dim_stim> p;
            switch (settings.stim_scale)
            {
            case Scale::Linear:
                p = lower + (1 - lower - lapse) * detail::vec_norm_cdf((x - loc) / scale);
                break;
            default:
                PSYDAPT_THROW(std::invalid_argument, "Only 'Linear' stimulus scale is implemented for NormCDF.");
                break;
            }
            return xt::stack(xt::xtuple(1.0 - p, p));
        }
    };
} // namespace psydapt::questplus

#endif
