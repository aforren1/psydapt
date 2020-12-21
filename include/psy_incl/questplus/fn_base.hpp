#ifndef PSYDAPT_QUESTPLUS_FNS_HPP
#define PSYDAPT_QUESTPLUS_FNS_HPP

#include <vector>
#include "xtensor/xarray.hpp"

#include "../base.hpp"

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
            virtual xt::xarray<double> generate_prior() = 0;
            virtual xt::xarray<double> generate_likelihoods() = 0;
            const std::vector<double> outcome_domain{0, 1};
        };

        struct BaseParams
        {
            std::vector<double> intensity;
            Scale scale = Scale::Log10;
        };

    } // namespace questplus
} // namespace psydapt

#endif
