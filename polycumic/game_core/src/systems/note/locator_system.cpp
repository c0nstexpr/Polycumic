// Created by BlurringShadow at 2021-03-20-下午 10:35

#include "systems/note/locator_system.h"

namespace polycumic::game_core::systems::note
{
    void locator_system::rotate(const direction_t direction)
    {
        using namespace std;

        static constexpr auto epsilon = 0.01;

        static const array<glm::dquat, 5> direction_rotation{
            glm::dquat{},
            angleAxis(glm::half_pi<double>(), glm::dvec3{0, 0, 1}),
            angleAxis(-glm::half_pi<double>(), glm::dvec3{0, 0, 1}),
            angleAxis(-glm::half_pi<double>(), glm::dvec3{0, 1, 0}),
            angleAxis(glm::half_pi<double>(), glm::dvec3{0, 1, 0}),
        };

        const auto current_qua = current_qua_ *
            direction_rotation[utility::to_underlying(direction)];
        const auto orientation = glm::dvec3{1, 0, 0} * current_qua;

        auto surface = surface_t::down;

        if(orientation.x > epsilon) surface = surface_t::front;
        else if(-orientation.x > epsilon) surface = surface_t::back;
        else if(orientation.y > epsilon) surface = surface_t::left;
        else if(-orientation.y > epsilon) surface = surface_t::right;
        else if(orientation.z > epsilon) surface = surface_t::up;

        move(timestamp_, surface);
        current_qua_ = current_qua;
        current_render_surface_ = surface;
    }

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
            [this, &group, &mutex, current_sur, timestamp](const auto id)
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
                        transform.value = translate(
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
                            transforms_changed_.emplace(id);
                        }
                    }
                }
            }
        );
    }
}
