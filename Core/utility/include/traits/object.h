// Created by BlurringShadow at 2021-03-03-下午 4:36

#pragma once

#include "utils.h"

namespace polycumic::utility::traits
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
    concept uniquely = std::movable<T> && !std::copyable<T>;
}
