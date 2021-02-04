#ifndef PSYDAPT_COMMON_TEST_HPP
#define PSYDAPT_COMMON_TEST_HPP

#include <vector>
#include <optional>

std::vector<int> makeBasicResponseCycles(int cycles, int n_correct, int n_incorrect,
                                         std::optional<int> length = std::nullopt)
{
    std::vector<int> out;
    int raw_len = n_correct * n_incorrect * cycles;
    out.reserve(length ? *length : raw_len);
    for (int j = 0; j < cycles; j++)
    {

        for (int i = 0; i < n_correct; i++)
        {
            out.push_back(1);
        }
        for (int i = 0; i < n_incorrect; i++)
        {
            out.push_back(0);
        }
    }
    if (length && *length < raw_len)
    {
        out.erase(out.begin() + *length, out.end());
    }
    return out;
}

#endif
