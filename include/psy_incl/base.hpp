
#ifndef PSYDAPT_BASE_HPP
#define PSYDAPT_BASE_HPP

#include <optional>

namespace psydapt
{
    enum class Scale
    {
        dB,
        Linear,
        Log
    };

    class Base
    {
    public:
        virtual double next() = 0;
        virtual bool update(int response, std::optional<double> intensity = std::nullopt) = 0;
    };
} // namespace psydapt
#endif
