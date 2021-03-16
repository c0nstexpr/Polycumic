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
                CPP_template(typename Func)(requires(std::is_invocable_r_v<T, Func, T>))
                constexpr auto operator()(Func func) const
                {
                    return std::integer_sequence<T, func(Values)...>{};
                }
            };
        };

        // ReSharper disable once CppFunctionIsNotImplemented
        template<typename ValueT, template<ValueT...> typename T, ValueT... I>
        static constexpr T<I...> apply_int_seq(std::integer_sequence<ValueT, I...>);

        template<typename ValueT>
        struct int_seq_invoker
        {
            template<ValueT... I>
            struct apply
            {
                template<typename T>
                static constexpr void invoke() { (T::template call<I>(), ...); }
            };
        };
    }

    template<typename... Types>
    inline constexpr auto index_sequence_for_v = std::index_sequence_for<Types...>{};

    // ReSharper disable once CppFunctionIsNotImplemented
    template<typename ValueT, template<ValueT...> typename T, typename Sequence>
    constexpr auto apply_sequence(Sequence seq) -> decltype(details::apply_int_seq<ValueT, T>(seq));

#ifdef __cpp_lib_concepts
    template<typename ValueT, template<ValueT...> typename T, auto Sequence>
    using apply_sequence_t = decltype(apply_sequence<ValueT, T>(Sequence));
#endif

    template<typename ValueT, typename T, typename Sequence>
    constexpr void apply_sequence_invoke(Sequence)
    {
#ifdef __cpp_lib_concepts
        apply_sequence_t<ValueT, details::int_seq_invoker<ValueT>::template apply, seq>
            ::template invoke<T>();
#else
        decltype(apply_sequence<ValueT, details::int_seq_invoker<ValueT>::template apply>(
                Sequence{}
            ))
            ::template invoke<T>();
#endif
    }

    template<typename ValueT, template<ValueT...> typename T>
    static constexpr auto apply_sequence_v = [](auto Sequence)
    {
        return
#ifdef __cpp_lib_concepts
            apply_sequence_t<ValueT, T, Sequence>{};
#else
            decltype(apply_sequence<ValueT, T>(Sequence)){};
#endif
    };

#ifdef __cpp_lib_concepts
    template<template<std::size_t...> typename T, auto Sequence>
    using apply_index_sequence_t = apply_sequence_t<std::size_t, T, Sequence>;
#endif

    template<template<std::size_t...> typename T>
    static constexpr auto apply_index_sequence_v = [](auto Sequence)
    {
        return apply_sequence_v<std::size_t, T>(Sequence);
    };

    template<typename T, typename Sequence>
    constexpr void apply_index_sequence_invoke(Sequence seq)
    {
        apply_sequence_invoke<size_t, T>(seq);
    }

    template<typename ValueT, typename Sequence, typename Func>
    static constexpr auto transform_sequence(Sequence seq, Func func)
    {
        return apply_sequence_v<
            ValueT,
            details::int_seq_transform<ValueT>::template apply
        >(seq)(func);
    }

    template<typename ValueT>
    static constexpr auto transform_sequence_v = [](auto seq, auto Func)
    {
        return transform_sequence<ValueT>(seq, Func);
    };

    template<typename T, T Size, T From = 0>
    static constexpr auto make_integer_sequence_v = transform_sequence_v<T>(
        std::make_integer_sequence<T, Size>{},
        [](const T& v) { return v + From; }
    );

    template<std::size_t Size, std::size_t From = 0>
    static constexpr auto make_index_sequence_v = make_integer_sequence_v<std::size_t, Size, From>;
}
