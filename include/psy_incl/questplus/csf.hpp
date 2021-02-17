#ifndef PSYDAPT_QUESTPLUS_CSF_HPP
#define PSYDAPT_QUESTPLUS_CSF_HPP

#include <vector>
#include <optional>

#include "xtensor/xio.hpp"
#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"

#include "../base.hpp"
#include "questplus.hpp"

/** @file
 * @brief Class @ref psydapt::questplus::CSF
 */

namespace psydapt
{
    namespace questplus
    {
        class CSF : public QuestPlusBase<3, 7, 2>
        {
        private:
            typedef QuestPlusBase<3, 7, 2> QPB;

        public:
            struct Params : BaseParams
            {
                Scale stim_scale = Scale::Log10;
                // stim
                std::vector<double> contrast;
                std::vector<double> spatial_freq;
                std::vector<double> temporal_freq;
                // params
                std::vector<double> c0;
                std::vector<double> cf;
                std::vector<double> cw;
                std::vector<double> min_thresh;
                std::vector<double> slope{3.5};
                std::vector<double> lower_asymptote{0.01};
                std::vector<double> lapse_rate{0.01};

                std::optional<std::vector<double>> c0_prior;
                std::optional<std::vector<double>> cf_prior;
                std::optional<std::vector<double>> cw_prior;
                std::optional<std::vector<double>> min_thresh_prior;
                std::optional<std::vector<double>> slope_prior;
                std::optional<std::vector<double>> lower_asymptote_prior;
                std::optional<std::vector<double>> lapse_rate_prior;
            };
            CSF(const Params &params) : QPB(params.random_seed), settings(params)
            {
                QPB::setup();
            }

        protected:
            const Params settings;

            void make_stimuli()
            {
                stimuli[0] = xt::adapt(settings.contrast, {settings.contrast.size()});
                stimuli[1] = xt::adapt(settings.spatial_freq, {settings.spatial_freq.size()});
                stimuli[2] = xt::adapt(settings.temporal_freq, {settings.temporal_freq.size()});
            }

            xt::xtensor<double, CSF::dim_param> generate_prior()
            {
                auto c0_prior = prior_helper(settings.c0, settings.c0_prior, 0);
                auto cf_prior = prior_helper(settings.cf, settings.cf_prior, 1);
                auto cw_prior = prior_helper(settings.cw, settings.cw_prior, 2);
                auto min_thresh_prior = prior_helper(settings.min_thresh, settings.min_thresh_prior, 3);
                auto slope_prior = prior_helper(settings.slope, settings.slope_prior, 4);
                auto lower_prior = prior_helper(settings.lower_asymptote, settings.lower_asymptote_prior, 5);
                auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 6);
                xt::xtensor<double, CSF::dim_param> prior = c0_prior * cf_prior * cw_prior * min_thresh_prior * slope_prior * lower_prior * lapse_prior;
                return prior / xt::sum(prior, xt::evaluation_strategy::immediate);
            }

            xt::xtensor<double, CSF::dim_param + CSF::dim_stim + 1> generate_likelihoods()
            {
                // (7 param, 3 stim)
                const Params &set = settings;
                using sz = std::array<std::size_t, CSF::dim_param + CSF::dim_stim>;
                // stim
                auto x = xt::adapt(set.contrast, sz{set.contrast.size(), 1, 1, 1, 1, 1, 1, 1, 1, 1});
                auto f = xt::adapt(set.spatial_freq, sz{1, set.spatial_freq.size(), 1, 1, 1, 1, 1, 1, 1, 1});
                auto w = xt::adapt(set.temporal_freq, sz{1, 1, set.temporal_freq.size(), 1, 1, 1, 1, 1, 1, 1});
                // param
                auto c0 = xt::adapt(set.c0, sz{1, 1, 1, set.c0.size(), 1, 1, 1, 1, 1, 1});
                auto cf = xt::adapt(set.cf, sz{1, 1, 1, 1, set.cf.size(), 1, 1, 1, 1, 1});
                auto cw = xt::adapt(set.cw, sz{1, 1, 1, 1, 1, set.cw.size(), 1, 1, 1, 1});
                auto min_thresh = xt::adapt(set.min_thresh, sz{1, 1, 1, 1, 1, 1, set.min_thresh.size(), 1, 1, 1});
                auto slope = xt::adapt(set.slope, sz{1, 1, 1, 1, 1, 1, 1, set.slope.size(), 1, 1});
                auto lower = xt::adapt(set.lower_asymptote, sz{1, 1, 1, 1, 1, 1, 1, 1, set.lower_asymptote.size(), 1});
                auto lapse = xt::adapt(set.lapse_rate, sz{1, 1, 1, 1, 1, 1, 1, 1, 1, set.lapse_rate.size()});

                auto t = xt::maximum(min_thresh, c0 + cf * f + cw * w);
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
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - t) / 20.0));
                    break;
                }
                // in this, we diverge from hoechenberger/questplus
                // store 0/incorrect as 0th element, so that we can index using the response
                return xt::stack(xt::xtuple(1.0 - p, p));
            }
        };
    } // namespace questplus
} // namespace psydapt

#endif
