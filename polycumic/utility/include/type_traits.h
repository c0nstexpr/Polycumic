// Created by BlurringShadow at 2021-03-01-下午 9:00

#pragma once

#include "utility_core.h"

namespace polycumic::utility
{
    template<typename T>
    using add_const_lvalue_ref_t = std::add_lvalue_reference_t<std::add_const_t<T>>;

    template<typename T>
    CPP_concept const_lvalue_ref = std::is_lvalue_reference_v<T> && std::is_const_v<T>;

    template<typename T, typename U>
    CPP_concept convertible_from = concepts::convertible_to<U, T>;

    template<typename T, typename U>
    CPP_concept inter_convertible = concepts::convertible_to<T, U> && convertible_from<T, U>;

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

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr array_literal(const base& base) : base(base) {}

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr array_literal(const T (&a)[Size]) { ranges::copy(a, a + Size, begin()); }
    };
}
