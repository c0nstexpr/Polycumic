// Created by BlurringShadow at 2021-03-11-下午 6:04

#pragma once

#include "utility_core.h"

namespace polycumic::utility
{
    template<typename Setter, typename T>
    CPP_concept setter_object = ranges::invocable<Setter, T>;
}
