// Created by BlurringShadow at 2021-03-14-下午 2:49

#include "systems/note/note_system.h"

#include "components/transform.h"
#include "components/note/judge_state.h"

namespace polycumic::game_core::systems::note
{
    std::vector<entt::entity> note_system::create_from_json(
        const simdjson::padded_string& str
    )
    {
        using namespace simdjson;

        std::vector<entt::entity> entities;

        try
        {
            for(ondemand::object&& obj : parser_.iterate(str))
            {
                const auto id = registry.create();
                entities.emplace_back(id);

                registry.emplace<components::transform>(id);
                registry.emplace<components::note::judge_state>(id);
                {
                    auto&& locator_obj = obj["locator"].value();
                    registry.emplace<components::note::locator>(
                        id, 
                        std::chrono::milliseconds{locator_obj["timestamp"].get_uint64().value()},
                        locator_obj["speed"].get_double().value(),
                        *magic_enum::enum_cast<components::note::surface>(
                            locator_obj["surface"].get_string().value()
                        ),
                        *magic_enum::enum_cast<components::note::coordinate>(
                            locator_obj["horizontal_coordinate"].get_string().value()
                        ),
                        *magic_enum::enum_cast<components::note::coordinate>(
                            locator_obj["vertical_coordinate"].get_string().value()
                        )
                    );
                }

                registry.emplace<components::note::direction>(
                    id, 
                    *magic_enum::enum_cast<components::note::direction>(
                        obj["direction"].get_string().value()
                    )
                );
            }
        }
        catch(...)
        {
            registry.destroy(entities.cbegin(), entities.cend());
            throw;
        }

        return entities;
    }
}
