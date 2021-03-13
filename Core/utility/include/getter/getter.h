// Created by BlurringShadow at 2021-03-11-下午 5:59

#pragma once
#include "utils.h"

namespace polycumic::utility
{
    template<typename Getter, typename T>
    concept getter_object = std::is_invocable_r_v<T, Getter>;

    template<typename Getter>
    using getter_value_t = std::remove_cvref_t<std::invoke_result_t<Getter>>;
}
