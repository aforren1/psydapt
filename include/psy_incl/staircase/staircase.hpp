
#ifndef PSYDAPT_STAIRCASE_HPP
#define PSYDAPT_STAIRCASE_HPP

#include <cmath>
#include <vector>
#include <optional>

#include "../base.hpp"

namespace psydapt
{
    /**
     * C++ port of PsychoPy's StairHandler
     * https://github.com/psychopy/psychopy/blob/817ed9e38c6ac1d15a4beda98e916031e3bacaac/psychopy/data/staircase.py#L46
     * 
     * 
     */
    namespace staircase
    {
        class Staircase : public Base<1>
        {
        public:
            struct Params
            {
                double start_val;
                std::optional<unsigned int> n_reversals = std::nullopt;
                std::vector<double> step_sizes;
                unsigned int n_trials;
                int n_up;
                int n_down;
                bool apply_initial_rule;
                Scale stim_scale = Scale::Linear;
                std::optional<double> min_val = std::nullopt;
                std::optional<double> max_val = std::nullopt;
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
            /**
         * Compute the next stimulus value.
         * @return Next stimulus value.
         */
            double next()
            {
                // first call to next(), easy out
                if (trial_count <= 0)
                {
                    return next_stimulus;
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
                    response_history.end()[-1] ? decrement() : increment();
                }
                else if (correct_count >= settings.n_down)
                {
                    // n right, so decrement
                    decrement();
                }
                else if (correct_count <= -settings.n_up)
                {
                    // n wrong, so increment
                    increment();
                }
                return next_stimulus;
            }

            /**
         * Update the staircase with user response and (optional) stimulus used, and check whether to proceed or not.
         * @param value Response made by participant (usually 0 or 1)
         * @param stimulus Optional stimulus of the stimulus, if different from the one
         * produced by the staircase. Otherwise, the most recent value produced by `next()` is used.
         * @return Whether to continue the staircase or not.
         */
            bool update(int value, std::optional<double> stimulus = std::nullopt)
            {
                // update history of stimulus/response
                // if the user provides an stimulus value, use that
                // otherwise, fill in the last generated one
                stimulus_history.push_back(stimulus ? *stimulus : next_stimulus);
                response_history.push_back(value);

                if (value)
                {
                    if (response_history.size() > 1 && response_history.end()[-2] == value)
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
                    if (response_history.size() > 1 && response_history.end()[-2] == value)
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
                    return false;
                }
                return true;
            }

        private:
            unsigned int trial_count = 0;
            unsigned int reversal_count = 0; // use this rather than tracking the reversal intensities
            int correct_count = 0;           //
            int current_direction = 0;       // 1 = up, -1 = down, 0 = initial
            bool variable_step = false;
            double step_size;

            Params settings;

            void increment()
            {
                switch (settings.stim_scale)
                {
                case Scale::dB:
                    next_stimulus *= std::pow(10.0, step_size / 20.0);
                    break;
                case Scale::Log10:
                    next_stimulus *= std::pow(10.0, step_size);
                    break;
                case Scale::Linear:
                    next_stimulus += step_size;
                    break;
                }
                if (settings.max_val)
                {
                    next_stimulus = std::min(next_stimulus, *settings.max_val);
                }
                correct_count = 0;
            }

            void decrement()
            {
                switch (settings.stim_scale)
                {
                case Scale::dB:
                    next_stimulus /= std::pow(10.0, step_size / 20.0);
                    break;
                case Scale::Log10:
                    next_stimulus /= std::pow(10.0, step_size);
                    break;
                case Scale::Linear:
                    next_stimulus -= step_size;
                    break;
                }
                if (settings.min_val)
                {
                    next_stimulus = std::max(next_stimulus, *settings.min_val);
                }
                correct_count = 0;
            }
        };
    } // namespace staircase

} // namespace psydapt

#endif
