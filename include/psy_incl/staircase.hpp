
#ifndef PSYDAPT_STAIRCASE_HPP
#define PSYDAPT_STAIRCASE_HPP

#include <cmath>
#include <vector>
#include <iostream>

#include "base.hpp"
#include "internal.hpp"

namespace psydapt
{
    /**
     * C++ port of PsychoPy's StairHandler
     * https://github.com/psychopy/psychopy/blob/817ed9e38c6ac1d15a4beda98e916031e3bacaac/psychopy/data/staircase.py#L46
     * 
     * 
     */
    class Staircase : public Base
    {
    public:
        template <typename T>
        struct StairParams
        {
            double start_val;
            std::optional<int> n_reversals = std::nullopt;
            T step_sizes;
            int n_trials;
            int n_up;
            int n_down;
            bool apply_initial_rule;
            Scale step_type;
            std::optional<double> min_val = std::nullopt;
            std::optional<double> max_val = std::nullopt;
        };

        template <typename T>
        Staircase(const StairParams<T> &params)
        {
            settings.start_val = params.start_val;
            settings.n_trials = params.n_trials;
            settings.n_up = params.n_up;
            settings.n_down = params.n_down;
            settings.apply_initial_rule = params.apply_initial_rule;
            settings.min_val = params.min_val;
            settings.max_val = params.max_val;
            settings.step_sizes = internal::anyToVector(params.step_sizes);
            step_size = settings.step_sizes[0];

            // even if step_sizes is specified, may be of length 1
            variable_step = settings.step_sizes.size() > 1;
            next_intensity = settings.start_val; // starting point
            // reserve 10x the number of expected trials (arbitrary)
            responses.reserve(10 * settings.n_trials);
            intensities.reserve(10 * settings.n_trials);
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
         * Compute the next intensity value.
         * @return Next intensity value.
         */
        double next()
        {
            // first call to next(), easy out
            if (trial_count <= 0)
            {
                return next_intensity;
            }

            // asking for trial #2, use 1-down, 1-up rule
            bool reversal = false;
            bool initial_rule = false;
            if (!reversal_count && settings.apply_initial_rule)
            {
                initial_rule = true; // moved from later
                // correct response previously
                if (responses.end()[-1])
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
                responses.end()[-1] ? decrement() : increment();
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
            return next_intensity;
        }

        /**
         * Update the staircase with user response and (optional) intensity used, and check whether to proceed or not.
         * @param value Response made by participant (usually 0 or 1)
         * @param intensity Optional intensity of the stimulus, if different from the one
         * produced by the staircase. Otherwise, the most recent value produced by `next()` is used.
         * @return Whether to continue the staircase or not.
         */
        bool update(const int value, std::optional<double> intensity = std::nullopt)
        {
            // update history of intensity/response
            // if the user provides an intensity value, use that
            // otherwise, fill in the last generated one
            intensities.push_back(intensity ? *intensity : next_intensity);
            responses.push_back(value);

            if (value)
            {
                if (responses.size() > 1 && responses.end()[-2] == value)
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
                if (responses.size() > 1 && responses.end()[-2] == value)
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
            if (reversal_count >= settings.n_reversals && intensities.size() >= settings.n_trials)
            {
                return false;
            }
            return true;
        }

    private:
        int trial_count = 0;
        int reversal_count = 0;          // use this rather than tracking the reversal intensities
        int correct_count = 0;           //
        int current_direction = 0;       // 1 = up, -1 = down, 0 = initial
        std::vector<int> responses;      // history of user responses
        std::vector<double> intensities; // history of intensities
        double next_intensity = 0;
        bool variable_step = false;
        double step_size;

        Staircase::StairParams<std::vector<double>> settings;

        void increment()
        {
            switch (settings.step_type)
            {
            case Scale::dB:
                next_intensity *= std::pow(10.0, step_size / 20.0);
                break;
            case Scale::Log:
                next_intensity *= std::pow(10.0, step_size);
                break;
            case Scale::Linear:
                next_intensity += step_size;
                break;
            }
            if (settings.max_val)
            {
                next_intensity = std::min(next_intensity, *settings.max_val);
            }
            correct_count = 0;
        }

        void decrement()
        {
            switch (settings.step_type)
            {
            case Scale::dB:
                next_intensity /= std::pow(10.0, step_size / 20.0);
                break;
            case Scale::Log:
                next_intensity /= std::pow(10.0, step_size);
                break;
            case Scale::Linear:
                next_intensity -= step_size;
                break;
            }
            if (settings.min_val)
            {
                next_intensity = std::max(next_intensity, *settings.min_val);
            }
            correct_count = 0;
        }
    };
} // namespace psydapt

#endif
