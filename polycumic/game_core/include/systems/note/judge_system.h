// Created by BlurringShadow at 2021-03-12-下午 2:21

#pragma once

#include "game_core.h"
#include "components/note/direction.h"
#include "components/note/judge_state.h"
#include "components/note/locator.h"

namespace polycumic::game_core::systems::note
{
    class judge_system
    {
    public:
        enum class result
        {
            perfect,
            good,
            miss
        };

        using component_t = components::note::judge_state;
        using direction_t = components::note::direction;
        using coordinate_t = components::note::coordinate;
        using surface_t = components::note::surface;

        void judge_from_input(
            std::chrono::milliseconds,
            coordinate_t,
            coordinate_t,
            surface_t,
            direction_t
        );

        void judge_from_timestamp(std::chrono::milliseconds);

        template<typename Func>
        void flush(Func func) { func(std::move(result_set_)); }

        static constexpr auto prefect_time = 50ms;

        static constexpr auto miss_time = 150ms;

    private:
        std::unordered_map<entt::entity, result> result_set_;
    };
}
