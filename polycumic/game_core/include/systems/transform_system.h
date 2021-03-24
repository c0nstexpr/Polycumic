// Created by BlurringShadow at 2021-03-10-下午 6:04

#pragma once

#include "component_observable.h"
#include "game_core.h"
#include "components/transform.h"

namespace polycumic::game_core::systems
{
    class transform_system
    {
    public:
        using component_t = components::transform;

    private:
        component_observable<component_t> observer_;

    public:
        auto get_view() { return registry.view<component_t>(); }

        auto get_view() const { return registry.view<const component_t>(); }

        void update(const entt::entity id, const component_t transform)
        {
            registry.get<component_t>(id) = transform;
        }

        auto get_observable() const { return observer_.observable(); }

        void flush() { return observer_.flush(); }
    };
}
