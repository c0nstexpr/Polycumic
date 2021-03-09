// Created by BlurringShadow at 2021-03-04-下午 11:27

#pragma once

#include "type_traits.h"

namespace polycumic::utility::traits
{
    namespace details
    {
        template<typename T>
        struct int_seq_transform
        {
            template<T... Values>
            struct apply
            {
                template<typename Func> requires std::is_invocable_r_v<T, Func, T>
                constexpr auto operator()(Func func) const
                {
                    return std::integer_sequence<T, func(Values)...>{};
                }
            };
        };

        // ReSharper disable once CppFunctionIsNotImplemented
        template<typename ValueT, template<ValueT...> typename T, ValueT... I>
        static constexpr T<I...> apply_int_seq(std::integer_sequence<ValueT, I...>);

        template<typename T, T From>
        static constexpr auto make_sequence_helper = [](const T v) { return v + From; };

        template<typename ValueT>
        struct int_seq_invoker
        {
            template<ValueT... I>
            struct apply
            {
                template<typename T> requires requires
                {
                    (T::template call<I>(), ...);
                }
                static constexpr void invoke() { (T::template call<I>(), ...); }
            };
        };
    }

    template<typename... Types>
    inline constexpr auto index_sequence_for_v = std::index_sequence_for<Types...>{};

    template<
        typename ValueT,
        template<ValueT...> typename T,
        auto Sequence>
    using apply_sequence_t = decltype(details::apply_int_seq<ValueT, T>(Sequence));

    template<
        typename ValueT,
        typename T,
        auto Sequence>
    constexpr void apply_sequence_invoke()
    {
        apply_sequence_t<ValueT, details::int_seq_invoker<ValueT>::template apply, Sequence>
            ::template invoke<T>();
    }

    template<typename ValueT, template<ValueT...> typename T, auto Sequence>
    static constexpr auto apply_sequence_v = apply_sequence_t<ValueT, T, Sequence>{};

    template<template<std::size_t...> typename T, auto SequenceT>
    using apply_index_sequence_t = apply_sequence_t<std::size_t, T, SequenceT>;

    template<template<std::size_t...> typename T, auto Sequence>
    static constexpr auto apply_index_sequence_v = apply_index_sequence_t<T, Sequence>{};

    template<typename T, auto Sequence>
    constexpr void apply_index_sequence_invoke() { apply_sequence_invoke<size_t, T, Sequence>(); }

    template<typename ValueT, auto Sequence, typename Func>
    static constexpr auto transform_sequence(Func func)
    {
        return apply_sequence_v<
            ValueT,
            details::int_seq_transform<ValueT>::template apply,
            Sequence
        >(func);
    }

    template<typename ValueT, auto Sequence, auto Func>
    static constexpr auto transform_sequence_v = transform_sequence<ValueT, Sequence>(Func);

    template<typename T, T Size, T From = 0>
    static constexpr auto make_integer_sequence_v = transform_sequence_v<
        T,
        std::make_integer_sequence<T, Size>{},
        details::make_sequence_helper<T, From>
    >;

    template<std::size_t Size, std::size_t From = 0>
    static constexpr auto make_index_sequence_v = make_integer_sequence_v<std::size_t, Size, From>;
}
