#include "NoteSystem.h"

void UNoteSystem::move(const std_time_t audio_time)
{
    locator_system_.set_timestamp(audio_time);

    const auto& view = game_core::registry.view<
        const direction,
        const transform_component_t>();
    locator_system_.each_transform(
        [this, &view](const auto& entities)
        {
            for(const entt::entity entity : entities)
            {
                if(note_map_.count(entity) == 0)
                    note_map_.emplace(
                        entity,
                        get_from_pool(view.get<const direction>(entity))
                    );

                note_map_.at(entity)->SetActorRelativeTransform(
                    to_FTransform(view.get<const transform_component_t>(entity))
                );
            }
        }
    );
}

void UNoteSystem::update(const float audio_time)
{
    const auto duration = to_duration(audio_time);
    move(duration);
    judge_update(duration);
}

CubeSurface UNoteSystem::rotate(const NoteDirection d)
{
    const auto& view = game_core::registry.view<const direction>();

    for(const auto [id, actor] : note_map_)
    {
        disable_actor(actor);
        pool_[utility::to_underlying(view.get<const direction>(id))].emplace_back(
            std::move(actor)
        );
    }
    note_map_.clear();

    locator_system_.rotate(static_cast<direction>(d));

    return static_cast<CubeSurface>(locator_system_.get_current_surface());
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

    if(!game_core::registry.empty<game_core::components::note::locator>())
    {
        const auto& view = game_core::registry.view<game_core::components::note::locator>();
        const std::vector entities(view.begin(), view.end());
        game_core::registry.destroy(entities.cbegin(), entities.cend());
    }
}

void UNoteSystem::load_chart_path(const FString& path_sv)
{
    using namespace std;
    release();
    try
    {
        chart_info_ = [&path_sv]
        {
            ostringstream str;
            str << ifstream(ToCStr(path_sv)).rdbuf();
            return str.str();
        }();

        reload();
    }
    catch(const exception& e)
    {
        ULog::Info(e.what());
        chart_info_ = decltype(chart_info_){};
    }
}

void UNoteSystem::judge_input(
    const Coordinate horizontal,
    const Coordinate vertical,
    const NoteDirection direction_v,
    const CubeSurface surface_v,
    const float audio_time
)
{
    judge_system_.judge_from_input(
        to_duration(audio_time),
        static_cast<coordinate>(horizontal),
        static_cast<coordinate>(vertical),
        static_cast<surface>(surface_v),
        static_cast<direction>(direction_v)
    );

    flush_judge_results();
}

bool UNoteSystem::is_loaded() const { return chart_info_.size() != 0; }

void UNoteSystem::reload()
{
    if(chart_info_.length() > 0)
    {
        release();
        auto&& notes = note_system_.create_from_json(chart_info_);

        ULog::Info_WithCondition("chart doesn't contains any notes", notes.empty());

        note_map_.reserve(notes.size());
    }
}

void UNoteSystem::set_reaction_time(const float t)
{
    locator_system_.set_reaction_time(to_duration(t));
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
