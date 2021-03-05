// Created by BlurringShadow at 2021-02-28-下午 10:45

#pragma once

#include <charconv>

#include <boost/lexical_cast/bad_lexical_cast.hpp>
#include "utils.h"

namespace polycumic::utility
{
    namespace details
    {
        struct to_string_cpo
        {
            template<typename T> requires requires(const T& t) { to_string(t); }
            [[nodiscard]] constexpr auto operator()(const T& value) const
            {
                return to_string(value);
            }

            template<typename T>
            [[nodiscard]] constexpr auto operator()(const T& value) const
            {
                return fmt::format("{}", value);
            }
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

    template<typename T> requires requires(T t) { std::string_view{t.data(), t.size()}; }
    constexpr std::string_view to_string_view(T& t) { return {t.data(), t.size()}; }

    template<typename T>
    inline constexpr details::from_string_view_obj<T> from_string;

    template<>
    inline constexpr auto from_string<frozen::string> = [](const std::string_view str)
    {
        return frozen::string{str.data(), str.size()};
    };
}

namespace entt
{
    template<typename... T>
    constexpr void register_to_string_conversion()
    {
        (meta<T>().template conv<polycumic::utility::to_string>(), ...);
    }

    template<typename... T>
    constexpr void register_from_string_view_conversion()
    {
        auto meta_v = meta<std::string_view>();
        (meta_v.conv<polycumic::utility::from_string<T>>(), ...);
    }

    template<typename... T>
    constexpr void register_string_conversion()
    {
        register_to_string_conversion<T...>();
        register_from_string_view_conversion<T...>();
    }

    namespace details
    {
        [[maybe_unused]] inline static const auto _ = []
        {
            register_string_conversion<char, short, int, long, long long,
                unsigned char, unsigned short, unsigned, unsigned long, unsigned long long,
                float, double, long double>();
            return 0;
        }();
    }
}
