// Created by BlurringShadow at 2021-02-28-上午 12:04

#pragma once

#include "setter.h"
#include  "traits/object.h"

namespace polycumic::utility
{
    template<typename T>
    struct value_setter : boost::type_identity<T>, traits::unique_object_trait
    {
        using boost::type_identity<T>::type;
        using reference_t = std::add_lvalue_reference_t<type>;

        reference_t v;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr value_setter(reference_t value) noexcept : v(value) {}

        CPP_template(typename... Args)(requires(concepts::constructible_from<T, Args...>))
        constexpr void operator()(
            Args&&... args
        ) noexcept(std::is_nothrow_constructible_v<type, Args...>)
        {
            v = type(std::forward<Args>(args)...);
        }
    };

    template<typename T>
    value_setter(T&) -> value_setter<T>;
}
