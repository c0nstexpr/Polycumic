// Created by BlurringShadow at 2021-02-28-下午 10:45

#pragma once

#include <charconv>
#include "utility_core.h"

namespace polycumic::utility
{
    namespace details
    {
        struct to_string_cpo
        {
            template<typename T>
            [[nodiscard]] constexpr auto operator()(const T& t) const
            {
                return fmt::format("{}", t);
            }

#ifdef __cpp_lib_concepts
            template<typename T> requires requires(T t) { to_string(t); }
#else
            template<typename T>
            CPP_requires(has_to_string_cpo, requires(T& t)(to_string(t)));

            CPP_template(typename T)(requires(CPP_requires_ref(has_to_string_view_cpo, T)))
#endif
            [[nodiscard]] constexpr auto operator()(const T& t) const { return to_string(t); }
        };

        template<typename T>
        CPP_concept constructible_from_sv_indirectly =
            ranges::concepts::constructible_from<T, std::string_view> ||
            ranges::concepts::constructible_from<T, const char*> ||
            ranges::concepts::constructible_from<T, const char*, const char*> ||
            ranges::concepts::constructible_from<T, const char*, std::size_t>;

        enum class select_from_sv_cpo
        {
            from_chars,
            constructor,
            cpo
        };

        template<typename T>
        CPP_requires(
            has_from_string_view_cpo,
            requires(T& t, const std::string_view str)
            (
                std::is_default_constructible_v<T>,
                from_string(str, t)
            )
        );

        template<typename T>
        inline constexpr auto from_string_view_cpo_selector =
            constructible_from_sv_indirectly<T> ?
            select_from_sv_cpo::constructor :
            CPP_requires_ref(has_from_string_view_cpo, T) ?
            select_from_sv_cpo::cpo :
            select_from_sv_cpo::from_chars;

        template<typename T, select_from_sv_cpo Selector = from_string_view_cpo_selector<T>>
        struct from_string_view_func_obj;

        template<typename T>
        struct from_string_view_func_obj<T, select_from_sv_cpo::from_chars>
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

        template<typename T>
        struct from_string_view_func_obj<T, select_from_sv_cpo::constructor>
        {
            [[nodiscard]] constexpr T operator()(const std::string_view str) const
            {
                if constexpr(concepts::constructible_from<T, std::string_view>)
                    return {str};
                else if constexpr(concepts::constructible_from<T, const char*>)
                    return {str.data()};
                else if constexpr(concepts::constructible_from<T, const char*, const char*>)
                    return {str.data(), str.data() + str.size()};
                else return {str.data(), str.size()};
            }
        };

        template<typename T>
        struct from_string_view_func_obj<T, select_from_sv_cpo::cpo>
        {
            [[nodiscard]] constexpr T operator()(const std::string_view str) const
            {
                T t{};
                from_string(str, t);
                return t;
            }
        };
    }

    inline constexpr details::to_string_cpo to_string;

    template<typename T>
    inline constexpr details::from_string_view_func_obj<T> from_string;
}
