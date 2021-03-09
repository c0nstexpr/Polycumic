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

    template<typename T, std::size_t Size>
    struct array_literal : std::array<T, Size>
    {
        using base = std::array<T, Size>;
        using base::at;
        using base::back;
        using base::base;
        using base::begin;
        using base::cbegin;
        using base::cend;
        using base::crbegin;
        using base::crend;
        using base::empty;
        using base::end;
        using base::fill;
        using base::front;
        using base::rbegin;
        using base::rend;
        using base::size;
        using base::swap;

        constexpr explicit(false) array_literal(const base& base) : base(base) {}

        constexpr explicit(false) array_literal(const T (&a)[Size]) { std::ranges::copy(a, a + Size, begin()); }
    };

    namespace literals
    {
        template<array_literal str>
        constexpr auto operator""_str_literal() { return str; }
    }

    template<typename T>
    concept str_literal = std::ranges::contiguous_range<T> &&
    std::ranges::sized_range<T> &&
    std::same_as<std::ranges::range_value_t<T>, char>;
}
