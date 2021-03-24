#include "UNoteSystem.h"

void UUNoteSystem::move(const float audio_time)
{
    locator_system_.set_timestamp(to_duration(audio_time));

    locator_system_.each_transform(
        [this](const auto& entities)
        {
            const auto& view = game_core::registry.view<
                const direction,
                const transform_component_t>();
            for(const entt::entity entity : entities)
            {
                if(note_map_.count(entity) == 0)
                    note_map_.emplace(
                        entity,
                        instantiate(
                            static_cast<NoteDirection>(
                                view.get<const direction>(entity)
                            )
                        )
                    );

                note_map_[entity]->SetActorRelativeTransform(
                    to_FTransform(view.get<const transform_component_t>(entity))
                );
            }
        }
    );
}
