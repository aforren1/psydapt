
#ifndef PSYDAPT_BASE_HPP
#define PSYDAPT_BASE_HPP

#include <optional>

namespace psydapt
{
    enum class Scale
    {
        dB,
        Linear,
        Log10
    };

    template <std::size_t DimStim = 1>
    class Base
    {

    protected:
        typedef std::conditional_t<(DimStim > 1), std::array<double, DimStim>, double> stim_type;

    public:
        virtual stim_type next() = 0;
        virtual bool update(int response,
                            const std::optional<stim_type> intensity = std::nullopt) = 0;
    };
} // namespace psydapt
#endif
