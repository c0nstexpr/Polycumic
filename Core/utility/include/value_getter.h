// Created by BlurringShadow at 2021-03-01-下午 9:06

#pragma once
#include "type_traits.h"

namespace polycumic::utility
{
    template<typename T, bool ReturnByValue = std::is_scalar_v<T>>
    struct value_getter : std::type_identity<T>
    {
        using base = std::type_identity<T>;
        using base::base;
        using base::type;
        using const_reference_t = add_const_lvalue_ref_t<type>;
        using result_t = std::conditional_t<ReturnByValue, typename base::type, const_reference_t>;

        const_reference_t v;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr value_getter(const_reference_t value) noexcept : v(value) {}

        constexpr result_t operator()() const { return v; }

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr operator result_t() const { return v; }
    };

    template<typename Getter>
    concept getter_object = type_queryable<Getter> &&
    inter_convertible<std::invoke_result_t<std::add_const_t<Getter>>, value_type<Getter>> &&
    inter_convertible<std::add_const_t<Getter>, value_type<Getter>> && 
    requires { typename Getter::result_t; };

    template<typename T>
    value_getter(const T&) -> value_getter<T>;

    template<typename T>
    using getter_result_t = typename T::result_t;
}
