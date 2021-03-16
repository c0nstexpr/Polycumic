// Created by BlurringShadow at 2021-03-11-上午 11:08

#pragma once

#include "concurrent_object.h"
#include "core.h"
#include "components/transform.h"
#include "components/note/judge_state.h"
#include "components/note/locator.h"
#include "property/property.h"

namespace polycumic::game_core::systems::note
{
    class locator_system
    {
    public:
        using component_t = components::note::locator;
        using timestamp_t = utility::traits::member_t<&component_t::timestamp>;
        using coordinate_t = components::note::coordinate;
        using surface_t = components::note::surface;

        static constexpr auto square_center(
            const coordinate_t horizontal,
            const coordinate_t vertical
        )
        {
            constexpr auto half_length = square_length / 2;

            return vec_d<3>{
                (utility::to_underlying(horizontal) - 1) * square_length + half_length,
                (utility::to_underlying(vertical) - 1) * square_length + half_length,
                half_length
            };
        }

        locator_system() = default;

        constexpr timestamp_t get_timestamp() const { return timestamp_; }

        void set_timestamp(const timestamp_t timestamp)
        {
            move(timestamp, current_render_surface_);
            timestamp_ = timestamp;
        }

        constexpr surface_t get_current_surface() const { return current_render_surface_; }

        void set_current_surface(const surface_t surface)
        {
            move(timestamp_, surface);
            current_render_surface_ = surface;
        }

        constexpr auto& speed() { return speed_prop_; }

        constexpr auto& speed() const { return speed_prop_; }

        constexpr auto& timestamp() { return timestamp_prop_; }

        constexpr auto& timestamp() const { return timestamp_prop_; }

        constexpr auto& current_surface() { return current_surface_prop_; }

        constexpr auto& current_surface() const { return current_surface_prop_; }

        auto get_view() { return registry.view<components::note::locator>(); }

        auto get_view() const { return registry.view<const components::note::locator>(); }

        void rearrange() const
        {
            registry.sort<component_t>(
                [](const auto& l, const auto& r)
                {
                    return l.surface < r.surface ||
                        l.timestamp < r.timestamp ||
                        l.horizontal_coordinate < r.horizontal_coordinate ||
                        l.vertical_coordinate < r.vertical_coordinate;
                }
            );
        }

        template<typename Func>
        void each_transform(Func func) { func(std::move(transforms_changed_)); }

        static constexpr unsigned cube_length = 1050;

        static constexpr unsigned square_count = 3;

        static constexpr auto square_length = cube_length / square_count;

    private:
        void move(const timestamp_t timestamp, const surface_t current_sur)
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
                            transform.value = glm::translate(
                                {},
                                glm::mix(
                                    vec_d<3>{},
                                    square_center(
                                        locator.horizontal_coordinate,
                                        locator.vertical_coordinate
                                    ),
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

        surface_t current_render_surface_;

        utility::traits::member_t<&component_t::speed> speed_{};

        timestamp_t timestamp_;

        static constexpr auto time_diff_render_guard_ = 3s;

        AUTO_MEMBER(speed_prop_, utility::generate_value_property(speed_));

        utility::property<
            timestamp_t,
            utility::member_getter<&locator_system::get_timestamp>,
            utility::member_setter<timestamp_t, &locator_system::set_timestamp>
        > timestamp_prop_ =
            utility::generate_property_from_mem<
                &locator_system::get_timestamp, &locator_system::set_timestamp
            >(*this);

        utility::property<
            surface_t,
            utility::member_getter<&locator_system::get_current_surface>,
            utility::member_setter<surface_t, &locator_system::set_current_surface>
        > current_surface_prop_ =
            utility::generate_property_from_mem<
                &locator_system::get_current_surface, &locator_system::set_current_surface
            >(*this);

        std::unordered_set<entt::entity> transforms_changed_;
    };
}
