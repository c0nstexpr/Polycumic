// Created by BlurringShadow at 2021-03-02-上午 10:42

#pragma once
#include "functional.h"
#include "type_traits.h"

namespace polycumic::utility
{
    enum class validation_result
    {
        success,
        ignored,
        failed
    };

    template<typename ValidatorT, typename T>
    CPP_concept validator = std::is_invocable_r_v<validation_result, ValidatorT, const T&, const T&>;

    template<typename T>
    inline constexpr auto equality_validator = [](
        add_const_lvalue_ref_t<T> pre,
        add_const_lvalue_ref_t<T> cur
    ) noexcept(noexcept(equal_to(pre, cur)))
    {
        return equal_to(pre, cur) ? validation_result::ignored : validation_result::success;
    };

    class validation_error : public std::invalid_argument
    {
        using base = std::invalid_argument;
        using base::base;
    };
}
