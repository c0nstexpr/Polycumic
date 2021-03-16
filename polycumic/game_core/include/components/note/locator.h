// Created by BlurringShadow at 2021-03-10-下午 5:01

#pragma once

#include "data_types.h"

namespace polycumic::game_core::components::note
{
    enum class surface
    {
        front,
        left,
        back,
        right,
        up,
        down
    };

    enum class coordinate
    {
        left,
        middle,
        right
    };

    struct locator
    {
        std::chrono::milliseconds timestamp;

        double speed;

        surface surface;

        coordinate horizontal_coordinate;

        coordinate vertical_coordinate;
    };
}
