// Created by BlurringShadow at 2021-03-20-下午 10:35

#include "systems/note/locator_system.h"

namespace polycumic::game_core::systems::note
{
    void locator_system::move(const timestamp_t timestamp, const surface_t current_sur)
    {
        using judge_state_t = components::note::judge_state;
        using transform_component_t = components::transform;

        auto&& group = registry.group<const component_t>(
            entt::get<transform_component_t, const judge_state_t>
        );
        std::mutex mutex;

        std::for_each(
            std::execution::par,
            group.begin(),
            group.end(),
            [=, &group, &transforms_changed = transforms_changed_, &mutex](const auto id)
            {
                auto&& [locator, transform, judge_state] = group.get<
                    const component_t,
                    transform_component_t,
                    const judge_state_t>(id);

                if(!judge_state.state && locator.surface == current_sur)
                {
                    const auto diff = (locator.timestamp - timestamp).count() /
                        (time_diff_render_guard_.count() / (speed_ * locator.speed));

                    if(utility::is_between(diff, 0, 1))
                    {
                        transform.value =
                            surface_rotation[utility::to_underlying(current_render_surface_)] *
                            translate(
                                scale({}, mix({}, glm::one<glm::dvec3>(), diff)),
                                mix(
                                    {},
                                    location[utility::to_underlying(
                                        locator.horizontal_coordinate
                                    )]
                                    [utility::to_underlying(locator.vertical_coordinate)],
                                    diff
                                )
                            );

                        {
                            std::scoped_lock _{mutex};
                            transforms_changed.emplace(id);
                        }
                    }
                }
            }
        );
    }
}
