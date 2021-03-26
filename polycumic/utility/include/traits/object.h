// Created by BlurringShadow at 2021-03-03-下午 4:36

#pragma once

#include "utility_core.h"

namespace polycumic::utility::traits
{
    template<typename Object>
    using value_type = typename Object::type;

    namespace details
    {
        template<typename Object>
        CPP_requires(type_queryable, requires(boost::type_identity_t<Object>)(true));
    }

    template<typename Object>
    CPP_concept type_queryable = CPP_requires_ref(details::type_queryable, Object);

    struct unique_object_trait
    {
        unique_object_trait() = default;

        unique_object_trait(const unique_object_trait&) = delete;

        unique_object_trait(unique_object_trait&&) = default;

        unique_object_trait& operator=(const unique_object_trait&) = delete;

        unique_object_trait& operator=(unique_object_trait&&) = default;
    };

    template<typename T>
    CPP_concept uniquely = concepts::movable<T> && !concepts::copyable<T>;
}
