// Created by BlurringShadow at 2021-03-03-下午 4:33

#pragma once
#include "type_list.h"

namespace polycumic::utility::traits
{
    namespace details
    {
        template<typename T>
        struct function_traits_helper;

        template<typename R, typename... Args>
        struct function_traits_helper<R(*)(Args ...)>
        {
            using result_t = R;
            using args_t = traits::type_list<Args...>;
        };
    }

    template<typename T> requires std::is_function_v<std::remove_pointer_t<std::decay_t<T>>>
    struct function_traits : details::function_traits_helper<std::decay_t<T>> {};

    template<auto Ptr> requires requires { function_traits<std::decay_t<decltype(Ptr)>>{}; }
    struct function_pointer_traits : function_traits<std::decay_t<decltype(Ptr)>> {};
}
