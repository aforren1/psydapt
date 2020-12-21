#ifndef PSYDAPT_QUESTPLUS_WEIBULL_HPP
#define PSYDAPT_QUESTPLUS_WEIBULL_HPP

#include <vector>
#include <optional>
#include "xtensor/xarray.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"

#include "fn_base.hpp"

namespace psydapt
{
    namespace questplus
    {
        class Weibull : PsychometricFunction
        {
        public:
            struct Params : BaseParams
            {
                std::vector<double> threshold;
                std::vector<double> slope{3.5};
                std::vector<double> lower_asymptote{0.01};
                std::vector<double> lapse_rate{0.01};
                std::optional<std::vector<double>> threshold_prior;
                std::optional<std::vector<double>> slope_prior;
                std::optional<std::vector<double>> lower_asymptote_prior;
                std::optional<std::vector<double>> lapse_rate_prior;
            };

            Params settings;
            const std::vector<double> outcome_domain{0, 1};

            Weibull(const Params &params)
            {
                settings = params;
            }

            xt::xarray<double> generate_prior()
            {
                using xtd4 = xt::xtensor<double, 4>;
                using shape_t = xtd4::shape_type;
                // threshold
                const auto thresh_size = settings.threshold.size();
                const shape_t thresh_shape{thresh_size, 1, 1, 1};
                xtd4 threshold_prior;
                if (settings.threshold_prior)
                {
                    auto &tp = *settings.threshold_prior;
                    if (tp.size() != thresh_size)
                    {
                        throw std::invalid_argument("The threshold prior and domain must match.");
                    }
                    threshold_prior = xt::adapt(tp, thresh_shape);
                }
                else
                {
                    threshold_prior = xt::ones<double>(thresh_shape);
                }
                // slope
                const auto slope_size = settings.slope.size();
                const shape_t slope_shape{1, slope_size, 1, 1};
                xtd4 slope_prior;
                if (settings.slope_prior)
                {
                    auto &tp = *settings.slope_prior;
                    if (tp.size() != slope_size)
                    {
                        throw std::invalid_argument("The slope prior and domain must match.");
                    }
                    slope_prior = xt::adapt(tp, slope_shape);
                }
                else
                {
                    slope_prior = xt::ones<double>(slope_shape);
                }
                // lower asymptote
                const auto lower_asymptote_size = settings.lower_asymptote.size();
                const shape_t lower_asymptote_shape{1, 1, lower_asymptote_size, 1};
                xtd4 lower_asymptote_prior;
                if (settings.lower_asymptote_prior)
                {
                    auto &tp = *settings.lower_asymptote_prior;
                    if (tp.size() != lower_asymptote_size)
                    {
                        throw std::invalid_argument("The lower asymptote prior and domain must match.");
                    }
                    lower_asymptote_prior = xt::adapt(tp, lower_asymptote_shape);
                }
                else
                {
                    lower_asymptote_prior = xt::ones<double>(lower_asymptote_shape);
                }
                // lapse rate
                const auto lapse_rate_size = settings.lapse_rate.size();
                const shape_t lapse_rate_shape{1, 1, 1, lapse_rate_size};
                xtd4 lapse_rate_prior;
                if (settings.lapse_rate_prior)
                {
                    auto &tp = *settings.lapse_rate_prior;
                    if (tp.size() != lapse_rate_size)
                    {
                        throw std::invalid_argument("The lapse rate prior and domain must match.");
                    }
                    lapse_rate_prior = xt::adapt(tp, lapse_rate_shape);
                }
                else
                {
                    lapse_rate_prior = xt::ones<double>(lapse_rate_shape);
                }
                auto prior = threshold_prior * slope_prior * lower_asymptote_prior * lapse_rate_prior;
                return prior / xt::sum(prior);
            }

            xt::xarray<double> generate_likelihoods()
            {
                using sz = std::vector<std::size_t>;
                auto x = xt::adapt(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
                auto thresh = xt::adapt(settings.threshold, sz{1, settings.threshold.size(), 1, 1, 1});
                auto slope = xt::adapt(settings.slope, sz{1, 1, settings.slope.size(), 1, 1});
                auto lower = xt::adapt(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
                auto lapse = xt::adapt(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

                xt::xarray<double> p;
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
