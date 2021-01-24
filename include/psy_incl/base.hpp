
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

    template <typename T = double>
    class Base
    {
    public:
        virtual T next() = 0;
        virtual bool update(int response, std::optional<T> intensity = std::nullopt) = 0;
    };
} // namespace psydapt
#endif
