#ifndef PSYDAPT_BASE_HPP
#define PSYDAPT_BASE_HPP
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

#include <optional>
#include <array>
#include <vector>

/** @file
 * @brief Class @ref psydapt::Base, enum @ref psydapt::Scale 
 */
namespace psydapt
{
    /** @brief Stimulus scale */
    enum class Scale
    {
        dB,
        Linear,
        Log10
    };

    /**
     * @brief Base
     * 
     * All adaptive procedures are derived from here, which defines the basic interface.
     */
    template <class T, std::size_t DimStim = 1>
    class Base
    {

    protected:
        typedef std::conditional_t<(DimStim > 1), std::array<double, DimStim>, double> stim_type;
        std::vector<int> response_history;
        std::vector<stim_type> stimulus_history;
        stim_type next_stimulus; // if stimulus not passed in update, use this
        bool should_continue = true;

    public:
        /** @brief Generate the next stimulus (or stimuli) */
        stim_type next()
        {
            return static_cast<T *>(this)->next();
        }
        /** @brief Update the state of the adaptive procedure
         * @param value Response made by participant (usually 0 or 1)
         * @param stimulus Optional value of the stimulus, if different from the one
         * produced by the procedure.
         * 
         * @return Whether to continue the procedure or not.
        */
        bool update(int response, const std::optional<stim_type> stimulus = std::nullopt)
        {
            return static_cast<T *>(this)->update(response, stimulus);
        }
    };
} // namespace psydapt
#endif
