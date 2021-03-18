#ifndef PSYDAPT_STAIRCASE_HPP
#define PSYDAPT_STAIRCASE_HPP
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

#include <cmath>
#include <vector>
#include <optional>

#include "../../config.hpp"
#include "../base.hpp"

/** @file
 * @brief Class @ref psydapt::staircase::Staircase
 */
namespace psydapt
{
    namespace staircase
    {
        /**
         * @brief C++ port of PsychoPy's StairHandler
         * https://github.com/psychopy/psychopy/blob/817ed9e38c6ac1d15a4beda98e916031e3bacaac/psychopy/data/staircase.py#L46
         */
        class Staircase : public Base<Staircase, 1>
        {
        public:
            struct Params
            {
                double start_val;                                       /// The initial value for the staircase.
                std::vector<double> step_sizes;                         /// Step sizes, which moves to next value with each reversal.
                unsigned int n_trials;                                  /// Minimum number of trials.
                int n_up;                                               /// Number of '0' responses before the staircase increases.
                int n_down;                                             /// Number of '1' responses before the staircase decreases.
                bool apply_initial_rule;                                /// If `true`, apply 1-up/1-down rule until first reversal.
                Scale stim_scale = Scale::Linear;                       /// Scale of the stimulus.
                std::optional<unsigned int> n_reversals = std::nullopt; /// Minimum number of reversals.
                std::optional<double> min_val = std::nullopt;           /// Smallest allowed staircase value.
                std::optional<double> max_val = std::nullopt;           /// Largest allowed staircase value.
            };
            Staircase(const Params &params) : settings(params)
            {
                step_size = settings.step_sizes[0];

                // even if step_sizes is specified, may be of length 1
                variable_step = settings.step_sizes.size() > 1;
                next_stimulus = settings.start_val; // starting point
                // reserve 10x the number of expected trials (arbitrary)
                response_history.reserve(10 * settings.n_trials);
                stimulus_history.reserve(10 * settings.n_trials);
                // handle n_reversals (default is 1)
                if (!params.n_reversals)
                {

                    settings.n_reversals = settings.step_sizes.size();
                    // length=1 step_sizes implicitly handled (default is 1)
                }
                else if (settings.step_sizes.size() > *params.n_reversals)
                {
                    settings.n_reversals = settings.step_sizes.size();
                }
                else
                {
                    settings.n_reversals = *params.n_reversals;
                }
            }

            double next()
            {
                // first call to next(), easy out
                if (trial_count <= 0)
                {
                    return next_stimulus;
                }

                if (!should_continue)
                {
                    PSYDAPT_THROW(std::runtime_error, "The staircase has already finished.");
                }

                // asking for trial #2, use 1-down, 1-up rule
                bool reversal = false;
                bool initial_rule = false;
                if (!reversal_count && settings.apply_initial_rule)
                {
                    initial_rule = true; // moved from later
                    // correct response previously
                    if (response_history.end()[-1])
                    {
                        reversal = current_direction == 1;
                        current_direction = -1;
                    }
                    else // wrong
                    {
                        reversal = current_direction == -1;
                        current_direction = 1;
                    }
                }
                // n correct, time to go down
                else if (correct_count >= settings.n_down)
                {
                    reversal = !(current_direction == 0 || current_direction == -1);
                    current_direction = -1;
                }
                // n wrong, time to go up
                else if (correct_count <= -settings.n_up)
                {
                    reversal = !(current_direction == 0 || current_direction == 1);
                    current_direction = 1;
                }

                // add reversal info
                reversal_count += reversal ? 1 : 0;
                // new step size if necessary
                if (reversal && variable_step)
                {
                    // check if we've gone beyond the list of step sizes; use the
                    // last one otherwise
                    if (reversal_count >= settings.step_sizes.size())
                    {
                        step_size = settings.step_sizes.end()[-1];
                    }
                    else
                    {
                        step_size = settings.step_sizes[reversal_count];
                    }
                }
                // apply new step size
                // check initial rule stuff first
                if ((!reversal_count || initial_rule) && settings.apply_initial_rule)
                {
                    initial_rule = false;
                    int sign = response_history.end()[-1] ? -1 : 1;
                    step(sign);
                }
                else if (correct_count >= settings.n_down)
                {
                    // n right, so decrement
                    step(-1);
                }
                else if (correct_count <= -settings.n_up)
                {
                    // n wrong, so increment
                    step(1);
                }
                return next_stimulus;
            }

            bool update(int response, std::optional<double> stimulus = std::nullopt)
            {
                // update history of stimulus/response
                // if the user provides an stimulus value, use that
                // otherwise, fill in the last generated one
                stimulus_history.push_back(stimulus ? *stimulus : next_stimulus);
                response_history.push_back(response);

                if (response)
                {
                    if (response_history.size() > 1 && response_history.end()[-2] == response)
                    {
                        correct_count++;
                    }
                    else
                    {
                        correct_count = 1;
                    }
                }
                else
                {
                    if (response_history.size() > 1 && response_history.end()[-2] == response)
                    {
                        correct_count--;
                    }
                    else
                    {
                        correct_count = -1;
                    }
                }

                // check termination condition, and return false if we should stop
                trial_count++;
                if (reversal_count >= settings.n_reversals && stimulus_history.size() >= settings.n_trials)
                {
                    should_continue = false;
                }
                return should_continue;
            }

        private:
            unsigned int trial_count = 0;
            unsigned int reversal_count = 0; // use this rather than tracking the reversal intensities
            int correct_count = 0;           //
            int current_direction = 0;       // 1 = up, -1 = down, 0 = initial
            bool variable_step = false;
            double step_size;

            Params settings;

            void step(double sign)
            {
                double temp_step = step_size * sign;
                switch (settings.stim_scale)
                {
                case Scale::dB:
                    next_stimulus *= std::pow(10.0, temp_step * 0.05);
                    break;
                case Scale::Log10:
                    next_stimulus *= std::pow(10.0, temp_step);
                    break;
                case Scale::Linear:
                    next_stimulus += temp_step;
                    break;
                }
                if (settings.max_val && sign > 0)
                {
                    next_stimulus = std::min(next_stimulus, *settings.max_val);
                }
                else if (settings.min_val && sign < 0)
                {
                    next_stimulus = std::max(next_stimulus, *settings.min_val);
                }
                correct_count = 0;
            }
        };
    } // namespace staircase

} // namespace psydapt

#endif
