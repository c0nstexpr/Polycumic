// Created by BlurringShadow at 2021-02-28-下午 10:45

#pragma once

#include <charconv>
#include "utils.h"

namespace polycumic::utility
{
    template<typename T>
    concept constructible_from_sv_indirectly =
    std::constructible_from<T, std::string_view> ||
    std::constructible_from<T, const char*> ||
    std::constructible_from<T, const char*, const char*> ||
    std::constructible_from<T, const char*, std::size_t>;

    namespace details
    {
        struct to_string_cpo
        {
            template<typename T>
            [[nodiscard]] constexpr auto operator()(const T& t) const
            {
                return fmt::format("{}", t);
            }

            template<typename T> requires requires(T t) { to_string(t); }
            [[nodiscard]] constexpr auto operator()(const T& t) const { return to_string(t); }
        };

        struct to_string_view_cpo
        {
            template<typename T>
            [[nodiscard]] constexpr auto operator()(const T& t) const
            {
                return std::string_view{t.data(), t.size()};
            }

            template<typename T> requires requires(T t) { to_string_view(t); }
            [[nodiscard]] constexpr auto operator()(const T& t) const { return to_string_view(t); }

            template<std::size_t Size>
            [[nodiscard]] constexpr auto operator()(const std::array<char, Size>& t) const
            {
                return (*this)(t.data());
            }

            template<std::size_t Size>
            [[nodiscard]] constexpr auto operator()(const char* t) const
            {
                return std::string_view{t};
            }
        };
    }

    template<typename T>
    struct from_string_view_func_obj
    {
        [[nodiscard]] constexpr T operator()(const std::string_view str) const
        {
            using std::from_chars;
            T t{};
            if(from_chars(str.data(), str.data() + str.size(), t).ec != std::errc{})
                throw std::invalid_argument("");

            return t;
        }
    };

    template<constructible_from_sv_indirectly T>
    struct from_string_view_func_obj<T>
    {
        [[nodiscard]] constexpr T operator()(const std::string_view str) const
        {
            if constexpr(std::constructible_from<T, std::string_view>)
                return {str};
            else if constexpr(std::constructible_from<T, const char*>)
                return {str.data()};
            else if constexpr(std::constructible_from<T, const char*, const char*>)
                return {str.data(), str.data() + str.size()};
            else return {str.data(), str.size()};
        }
    };

    template<typename T>
    requires std::is_default_constructible_v<T> &&
    !constructible_from_sv_indirectly<T> &&
    requires(const std::string_view str, T& t) { from_string(str, t); }
    struct from_string_view_func_obj<T>
    {
        [[nodiscard]] constexpr T operator()(const std::string_view str) const
        {
            T t{};
            from_string(str, t);
            return t;
        }
    };

    template<typename Range>
    requires !constructible_from_sv_indirectly<Range> &&
    std::is_default_constructible_v<Range> &&
    requires(const std::string_view str, Range r)
    {
        std::ranges::copy(str, r.begin());
    }
    struct from_string_view_func_obj<Range>
    {
        [[nodiscard]] constexpr auto operator()(const std::string_view str) const
        {
            Range r{};
            std::ranges::copy(str, r.begin());

            return r;
        }
    };

    inline constexpr details::to_string_cpo to_string;

    inline constexpr details::to_string_view_cpo to_string_view;

    template<typename T>
    inline constexpr from_string_view_func_obj<T> from_string;
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
            register_string_conversion<
                char, short, int, long, long long,
                unsigned char, unsigned short, unsigned, unsigned long, unsigned long long,
                float, double, long double>();
            return 0;
        }();
    }
}
