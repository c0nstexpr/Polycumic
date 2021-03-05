// Created by BlurringShadow at 2021-03-01-下午 9:00

#pragma once

#include "utils.h"

namespace polycumic::utility
{
    template<typename T>
    using add_const_lvalue_ref_t = std::add_lvalue_reference_t<std::add_const_t<T>>;

    template<typename T>
    concept const_lvalue_ref = std::is_lvalue_reference_v<T> && std::is_const_v<T>;

    template<typename T, typename U>
    concept convertible_from = std::convertible_to<U, T>;

    template<typename T, typename U>
    concept inter_convertible = std::convertible_to<T, U> && convertible_from<T, U>;

    template<typename T>
    concept str_literal = std::ranges::contiguous_range<T> && 
    std::ranges::sized_range<T> &&
    std::same_as<std::ranges::range_value_t<T>, char>;
}
