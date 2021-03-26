// Created by BlurringShadow at 2021-03-11-下午 5:59

#pragma once
#include "utility_core.h"

namespace polycumic::utility
{
    template<typename Getter, typename T>
    CPP_concept getter_object = std::is_invocable_r_v<T, std::add_const_t<Getter>>;

    template<typename Getter>
    using getter_value_t = concepts::remove_cvref_t<std::invoke_result_t<Getter>>;
}
