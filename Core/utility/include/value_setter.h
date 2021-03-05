// Created by BlurringShadow at 2021-02-28-上午 12:04

#pragma once

#include  "traits/object.h"

namespace polycumic::utility
{
    template<std::swappable T>
    struct value_setter : std::type_identity<T>
    {
        using base = std::type_identity<T>;
        using base::base;
        using base::type;
        using reference_t = std::add_lvalue_reference_t<type>;

        reference_t v;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr value_setter(reference_t value) noexcept : v(value) {}

        template<typename... Args> requires std::constructible_from<T, Args...>
        constexpr void operator()(Args&&... args) { v = base::type(std::forward<Args>(args)...); }
    };

    template<typename Setter>
    concept setter_object = traits::type_queryable<Setter> && std::invocable<Setter, traits::value_type<Setter>>;

    template<typename T>
    value_setter(T&) -> value_setter<T>;
}
