// Created by BlurringShadow at 2021-02-28-上午 10:47

#pragma once

#include "string_conversion.h"
#include "traits/member.h"

namespace nameof
{
    template<typename T>
    inline constexpr auto nameof_full_type_v = []()
    {
        if constexpr(std::is_fundamental_v<T>) return nameof_full_type<T>();
        else
        {
            constexpr auto result_sv = []
            {
                constexpr auto n = nameof_full_type<T>();
                std::size_t i = 0;
                if(n.front() == 's')
                {
                    ++i;
                    // ReSharper disable once StringLiteralTypo
                    const auto sv = "truct "sv;
                    for(std::size_t j = 0; j < sv.size(); ++j, ++i)
                        if(i == n.size() || n[i] != sv[j]) return n;
                }
                else
                {
                    const auto sv = "class "sv;
                    for(std::size_t j = 0; j < sv.size(); ++j, ++i)
                        if(i == n.size() || n[i] != sv[j]) return n;
                }
                return n.substr(i);
            }();

            std::array<char, result_sv.size() + 1> str_array{};
            for(std::size_t i = 0; i < result_sv.size(); ++i) str_array[i] = result_sv[i];
            return str_array;
        }
    }();

    namespace details
    {
        template<auto Ptr, std::size_t Size>
        struct func_name
        {
            template<typename T>
            constexpr auto operator()(const T& name) const
            {
                constexpr auto sig = nameof_full_type_v<decltype(Ptr)>;
                const std::string_view name_sv = {name.data(), name.size() - 1};
                std::array<char, Size + sig.size() - 1> full_name{};

                std::size_t i = 0;

                for(;; ++i)
                {
                    const auto c = sig[i];
                    if(c == '*') break;
                    full_name[i] = c;
                }

                for(auto value : name_sv) full_name[i++] = value;

                for(auto j = i - name_sv.size(); j < sig.size(); ++i, ++j) full_name[i] = sig[j];

                return full_name;
            }
        };
    }

    template<auto Ptr, std::size_t Size>
    inline constexpr auto func_name_v = details::func_name<Ptr, Size>{};

    namespace details
    {
        template<std::size_t Size, typename T>
        constexpr auto name_str_to_literal(const T& str)
        {
            std::array<char, Size + 1> result{};
            ranges::copy(str, result.begin());
            return result;
        }
    }

#define NAME_STR_TO_LITERAL(str) ::nameof::details::name_str_to_literal<str.size()>(str)

#define NAMEOF_FUNC(ptr) []\
    {\
        constexpr auto str = NAME_STR_TO_LITERAL(NAMEOF_FULL(ptr));\
        return ::nameof::func_name_v<ptr, str.size()>(str);\
    }()
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
        inline static const auto& all_str = as_const(str_map_) | ranges::views::values;
    };

    struct str_hash_traits : hash_traits
    {
        static constexpr auto set_str = [](const auto& str)
        {
            const std::string_view sv{str.data()};
            const auto id = to_hashed(sv);
            str_map_.emplace(id, sv);
            return id;
        };
    };

    template<typename T>
    struct type_hash_traits : str_hash_traits
    {
        inline static const auto id = set_str(nameof::nameof_full_type_v<T>);

        inline static const auto& str = str_map_[id];
    };
}

namespace polycumic::utility::reflection
{
    template<typename T>
    class type_meta
    {
        using type_hash_traits = entt::type_hash_traits<T>;

        inline static auto meta_ = entt::meta<T>().type(type_hash_traits::id);

        struct apply_meta
        {
            template<typename Arg>
            static void call() { type_meta<Arg>{}; }
        };

    public:
        static constexpr auto meta() { return meta_; }

        CPP_template(auto Ptr, typename NameT)(
            requires (traits::member_func_of<decltype(Ptr), T>)
        )
        static auto func(const NameT& name)
        {
            using func_traits = traits::member_function_pointer_traits<Ptr>;
            using involved_list_t =
                typename func_traits::args_t
                ::template apply_t<traits::type_set>
                ::template append_t<typename func_traits::result_t>;

            static const auto _ = [id = entt::str_hash_traits::set_str(name)]
            {
                involved_list_t::container::template call<apply_meta>();
                meta_.template func<Ptr>(id);
                return 0;
            }();
            return meta();
        }

        CPP_template(auto Ptr, typename NameT)(
            requires (traits::member_of<decltype(Ptr), T>)
        )
        static auto data(const NameT& name)
        {
            static const auto _ = [id = entt::str_hash_traits::set_str(name)]
            {
                type_meta<typename traits::member_pointer_traits<Ptr>::type>{};
                meta_.template data<Ptr>(id);
                return 0;
            }();
            return meta();
        }

        CPP_template(
            auto Getter,
            typename NameT,
            auto Setter = nullptr
        )(
            requires(
                traits::member_of<decltype(Getter), T> &&
                traits::member_of<
                std::conditional_t<Setter == nullptr, decltype(Getter), decltype(Setter)>, T
                > &&
                std::is_member_function_pointer_v<decltype(Getter)>)
        )
        static auto data(const NameT& name)
        {
            static const auto _ = [id = entt::str_hash_traits::set_str(name)]
            {
                type_meta<typename traits::member_pointer_traits<Getter>::type>{};

                if constexpr(Setter != nullptr)
                {
                    using func_traits = traits::member_function_pointer_traits<Setter>;
                    using involved_list_t = typename func_traits::args_t::
                        template append_t<typename func_traits::result_t>;
                    involved_list_t::template call<apply_meta>();
                }

                meta_.template data<Setter, Getter>(id);

                return 0;
            }();
            return meta();
        }
    };

#define MEMBER_FUNC(member_ptr) func<member_ptr>(NAMEOF_FUNC(member_ptr))

#define MEMBER_DATA(member_ptr) data<member_ptr>(NAME_STR_TO_LITERAL(NAMEOF(member_ptr)))
}

namespace entt
{
    namespace details
    {
        template<typename T>
        constexpr auto to_string_(const T& t) { return polycumic::utility::to_string(t); }

        template<typename T>
        constexpr auto from_string_(const std::string_view sv) { return polycumic::utility::from_string<T>(sv); }
    }

    template<typename... T>
    constexpr void register_to_string_conversion()
    {
        [[maybe_unused]] static const auto _ = []
        {
            (polycumic::utility::reflection::type_meta<T>::meta()
                .template conv<&details::to_string_<T>>(), ...);
            return 0;
        };
    }

    template<typename... T>
    constexpr void register_from_string_view_conversion()
    {
        using namespace polycumic::utility;

        [[maybe_unused]] static const auto _ = []
        {
            static auto meta_v = reflection::type_meta<std::string_view>::meta();
            // ReSharper disable once CppRedundantTemplateKeyword
            (meta_v.template conv<&details::from_string_<T>>(), ...);
            return 0;
        };
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
            polycumic::utility::reflection::type_meta<std::string>{};
            register_string_conversion<
                char, short, int, long, long long,
                unsigned char, unsigned short, unsigned, unsigned long, unsigned long long,
                float, double, long double>();
            return 0;
        }();
    }
}
