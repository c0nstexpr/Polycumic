// Created by BlurringShadow at 2021-02-28-下午 10:45

#pragma once

#include <charconv>

#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include "utils.h"

namespace utility
{
    namespace details
    {
        struct to_string_cpo
        {
            template<typename T> requires requires(const T& t) { to_string(t); }
            [[nodiscard]] constexpr auto operator()(const T& value) const { return to_string(value); }

            template<typename T>
            [[nodiscard]] constexpr auto operator()(const T& value) const { return fmt::format("{}", value); }
        };

        template<typename T>
        struct from_string_view_obj
        {
            template<typename... Args> requires std::constructible_from<T, Args...>
            constexpr T operator()(const std::string_view str, Args&&... args) const
            {
                using std::from_chars;

                T t(std::forward<Args>(args)...);
                if(from_chars(str.data(), str.data() + str.size(), t).ec != std::errc{})
                    throw boost::bad_lexical_cast();
                return t;
            }
        };
    }

    inline constexpr details::to_string_cpo to_string;

    template<typename T>
    inline constexpr details::from_string_view_obj<T> from_string;
}
