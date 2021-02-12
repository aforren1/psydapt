#ifndef PSYDAPT_COMMON_TEST_HPP
#define PSYDAPT_COMMON_TEST_HPP

#include <vector>
#include <optional>

std::vector<int> makeBasicResponseCycles(int cycles, int n_correct, int n_incorrect,
                                         std::optional<int> length = std::nullopt);

#endif
