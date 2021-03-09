// Created by BlurringShadow at 2021-03-04-下午 11:27

#pragma once

#include "type_traits.h"

namespace polycumic::utility::traits
{
    namespace details
    {
        template<typename T, auto Func>
        requires std::is_invocable_r_v<T, decltype(Func), T>
        struct transform
        {
            template<T... Values>
            using apply_t = std::integer_sequence<T, Func(Values)...>;

            template<T... Values>
            static constexpr auto apply_v = apply_t<Values...>{};
        };

        // ReSharper disable once CppFunctionIsNotImplemented
        template<typename ValueT, template<ValueT...> typename T, ValueT... I>
        static constexpr T<I...> apply_index_sequence(std::integer_sequence<ValueT, I...>);

        template<typename T, T From>
        static constexpr auto make_sequence_helper = [](const T v) { return v + From; };
    }

    template<typename... Types>
    inline constexpr auto index_sequence_for_v = std::index_sequence_for<Types...>{};

    template<
        typename ValueT,
        template<ValueT...> typename T,
        auto Sequence>
    using apply_sequence_t = decltype(details::apply_index_sequence<ValueT, T>(Sequence));

    template<typename ValueT, template<ValueT...> typename T, auto Sequence>
    static constexpr auto apply_sequence_v = apply_sequence_t<ValueT, T, Sequence>{};

    template<template<std::size_t...> typename T, auto SequenceT>
    using apply_index_sequence_t = apply_sequence_t<std::size_t, T, SequenceT>;

    template<template<std::size_t...> typename T, auto Sequence>
    static constexpr auto apply_index_sequence_v = apply_index_sequence_t<T, Sequence>{};

    template<typename ValueT, auto Func, auto Sequence>
    static constexpr auto transform_sequence_v = apply_sequence_v<
        ValueT,
        details::transform<ValueT, Func>::template apply_t,
        Sequence
    >;

    template<typename T, T Size, T From = 0>
    static constexpr auto make_integer_sequence_v = transform_sequence_v<
        T,
        details::make_sequence_helper<T, From>,
        std::make_integer_sequence<T, Size>{}
    >;

    template<std::size_t Size, std::size_t From = 0>
    static constexpr auto make_index_sequence_v = make_integer_sequence_v<std::size_t, Size, From>;
}
