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
    concept validator = std::same_as<std::invoke_result_t<ValidatorT, T, T>, validation_result>;

    template<typename T>
    inline constexpr auto equality_validator = [](
        add_const_lvalue_ref_t<T> pre,
        add_const_lvalue_ref_t<T> cur
    )
    {
        return equal_to(pre, cur) ? validation_result::equaled : validation_result::success;
    };
}
