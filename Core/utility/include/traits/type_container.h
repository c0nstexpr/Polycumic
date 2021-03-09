// Created by BlurringShadow at 2021-03-03-下午 4:35

#pragma once

#include "integer_seqence.h"

namespace polycumic::utility::traits
{
    namespace details
    {
        template<typename... T>
        struct type_list
        {
            static constexpr auto size = sizeof...(T);

            template<std::size_t I, typename>
            struct indexed_type
            {
                // ReSharper disable once CppFunctionIsNotImplemented
                static constexpr indexed_type get_from_index(
                    std::integral_constant<std::size_t, I>
                ) noexcept;
            };

            template<typename Find>
            static constexpr std::array<bool, size> type_math_result()
            {
                if constexpr(size == 0) return {};
                else return {std::same_as<Find, T>...};
            }

            template<typename Find>
            static constexpr auto index_of = []
            {
                constexpr auto query = type_math_result<Find>();

                return static_cast<std::size_t>(
                    std::ranges::find(query, true) - query.cbegin()
                );
            }();

            template<typename Find>
            static constexpr auto count = std::ranges::count(type_math_result<Find>(), true);

            template<typename Find>
            static constexpr auto contains = count<Find> != 0;

            template<std::size_t... I>
            struct indexed : private indexed_type<I, T>...
            {
                static constexpr auto size = type_list::size;

                using indexed_type<I, T>::get_from_index...;

            private:
                // ReSharper disable once CppFunctionIsNotImplemented
                template<std::size_t Index, typename MatchT>
                static constexpr MatchT type_extractor(indexed_type<Index, MatchT>);

            public:
                template<std::size_t Index>
                using indexed_t = decltype(type_extractor(
                    get_from_index(std::integral_constant<std::size_t, Index>{})
                ));

                // ReSharper disable once CppRedundantQualifier
                template<typename Find>
                static constexpr std::size_t type_index = type_list::template index_of<Find>;

                template<typename Find>
                static constexpr std::size_t count = type_list::template count<Find>;

                template<typename Find>
                static constexpr std::size_t contains = type_list::template contains<Find>;

                template<typename Func>
                static constexpr auto call() noexcept(noexcept((Func::template call<T>(), ...)))
                {
                    (Func::template call<T>(), ...);
                }

                template<template<typename...> typename Type>
                using apply_t = Type<T...>;
            };
        };
    }

    template<typename... Types>
    struct type_container : apply_index_sequence_t<
            details::type_list<Types...>::template indexed,
            index_sequence_for_v<Types...>
        >
    {
        using base = apply_index_sequence_t<
            details::type_list<Types...>::template indexed,
            index_sequence_for_v<Types...>
        >;

        template<std::size_t... OtherInt>
        using sequence_indexed_t = type_container<typename base::template indexed_t<OtherInt>...>;

        template<typename Other>
        using append_t = type_container<Types..., Other>;

        template<typename Other>
        using append_front_t = type_container<Other, Types...>;

        template<std::size_t Size, std::size_t From = 0>
        using select_range_indexed_t = apply_index_sequence_t<
            sequence_indexed_t,
            make_index_sequence_v<Size, From>
        >;

        template<std::size_t Index, typename Other>
        using insert_t = typename select_range_indexed_t<Index>
        ::template append_t<Other>
        ::template apply_t<
            select_range_indexed_t<base::size - Index, Index>::template append_front_t
        >;

        using rest_t = select_range_indexed_t<base::size - 1>;
    };

    template<>
    struct type_container<> : apply_index_sequence_t<
            details::type_list<>::indexed,
            index_sequence_for_v<>
        >
    {
        using base = apply_index_sequence_t<
            details::type_list<>::indexed,
            index_sequence_for_v<>
        >;

        template<std::size_t... OtherInt>
        using sequence_indexed_t = type_container<indexed_t<OtherInt>...>;

        template<typename Other>
        using append_t = type_container<Other>;

        template<typename Other>
        using append_front_t = type_container<Other>;

        template<std::size_t Size, std::size_t From = 0>
        requires requires { Size == 0; }
        using select_range_indexed_t = type_container;

        using rest_t = type_container;

        using unique_t = type_container;
    };

    using empty_type_list = type_container<>;

    namespace details
    {
        template<typename... Types>
        struct type_set_builder
        {
            using container = type_container<Types...>;

            static constexpr std::pair<std::array<std::size_t, container::size>, std::size_t>
            filtered_result = []
            {
                if constexpr(container::size == 0) return {};
                else
                {
                    constexpr std::array indices{container::template type_index<Types>...};
                    std::array<std::size_t, container::size> filtered{};
                    std::size_t valid_size = 1;
                    for(std::size_t j = 1; j < indices.size(); ++j)
                    {
                        const auto index = indices[j];
                        if(index > filtered[valid_size - 1])
                        {
                            filtered[valid_size] = index;
                            ++valid_size;
                        }
                    }

                    return std::pair{filtered, valid_size};
                }
            }();

            template<std::size_t... I>
            using selector =
            type_container<typename container::template indexed_t<filtered_result.first[I]>...>;

            using filtered_t = apply_sequence_t<
                std::size_t,
                selector,
                make_index_sequence_v<filtered_result.second>
            >;
        };
    }

    template<typename... Types>
    struct type_set
    {
        using container = typename details::type_set_builder<Types...>::filtered_t;

        template<typename Other>
        using append_t = std::conditional_t<
            container::template contains<Other>,
            type_set,
            type_set<Types..., Other>
        >;
    };
}
