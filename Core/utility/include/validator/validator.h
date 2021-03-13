// Created by BlurringShadow at 2021-03-02-上午 10:42

#pragma once
#include "functional.h"
#include "type_traits.h"

namespace polycumic::utility
{
    enum class validation_result
    {
        success,
        equaled,
        failed
    };

    template<typename ValidatorT, typename T>
    concept validator = std::is_invocable_r_v<validation_result, ValidatorT, const T&, const T&>;

    template<typename T>
    inline constexpr auto equality_validator = [](
        add_const_lvalue_ref_t<T> pre,
        add_const_lvalue_ref_t<T> cur
    ) noexcept(
        std::is_nothrow_invocable_v<
            decltype(equal_to),
            add_const_lvalue_ref_t<T>,
            add_const_lvalue_ref_t<T>
        >
    )
    {
        return equal_to(pre, cur) ? validation_result::equaled : validation_result::success;
    };

    class validation_error : public std::invalid_argument
    {
        using base = std::invalid_argument;
        using base::base;
    };
}
