#include "systems/note/judge_system.h"

namespace polycumic::game_core::systems::note
{
    void judge_system::judge_from_input(
        const std::chrono::milliseconds input_timestamp,
        const coordinate_t horizontal,
        const coordinate_t vertical,
        const surface_t surface,
        const direction_t direction
    )
    {
        auto&& group = 
            registry.group<component_t, direction_t>(entt::get<const components::note::locator>);
        entt::entity candidate_note_entity = entt::null;

        auto timestamp = std::chrono::milliseconds::max();
        for(const auto note : group)
        {
            const auto [judge_state, locator] =
                group.get<component_t, const components::note::locator>(note);

            if(locator.surface == surface &&
                !judge_state.state &&
                locator.horizontal_coordinate == horizontal &&
                locator.vertical_coordinate == vertical &&
                locator.timestamp < timestamp)
            {
                candidate_note_entity = note;
                timestamp = locator.timestamp;
            }
        }

        if(candidate_note_entity == entt::null) return;

        auto& judge_state = group.get<component_t>(candidate_note_entity);
        const auto diff = timestamp - input_timestamp;
        const auto note_direction = group.get<direction_t>(candidate_note_entity);

        if((note_direction == direction_t::none || note_direction == direction) &&
            utility::is_between(diff, -miss_time, miss_time))
        {
            judge_state.state = true;

            result_set_.emplace(
                candidate_note_entity,
                utility::is_between(diff, -prefect_time, prefect_time) ?
                result::perfect :
                result::good
            );
        }
    }

    void judge_system::judge_from_timestamp(const std::chrono::milliseconds timestamp)
    {
        auto&& group = registry.group<component_t>(entt::get<const components::note::locator>);
        std::mutex mutex;
        std::for_each(
            std::execution::par,
            group.begin(),
            group.end(),
            [=, &result_set = result_set_, &group, &mutex](const auto note)
            {
                auto&& [judge_state, locator] = group.get<component_t, const
                    components::note::locator>(note);

                auto& state = judge_state.state;

                if(!state && locator.timestamp + miss_time < timestamp)
                {
                    state = true;

                    {
                        std::scoped_lock _(mutex);
                        result_set.emplace(note, result::miss);
                    }
                }
            }
        );
    }
}
