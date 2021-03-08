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
            template<std::size_t, typename>
            struct indexed_type {};

            template<std::size_t... I>
            struct indexed : indexed_type<I, T>...
            {
                template<std::size_t Index>
                using indexed_t = std::decay_t<decltype(
                    []<typename MatchT>(indexed_type<Index, MatchT>&)
                    {
                        return std::declval<MatchT>();
                    }(std::declval<type_list>()))>;

                template<typename Find>
                static constexpr auto type_index = []<std::size_t Index>(indexed_type<Index, Find>&)
                {
                    return Index;
                }(std::declval<type_list>());
            };
        };
    }

    template<typename... Types>
    struct type_list : apply_index_sequence_t<details::type_list<Types...>::template indexed,
            index_sequence_for_v<Types...>>
    {
        template<std::size_t... I>
        using sequence_indexed_t = type_list<typename type_list::template indexed_t<I>...>;

        template<typename Func>
        static constexpr void call() { (Func::template call<Types>(), ...); }

        template<typename T>
        using append_t = type_list<Types..., T>;

        template<typename T>
        using append_front_t = type_list<T, Types...>;

        template<template<typename...> typename T>
        using apply_type_list_t = T<Types...>;

        template<std::size_t Size, std::size_t From = 0>
        using select_range_indexed_t = apply_index_sequence_t<sequence_indexed_t,
            make_index_sequence_v<Size, From>>;
    };
}
