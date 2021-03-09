// Created by BlurringShadow at 2021-02-28-上午 10:47

#pragma once

#include "traits/member.h"

namespace nameof
{
    template<typename T>
    inline constexpr std::string_view nameof_full_type_v = []
    {
        constexpr std::string_view n = nameof_full_type<T>();

        std::size_t i = 0;
        if constexpr(n.front() == 's')
        {
            ++i;
            // ReSharper disable once StringLiteralTypo
            for(const auto c : "truct ")
            {
                if(c == 0) break;
                if(i == n.size() || n[i++] != c)
                    return n;
            }
        }
        else
            for(const auto c : "class ")
            {
                if(c == 0) break;
                if(i == n.size() || n[i++] != c) return n;
            }
        return n.substr(i);
    }();

    template<auto Ptr, polycumic::utility::str_literal auto Name>
    inline constexpr auto func_name_v = []()
    {
        constexpr auto sig = nameof_full_type_v<decltype(Ptr)>;
        constexpr std::string_view name = {Name.data(), Name.size() - 1};
        std::array<char, name.size() + sig.size() + 1> full_name{};

        std::size_t i = 0;

        for(;; ++i)
        {
            const auto c = sig[i];
            if(c == '*') break;
            full_name[i] = c;
        }

        for(auto value : name) full_name[i++] = value;

        for(auto j = i - name.size(); j < sig.size(); ++i, ++j) full_name[i] = sig[j];

        return full_name;
    }();

    namespace details
    {
        template<std::size_t Size, std::ranges::range T>
        requires std::same_as<std::ranges::range_value_t<T>, char>
        constexpr auto name_str_to_literal(const T& str)
        {
            std::array<char, Size + 1> result{};
            std::ranges::copy(str, result.begin());
            return result;
        }
    }

#define NAME_STR_TO_LITERAL(str) ::nameof::details::name_str_to_literal<str.size()>(str)

#define NAMEOF_FUNC(ptr) ::nameof::func_name_v<ptr, NAME_STR_TO_LITERAL(NAMEOF_FULL(ptr))>
}

namespace entt
{
    constexpr auto to_hashed(const std::string_view& str)
    {
        return hashed_string(str.data()).value();
    }

    struct hash_traits
    {
        using id_t = hashed_string::hash_type;

        static auto find_str(const id_t id)
        {
            const auto& it = str_map_.find(id);
            return it != str_map_.cend() ? std::optional{it->second} : std::nullopt;
        }

    protected:
        inline static std::unordered_map<id_t, std::string_view> str_map_;

    public:
        inline static const auto& all_str = as_const(str_map_) | std::ranges::views::values;
    };

    template<polycumic::utility::str_literal auto Str>
    struct str_hash_traits : hash_traits
    {
        static constexpr auto id = to_hashed({Str.data(), Str.size() - 1});

        inline static const auto& str = []
        {
            str_map_[id] = {Str.data(), Str.size() - 1};
            return Str;
        }();
    };

    template<typename T>
    struct type_hash_traits :
        str_hash_traits<NAME_STR_TO_LITERAL(nameof::nameof_full_type_v<T>)> {};
}

namespace polycumic::utility::reflection
{
    template<typename T>
    class type_meta
    {
        inline static auto meta_ = entt::meta<T>().type(entt::type_hash_traits<T>::id);

        struct apply_meta
        {
            template<typename Arg>
            static void call() { type_meta<Arg>{}; }
        };

    public:
        static constexpr auto meta() { return meta_; }

        // Avoid MSVC bug
        template</*traits::member_func_of<T>*/ auto Ptr, str_literal auto Name>
        requires traits::member_func_of<decltype(Ptr), T>
        static auto func()
        {
            static const auto _ = []
            {
                using func_traits = traits::member_function_pointer_traits<Ptr>;
                using involved_list_t = 
                    typename func_traits::args_t
                    ::template apply_t<traits::type_set>
                    ::template append_t<typename func_traits::result_t>;
                involved_list_t::container::template call<apply_meta>();
                meta_.template func<Ptr>(entt::str_hash_traits<Name>::id);
                return 0;
            }();
            return meta();
        }

        template</*traits::member_of<T>*/ auto Ptr, str_literal auto Name>
        requires traits::member_of<decltype(Ptr), T>
        static auto data()
        {
            static const auto _ = []
            {
                type_meta<typename traits::member_pointer_traits<Ptr>::type>{};
                meta_.template data<Ptr>(entt::str_hash_traits<Name>::id);
                return 0;
            }();
            return meta();
        }

        template<
            traits::member_func_of<T> auto Getter,
            str_literal auto Name,
            auto Setter = nullptr
        >
        requires (Setter == nullptr || traits::member_func_of<Setter, T>)
        static auto data()
        {
            static const auto _ = []
            {
                type_meta<typename traits::member_pointer_traits<Getter>::type>{};

                if constexpr(Setter != nullptr)
                {
                    using func_traits = traits::member_function_pointer_traits<Setter>;
                    using involved_list_t = typename func_traits::args_t::
                        template append_t<typename func_traits::result_t>;
                    involved_list_t::template call<apply_meta>();
                }

                meta_.template data<Setter, Getter>(entt::str_hash_traits<Name>::id);

                return 0;
            }();
            return meta();
        }
    };

#define MEMBER_FUNC(member_ptr) func<member_ptr, NAMEOF_FUNC(member_ptr)>()

#define MEMBER_DATA(member_ptr) data<member_ptr, NAME_STR_TO_LITERAL(NAMEOF(member_ptr))>()
}
