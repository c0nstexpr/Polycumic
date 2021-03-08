// Created by BlurringShadow at 2021-02-28-上午 12:04

#pragma once

#include  "traits/object.h"

namespace polycumic::utility
{
    template<typename T>
    struct value_setter : std::type_identity<T>, traits::unique_object_trait
    {
        using std::type_identity<T>::type;
        using reference_t = std::add_lvalue_reference_t<type>;

        reference_t v;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr value_setter(reference_t value) noexcept : v(value) {}

        template<typename... Args> requires std::constructible_from<T, Args...>
        constexpr void operator()(
            Args&&... args
        ) noexcept(std::is_nothrow_constructible_v<type, Args...>)
        {
            v = type(std::forward<Args>(args)...);
        }
    };

    template<typename Setter, typename T>
    concept setter_object = std::invocable<Setter, T>;

    template<typename T>
    value_setter(T&) -> value_setter<T>;
}
