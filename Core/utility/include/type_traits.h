// Created by BlurringShadow at 2021-03-01-下午 9:00

#pragma once
#include "utils.h"

namespace polycumic::utility
{
    template<typename Object>
    using value_type = typename Object::type;

    template<typename Object>
    concept type_queryable = std::derived_from<Object, std::type_identity<value_type<Object>>> ||
    requires { typename value_type<Object>; };

    struct unique_object_trait
    {
        unique_object_trait() = default;
        unique_object_trait(const unique_object_trait&) = delete;
        unique_object_trait(unique_object_trait&&) = default;
        unique_object_trait& operator=(const unique_object_trait&) = delete;
        unique_object_trait& operator=(unique_object_trait&&) = default;
    };

    template<typename T>
    using add_const_lvalue_ref_t = std::add_lvalue_reference_t<std::add_const_t<T>>;

    template<typename T>
    concept uniquely = std::movable<T> && !std::copyable<T>;

    template<typename T>
    concept const_lvalue_ref = std::is_lvalue_reference_v<T> && std::is_const_v<T>;

    template<typename T, typename U>
    concept convertible_from = std::convertible_to<U, T>;

    template<typename T, typename U>
    concept inter_convertible = std::convertible_to<T, U> && convertible_from<T, U>;

    template<auto Left, auto Right>
    concept equal = Left == Right;
}
