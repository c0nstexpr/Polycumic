// Created by BlurringShadow at 2021-03-11-下午 6:04

#pragma once

#include "utils.h"

namespace polycumic::utility
{
    template<typename Setter, typename T>
    concept setter_object = std::invocable<Setter, T>;
}
