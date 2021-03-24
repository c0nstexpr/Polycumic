// Created by BlurringShadow at 2021-03-11-上午 11:08

#pragma once

#include "concurrent_object.h"
#include "game_core.h"
#include "components/transform.h"
#include "components/note/judge_state.h"
#include "components/note/locator.h"
#include "traits/member.h"

namespace polycumic::game_core::systems::note
{
    class locator_system
    {
    public:
        using component_t = components::note::locator;
        using timestamp_t = utility::traits::member_t<&component_t::timestamp>;
        using coordinate_t = components::note::coordinate;
        using surface_t = components::note::surface;

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

        auto get_view() { return registry.view<components::note::locator>(); }

        auto get_view() const { return registry.view<const components::note::locator>(); }

        void rearrange()
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

        static constexpr auto cube_length = 300;

        static constexpr auto square_count = 3;

        static constexpr auto square_length = cube_length / square_count;

        static constexpr auto location = []
        {
            std::array<std::array<glm::dvec3, 3>, 3> v{};

            for(auto i = 0; i < v.size(); ++i)
                for(auto j = 0; j < v.size(); ++j)
                    v[i][j] = {
                        (i - 1) * square_length,
                        square_length / 2,
                        (1 - j) * square_length
                    };

            return v;
        }();

        const std::array<glm::dmat4, 6> surface_rotation = []
        {
            std::array<glm::dquat, 6> quaternions{};
            std::array<glm::dmat4, 6> rotations{};

            quaternions[utility::to_underlying(surface_t::back)] =
                angleAxis(glm::pi<double>(), glm::dvec3{0, 0, 1});
            quaternions[utility::to_underlying(surface_t::left)] =
                angleAxis(glm::half_pi<double>(), glm::dvec3{0, 0, 1});
            quaternions[utility::to_underlying(surface_t::right)] =
                angleAxis(-glm::half_pi<double>(), glm::dvec3{0, 0, 1});
            quaternions[utility::to_underlying(surface_t::up)] =
                angleAxis(-glm::half_pi<double>(), glm::dvec3{1, 0, 0});
            quaternions[utility::to_underlying(surface_t::down)] =
                angleAxis(glm::half_pi<double>(), glm::dvec3{1, 0, 0});

            for(std::size_t i = 0; i < rotations.size(); ++i)
                rotations[i] = static_cast<glm::dmat4>(quaternions[i]);

            return rotations;
        }();

    private:
        void move(timestamp_t, surface_t);

        surface_t current_render_surface_;

        utility::traits::member_t<&component_t::speed> speed_{};

        timestamp_t timestamp_;

        static constexpr auto time_diff_render_guard_ = 3s;

        std::unordered_set<entt::entity> transforms_changed_;
    };
}
