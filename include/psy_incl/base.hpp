
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

    template <std::size_t DimStim = 1,
              typename T = std::conditional_t<(DimStim > 1),
                                              std::array<double, DimStim>,
                                              double>>
    class Base
    {
    public:
        virtual T next() = 0;
        virtual bool update(int response,
                            const std::optional<T> intensity = std::nullopt) = 0;

    protected:
        typedef T stim_type;
    };
} // namespace psydapt
#endif
