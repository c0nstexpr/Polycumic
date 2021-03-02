// Created by BlurringShadow at 2021-03-02-上午 12:06

#pragma once
#include "utils.h"

namespace polycumic::utility
{
    inline constexpr auto equal_to = std::ranges::equal_to{};
    inline constexpr auto not_equal_to = std::ranges::not_equal_to{};
    inline constexpr auto less = std::ranges::less{};
    inline constexpr auto greater = std::ranges::greater{};
    inline constexpr auto less_equal = std::ranges::less_equal{};
    inline constexpr auto greater_equal = std::ranges::greater_equal{};

    inline constexpr auto plus = std::plus<>{};
    inline constexpr auto minus = std::minus<>{};
    inline constexpr auto divides = std::divides<>{};
    inline constexpr auto multiplies = std::multiplies<>{};
    inline constexpr auto modulus = std::modulus<>{};
    inline constexpr auto negate = std::negate<>{};

    inline constexpr auto logical_and = std::logical_and<>{};
    inline constexpr auto logical_not = std::logical_not<>{};
    inline constexpr auto logical_or = std::logical_or<>{};

    inline constexpr auto bit_and = std::bit_and<>{};
    inline constexpr auto bit_not = std::bit_not<>{};
    inline constexpr auto bit_or = std::bit_or<>{};
    inline constexpr auto bit_xor = std::bit_xor<>{};
}
