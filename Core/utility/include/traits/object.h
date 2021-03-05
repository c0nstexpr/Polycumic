// Created by BlurringShadow at 2021-03-03-下午 4:36

#pragma once

#include "function.h"

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

    template<typename>
    struct member_traits;

    namespace details
    {
        struct shadowed_void {};
    }

    template<typename T, typename ClassT>
    struct member_traits<T(ClassT::*)> : std::type_identity<
            std::conditional_t<std::same_as<T, details::shadowed_void>, void, T>
        >
    {
        using class_t = ClassT;
    };

    template<auto Ptr>
    struct member_pointer_traits : member_traits<std::decay_t<decltype(Ptr)>> {};

    template<typename>
    struct member_function_traits;

    namespace details
    {
        template<typename R, typename ClassT, typename... Args>
        struct member_function_base_traits :
            member_traits<std::conditional_t<std::same_as<R, void>, shadowed_void,
                std::remove_reference_t<R>>(ClassT::*)>,
            function_traits<R(*)(Args ...)> {};
    }

    enum class member_ref_qualifier
    {
        none,
        lvalue,
        rvalue
    };

    template<bool IsConst, bool IsVolatile, member_ref_qualifier RefType>
    struct member_qualifier_traits
    {
        static constexpr auto is_const = IsConst;
        static constexpr auto is_volatile = IsVolatile;
        static constexpr member_ref_qualifier ref_type = RefType;
    };

    template<typename R, typename ClassT, typename... Args>
    struct member_function_traits<R(ClassT::*)(Args ...)> :
        details::member_function_base_traits<R, ClassT, Args...>
    {
        static constexpr auto is_const = false;
    };

    template<typename R, typename ClassT, typename... Args>
    struct member_function_traits<R(ClassT::*)(Args ...) const> : 
        details::member_function_base_traits<R, ClassT, Args...>
    {
        static constexpr auto is_const = true;
    };

    template<auto Ptr> requires std::is_member_function_pointer_v<decltype(Ptr)>
    struct member_function_pointer_traits : member_function_traits<std::decay_t<decltype(Ptr)>> {};

    template<typename T, typename ClassT>
    concept member_of = std::same_as<typename member_traits<T>::class_t, ClassT>;
}
