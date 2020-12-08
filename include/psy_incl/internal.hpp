#ifndef PSYDAPT_INTERNAL_HPP
#define PSYDAPT_INTERNAL_HPP

#include <vector>

namespace psydapt
{
    namespace internal
    {
        template <class S>
        std::vector<double> anyToVector(const S &s)
        {
            if (constexpr(std::is_arithmetic_v<S>))
            {
                return {s};
            }
            else
            {
                return std::vector<double>(s.begin(), s.end());
            }
        }
    } // namespace internal
} // namespace psydapt
#endif
