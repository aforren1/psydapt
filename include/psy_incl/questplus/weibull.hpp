#ifndef PSYDAPT_QUESTPLUS_WEIBULL_HPP
#define PSYDAPT_QUESTPLUS_WEIBULL_HPP

#include <vector>
#include <optional>

#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"

#include "../base.hpp"
#include "questplus.hpp"

namespace psydapt
{
    namespace questplus
    {

        class Weibull : public QuestPlusBase<1, 4, 2>
        {
        private:
            // short alias to call super methods
            typedef QuestPlusBase<1, 4, 2> QPB;

        public:
            struct Params : BaseParams
            {
                Scale scale = Scale::Log10;
                std::vector<double> intensity;
                std::vector<double> threshold;
                std::vector<double> slope{3.5};
                std::vector<double> lower_asymptote{0.01};
                std::vector<double> lapse_rate{0.01};
                std::optional<std::vector<double>> threshold_prior;
                std::optional<std::vector<double>> slope_prior;
                std::optional<std::vector<double>> lower_asymptote_prior;
                std::optional<std::vector<double>> lapse_rate_prior;
            };
            Weibull(const Params &params) : QPB(params.random_seed), settings(params)
            {
                // we need to delay
                QPB::setup();
            }

        protected:
            const Params settings;

            xt::xtensor<double, Weibull::dim_param> generate_prior()
            {
                auto thresh_prior = prior_helper(settings.threshold, settings.threshold_prior, 0);
                auto slope_prior = prior_helper(settings.slope, settings.slope_prior, 1);
                auto lower_prior = prior_helper(settings.lower_asymptote,
                                                settings.lower_asymptote_prior, 2);
                auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 3);
                auto prior = thresh_prior * slope_prior * lower_prior * lapse_prior;
                return prior / xt::sum(prior);
            }

            xt::xtensor<double, Weibull::dim_param + Weibull::dim_stim + 1> generate_likelihoods()
            {
                using sz = std::array<std::size_t, Weibull::dim_param + Weibull::dim_stim + 1>;
                auto x = xt::adapt(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
                auto thresh = xt::adapt(settings.threshold, sz{1, settings.threshold.size(), 1, 1, 1});
                auto slope = xt::adapt(settings.slope, sz{1, 1, settings.slope.size(), 1, 1});
                auto lower = xt::adapt(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
                auto lapse = xt::adapt(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

                xt::xtensor<double, Weibull::dim_param + Weibull::dim_stim> p;
                switch (settings.scale)
                {
                case Scale::Linear:
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(x / thresh, slope));
                    break;
                case Scale::Log10:
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh)));
                    break;
                case Scale::dB:
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(-xt::pow(10, slope * (x - thresh) / 20.0));
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
