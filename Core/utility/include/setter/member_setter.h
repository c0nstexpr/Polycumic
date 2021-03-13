// Created by BlurringShadow at 2021-03-11-下午 4:30

#pragma once

#include "traits/member.h"
#include "traits/object.h"

namespace polycumic::utility
{
    template<typename T, auto MemberPtr>
    requires requires { traits::member_function_pointer_traits<MemberPtr>{}; }
    class member_setter : public std::type_identity<T>, public traits::unique_object_trait
    {
        using traits = traits::member_function_pointer_traits<MemberPtr>;
        using class_t = typename traits::class_t;
        using std::type_identity<T>::type;

        static constexpr auto setter_ = std::mem_fn(MemberPtr);

        class_t& instance_;

        template<typename... Args>
        static constexpr auto is_emplace_setter_ = 
            std::is_invocable_v<decltype(setter_), class_t&, Args...>;

    public:
        explicit constexpr member_setter(class_t& instance) : instance_(instance) {}

        template<typename... Args> requires !is_emplace_setter_<Args...> && std::constructible_from<T, Args...>
        constexpr void operator()(
            Args&&... args
        ) noexcept(noexcept(setter_(instance_, type(std::forward<Args>(args)...))))
        {
            setter_(instance_, type(std::forward<Args>(args)...));
        }

        template<typename... Args> requires is_emplace_setter_<Args...>
        constexpr void operator()(
            Args&&... args
        ) noexcept(noexcept(setter_(instance_, std::forward<Args>(args)...)))
        {
            setter_(instance_, std::forward<Args>(args)...);
        }
    };
}
