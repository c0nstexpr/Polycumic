// Created by BlurringShadow at 2021-02-28-上午 10:47

#pragma once

#include "string_conversion.h"

namespace entt
{
    template<typename... T>
    constexpr void register_to_string_conversion()
    {
        (meta<T>().template conv<utility::to_string>(), ...);
    }

    template<typename... T>
    constexpr void register_from_string_view_conversion()
    {
        auto meta_v = meta<std::string_view>();
        (meta_v.conv<utility::from_string<T>>(), ...);
    }

    template<typename... T>
    constexpr void register_string_conversion()
    {
        register_to_string_conversion<T...>();
        register_from_string_view_conversion<T...>();
    }

    namespace detail
    {
        [[maybe_unused]] inline static const auto _ = []
        {
            register_string_conversion<char, short, int, long, long long,
                unsigned char, unsigned short, unsigned, unsigned long, unsigned long long,
                float, double, long double>();
            return 0;
        }();
    }

    template<typename StringT>
    constexpr auto to_hashed(const StringT& str) { return operator ""_hs(str.data(), 0); }
}

namespace nameof
{
    template<std::size_t Size>
    constexpr auto get_func_name(const std::string_view name, const std::string_view sig)
    {
        std::array<char, Size + 1> full_name{};

        std::size_t i = 0;
        for(;; ++i)
        {
            const auto c = sig[i];
            if(c == '*') break;
            full_name[i] = c;
        }

        for(auto value : name)
        {
            full_name[i] = value;
            ++i;
        }

        for(auto j = i - name.size(); j < sig.size(); ++i, ++j) full_name[i] = sig[j];

        return full_name;
    }
}

namespace utility::reflection
{
    template<typename T>
    concept class_type = std::is_class_v<T>;

    template<class_type T>
    [[maybe_unused]] static inline auto type = entt::meta<T>()
        .type(entt::to_hashed(nameof::nameof_type<T>()));

#define MEMBER_FUNC_NAME(member_ptr)\
    []\
    {\
        static constexpr auto name = NAMEOF_FULL(member_ptr);\
        static constexpr auto sig = NAMEOF_FULL_TYPE_EXPR(member_ptr);\
        static constexpr auto func_name = nameof::get_func_name<name.size() + sig.size()>(name, sig);\
        return func_name;\
    }()

#define MEMBER_FUNC(member_ptr) func<member_ptr>(entt::to_hashed(MEMBER_FUNC_NAME(member_ptr)))

    // use identity to escape ReSharper wrong errors
#define MEMBER_VAR(member_ptr) data<member_ptr>(entt::to_hashed([]\
    {\
        static constexpr auto v = NAMEOF(member_ptr);\
        return std::identity{}(v);\
    }()))
}
