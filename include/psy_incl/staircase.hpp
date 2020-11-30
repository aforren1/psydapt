
#ifndef PSYDAPT_STAIRCASE_HPP
#define PSYDAPT_STAIRCASE_HPP

#include <vector>

#include "base.hpp"

namespace psydapt
{
    class Staircase : public Base
    {
    public:
        struct StairParams
        {
            double start_val;
            int n_reversals;
            int step_sizes;
            int n_trials;
            int n_up;
            int n_down;
            bool apply_initial_rule;
            Scale step_type;
            double min_val;
            double max_val;
        };
        Staircase(const StairParams &params)
        {
            settings = params;
            // reserve space ahead of time
            responses.reserve(settings.n_trials);
            intensities.reserve(settings.n_trials);
        }
        /**
     * Compute the next intensity value.
     * @return Next intensity value.
     */
        double next()
        {
            return 0;
        }

        /**
     * Update the staircase with user response and (optional) intensity used.
     * @param value Response made by participant (usually 0 or 1)
     * @param intensity Optional intensity of the stimulus, if different from the one
     * produced by the staircase. Otherwise, the most recent value produced by `next()` is used.
     * @return Whether to continue the staircase or not.
     */
        bool update(const int value, std::optional<double> intensity)
        {
            bool proceed = true;
            // update history of intensity/response
            // if the user provides an intensity value, use that
            // otherwise, fill in the last generated one
            if (intensity)
            {
                intensities.push_back(*intensity);
            }
            else
            {
                intensities.push_back(last_intensity);
            }
            responses.push_back(value);
            // check termination condition
        }

    private:
        StairParams settings;
        int trial_count = 0;
        int reversal_count = 0;
        int correct_count = 0;
        int current_direction = 0;       // 1 = up, -1 = down, 0 = initial
        std::vector<int> responses;      // history of user responses
        std::vector<double> intensities; // history of intensities
        double last_intensity = 0;
    };
} // namespace psydapt

#endif
