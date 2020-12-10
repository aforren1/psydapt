#ifndef PSYDAPT_QUESTPLUS_FNS_HPP
#define PSYDAPT_QUESTPLUS_FNS_HPP

#include <vector>
#include <optional>
#include "xtensor/xarray.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"

#include "base.hpp"

/*
PsychometricFunctions (at least for quest+) know the following:
 - stimulus domain (all valid inputs, e.g. intensity)
 - parameter domain (all valid parameter values, e.g. thresholds & slopes)
 - outcome domain? (all valid responses) (always 0 or 1 at this point!)
 - priors over parameters (need to match dim/length of corresponding parameter domain)
 - stimulus scale (log10, lin, dB)
*/
namespace psydapt
{
    namespace questplus
    {
        class PsychometricFunction
        {
        public:
            virtual xt::xarray<double> generate_grid() = 0;
        };

        struct BaseParams
        {
            std::vector<double> intensity;
            Scale scale = Scale::Log10;
        };

        class Weibull : PsychometricFunction
        {
        public:
            struct Params : BaseParams
            {
                std::vector<double> threshold;
                std::vector<double> slope = {3.5};
                std::vector<double> lower_asymptote = {0.01};
                std::vector<double> lapse_rate = {0.01};
                std::optional<std::vector<double>> threshold_prior;
                std::optional<std::vector<double>> slope_prior;
                std::optional<std::vector<double>> lower_asymptote_prior;
                std::optional<std::vector<double>> lapse_rate_prior;
            };

            Params settings;

            Weibull(const Params &params)
            {
                settings = params;
            }

            xt::xarray<double> generate_grid()
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
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(xt::pow(-(x / thresh), slope));
                    break;
                case Scale::Log10:
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(xt::pow(-10, slope * (x - thresh)));
                    break;
                case Scale::dB:
                    p = 1 - lapse - (1 - lower - lapse) * xt::exp(xt::pow(-10, slope * (x - thresh) / 20.0));
                    break;
                }
                return p;
            }
        };
    } // namespace questplus
} // namespace psydapt

#endif
