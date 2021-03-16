// Created by BlurringShadow at 2021-03-05-下午 11:53

#pragma once

#include "function.h"

namespace polycumic::utility::traits
{
    template<typename>
    struct member_traits;

    namespace details
    {
        struct shadowed_void {};
    }

    template<typename T, typename ClassT>
    struct member_traits<T(ClassT::*)> :
        boost::type_identity<std::conditional_t<concepts::same_as<T, details::shadowed_void>, void, T>>
    {
        using class_t = ClassT;
    };

    template<auto Ptr>
    struct member_pointer_traits : member_traits<std::decay_t<decltype(Ptr)>> {};

    template<auto Ptr>
    using member_t = typename member_pointer_traits<Ptr>::type;

    template<typename>
    struct member_function_traits;

    namespace details
    {
        template<typename R, typename ClassT, typename... Args>
        struct member_function_base_traits :
            member_traits<std::conditional_t<concepts::same_as<R, void>, shadowed_void,
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

#define UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS(is_const, is_volatile, ref_type, qualifiers) \
template<typename R, typename ClassT, typename... Args>                                    \
struct member_function_traits<R(ClassT::*)(Args ...) qualifiers> :                         \
    details::member_function_base_traits<R, ClassT, Args...>,                              \
    member_qualifier_traits<is_const, is_volatile, member_ref_qualifier::ref_type> {};     \

#define UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_CONST_PACK(is_volatile, ref_type, qualifiers) \
UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS(true, is_volatile, ref_type, const qualifiers)        \
UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS(false, is_volatile, ref_type, qualifiers)             \

#define UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_VOLATILE_PACK(ref_type, qualifiers)      \
 UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_CONST_PACK(true, ref_type, volatile qualifiers) \
 UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_CONST_PACK(false, ref_type, qualifiers)         \

#define UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_REF_PACK          \
UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_VOLATILE_PACK(none, )     \
UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_VOLATILE_PACK(lvalue, &)  \
UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_VOLATILE_PACK(rvalue, &&) \

    UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_REF_PACK

#undef UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_REF_PACK
#undef UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_VOLATILE_PACK
#undef UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS_CONST_PACK
#undef UTILITY_TRAITS_MEMBER_FUNCTION_TRAITS

    template<auto Ptr>
    struct member_function_pointer_traits : member_function_traits<std::decay_t<decltype(Ptr)>> {};

    template<typename T, typename ClassT>
    CPP_concept member_of = concepts::same_as<typename member_traits<T>::class_t, ClassT>;

    template<typename T, typename ClassT>
    CPP_concept member_func_of = std::is_member_pointer_v<T> && member_of<T, ClassT>;
}
