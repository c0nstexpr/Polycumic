// Created by BlurringShadow at 2021-03-01-下午 9:06

#pragma once

#include "getter.h"
#include "type_traits.h"
#include "traits/object.h"

namespace polycumic::utility
{
    template<typename T, bool ReturnByValue = std::is_scalar_v<T>>
    struct value_getter : std::type_identity<T>, traits::unique_object_trait
    {
        using base = std::type_identity<T>;
        using base::base;
        using base::type;
        using const_reference_t = add_const_lvalue_ref_t<type>;
        using getter_result_t = std::conditional_t<ReturnByValue, typename base::type, const_reference_t>;

    private:
        static constexpr auto is_nothrow_gettable_v_ = !ReturnByValue ||
            std::is_nothrow_copy_constructible_v<T>;

    public:
        const_reference_t v;

        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr value_getter(const_reference_t value) noexcept : v(value) {}

        [[nodiscard]] constexpr getter_result_t operator()() const noexcept(is_nothrow_gettable_v_)
        {
            return v;
        }

        // ReSharper disable once CppNonExplicitConversionOperator
        [[nodiscard]] constexpr operator getter_result_t() const noexcept(is_nothrow_gettable_v_)
        {
            return v;
        }
    };

    template<typename T>
    value_getter(const T&) -> value_getter<T>;
}
