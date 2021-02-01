#ifndef PSYDAPT_QUESTPLUS_NORMCDF_HPP
#define PSYDAPT_QUESTPLUS_NORMCDF_HPP

#include <vector>
#include <optional>
#include <cmath>

#include "xtensor/xio.hpp"
#include "xtensor/xtensor.hpp"
#include "xtensor/xadapt.hpp"
#include "xtensor/xmath.hpp"
#include "xtensor/xvectorize.hpp"

#include "../base.hpp"
#include "questplus.hpp"

namespace psydapt
{
    namespace questplus
    {
        namespace detail
        {
            double norm_cdf(double x);
            double norm_cdf(double x)
            {
                return std::erfc(-x / std::sqrt(2)) / 2;
            }
            auto vec_norm_cdf = xt::vectorize(norm_cdf);
        } // namespace detail
        class NormCDF : public QuestPlusBase<1, 4, 2>
        {
        private:
            typedef QuestPlusBase<1, 4, 2> QPB;

        public:
            struct Params : BaseParams
            {
                Scale stim_scale = Scale::Linear;
                std::vector<double> intensity;
                std::vector<double> location;
                std::vector<double> scale{3.5};
                std::vector<double> lower_asymptote{0.01};
                std::vector<double> lapse_rate{0.01};
                std::optional<std::vector<double>> location_prior;
                std::optional<std::vector<double>> scale_prior;
                std::optional<std::vector<double>> lower_asymptote_prior;
                std::optional<std::vector<double>> lapse_rate_prior;
            };
            NormCDF(const Params &params) : QPB(params.random_seed), settings(params)
            {
                QPB::setup();
            }

        protected:
            const Params settings;

            xt::xtensor<double, NormCDF::dim_stim> make_stimuli()
            {
                return xt::adapt(settings.intensity, {settings.intensity.size()});
            }

            xt::xtensor<double, NormCDF::dim_param> generate_prior()
            {
                auto loc_prior = prior_helper(settings.location, settings.location_prior, 0);
                auto scale_prior = prior_helper(settings.scale, settings.scale_prior, 1);
                auto lower_prior = prior_helper(settings.lower_asymptote, settings.lower_asymptote_prior, 2);
                auto lapse_prior = prior_helper(settings.lapse_rate, settings.lapse_rate_prior, 3);
                xt::xtensor<double, NormCDF::dim_param> prior = loc_prior * scale_prior * lower_prior * lapse_prior;
                return prior / xt::sum(prior);
            }

            xt::xtensor<double, NormCDF::dim_param + NormCDF::dim_stim + 1> generate_likelihoods()
            {
                using sz = std::array<std::size_t, NormCDF::dim_param + NormCDF::dim_stim>;
                auto x = xt::adapt(settings.intensity, sz{settings.intensity.size(), 1, 1, 1, 1});
                auto loc = xt::adapt(settings.location, sz{1, settings.location.size(), 1, 1, 1});
                auto scale = xt::adapt(settings.scale, sz{1, 1, settings.scale.size(), 1, 1});
                auto lower = xt::adapt(settings.lower_asymptote, sz{1, 1, 1, settings.lower_asymptote.size(), 1});
                auto lapse = xt::adapt(settings.lapse_rate, sz{1, 1, 1, 1, settings.lapse_rate.size()});

                xt::xtensor<double, NormCDF::dim_param + NormCDF::dim_stim> p;
                switch (settings.stim_scale)
                {
                case Scale::Linear:
                    p = lower + (1 - lower - lapse) * detail::vec_norm_cdf((x - loc) / scale);
                    break;
                default:
                    throw std::invalid_argument("Only 'Linear' stimulus scale is implemented for NormCDF.");
                    break;
                }
                return xt::stack(xt::xtuple(1.0 - p, p));
            }
        };
    } // namespace questplus
} // namespace psydapt

#endif
