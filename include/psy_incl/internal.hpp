#ifndef PSYDAPT_INTERNAL_HPP
#define PSYDAPT_INTERNAL_HPP

#include <vector>

namespace psydapt
{
    namespace internal
    {
        template <typename T>
        std::vector<double> anyToVector(const T t)
        {
            return {t};
        }

        template <typename T>
        std::vector<double> anyToVector(const std::vector<T> &t)
        {
            return std::vector<double>(t.begin(), t.end());
        }
    } // namespace internal
} // namespace psydapt
#endif
