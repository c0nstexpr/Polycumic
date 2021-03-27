#include "NoteSystem.h"

void UNoteSystem::move(const float audio_time)
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
                        get_from_pool(
                            static_cast<NoteDirection>(view.get<const direction>(entity))
                        )
                    );

                note_map_[entity]->SetActorRelativeTransform(
                    to_FTransform(view.get<const transform_component_t>(entity))
                );
            }
        }
    );
}

void UNoteSystem::release()
{
    for(const auto actor : note_map_ | ranges::views::values)
        GetWorld()->DestroyActor(actor);
    note_map_ = {};

    for(auto& vec : pool_)
    {
        for(const auto actor : vec) GetWorld()->DestroyActor(actor);
        vec = {};
        vec.reserve(ini_cap_);
    }

    for(const auto note : notes_) game_core::registry.destroy(note);
    notes_.clear();
}

void UNoteSystem::load_chart_path(const FString& path_sv)
{
    using namespace std;
    release();
    try
    {
        chart_info_ = [&path_sv]
        {
            ifstream fs(ToCStr(path_sv));
            string content;
            fs >> content;
            return content;
        }();

        notes_ = note_system_.create_from_json(chart_info_);

        note_map_.reserve(notes_.size());
    }
    catch(...) { chart_info_ = decltype(chart_info_){}; }
}

// ReSharper disable once CppInconsistentNaming
AActor* UNoteSystem::instantiate_Implementation(NoteDirection)
{
    throw std::runtime_error("not implemented");
}

// ReSharper disable once CppInconsistentNaming
void UNoteSystem::perform_judge_res_Implementation(AActor*, JudgeResult)
{
    throw std::runtime_error("not implemented");
}
