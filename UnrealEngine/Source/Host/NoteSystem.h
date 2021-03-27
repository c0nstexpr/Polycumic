#pragma once

#include "Host.h"

#include <Kismet/GameplayStatics.h>

#include <third_include_start.h>

#include <fstream>
#include <systems/note/judge_system.h>
#include <systems/note/locator_system.h>
#include <systems/note/note_system.h>

#include <third_include_end.h>

#include "NoteSystem.generated.h"

using game_core::components::note::direction;
using game_core::components::note::coordinate;
using game_core::components::note::surface;
using game_core::systems::note::judge_system;

UENUM(BlueprintType)
enum class NoteDirection : uint8
{
    none = 0,
    left = direction::left,
    right = direction::right,
    up = direction::up,
    down = direction::down
};

UENUM(BlueprintType)
enum class Coordinate : uint8
{
    left = 0,
    middle = coordinate::middle,
    right = coordinate::right
};

UENUM(BlueprintType)
enum class CubeSurface : uint8
{
    front = 0,
    left = surface::left,
    back = surface::back,
    right = surface::right,
    up = surface::up,
    down = surface::down,
};

UENUM(BlueprintType)
enum class JudgeResult : uint8
{
    perfect = 0,
    good = judge_system::result::good,
    miss = judge_system::result::miss,
};

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOST_API UNoteSystem : public UActorComponent
{
    GENERATED_BODY()

    using std_time_t = std::chrono::milliseconds;

public:
    UNoteSystem(): direction_view_(std::as_const(game_core::registry).view<const direction>())
    {
        PrimaryComponentTick.bCanEverTick = false;
    }

    UFUNCTION(BlueprintCallable)
    void return_to_pool(const FEntity id)
    {
        deactivate(id, direction_view_.get<const direction>(id));
        game_core::registry.destroy(id);
    }

    UFUNCTION(BlueprintCallable)
    void move(const float audio_time);

    static constexpr auto to_duration(const float audio_time)
    {
        return std_time_t(static_cast<std_time_t::rep>(audio_time * 1000));
    }

    UFUNCTION(BlueprintCallable)
    void judge_update(const float audio_time)
    {
        judge_system_.judge_from_timestamp(to_duration(audio_time));
        flush_judge_results();
    }

    UFUNCTION(BlueprintCallable)
    void update(const float audio_time)
    {
        move(audio_time);
        judge_update(audio_time);
    }

    UFUNCTION(BlueprintCallable)
    CubeSurface rotate(const NoteDirection d)
    {
        for(const auto [id, actor] : note_map_)
        {
            disable_actor(actor);
            pool_[utility::to_underlying(
                std::as_const(game_core::registry).get<const direction>(id)
            )].emplace_back(actor);
        }
        note_map_.clear();

        locator_system_.rotate(static_cast<direction>(d));

        return static_cast<CubeSurface>(locator_system_.get_current_surface());
    }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AActor* instantiate(const NoteDirection direction);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void perform_judge_res(AActor* note, const JudgeResult result);

    UFUNCTION(BlueprintCallable)
    void load_chart_path(const FString& path_sv);

    void flush_judge_results()
    {
        judge_system_.flush(
            [this](const auto& results)
            {
                for(const auto [entity, result] : results)
                {
                    perform_judge_res(note_map_[entity], static_cast<JudgeResult>(result));
                    return_to_pool({entity});
                }
            }
        );
    }

    UFUNCTION(BlueprintCallable)
    void judge_input(
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

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool is_loaded() const { return !notes_.empty(); }

    void OnComponentDestroyed(bool bDestroyingHierarchy) override { release(); }

    UFUNCTION(BlueprintCallable)
    void reload()
    {
        if(chart_info_.length() > 0)
        {
            release();
            notes_ = note_system_.create_from_json(chart_info_);
            note_map_.reserve(notes_.size());
        }
    }

    UFUNCTION(BlueprintCallable)
    void set_reaction_time(const int t) { locator_system_.set_reaction_time(std_time_t{t}); }

private:
    void release();

    AActor* get_from_pool(const NoteDirection direction)
    {
        auto& p = pool_[utility::to_underlying(direction)];
        if(p.empty()) return instantiate(direction);
        const auto res = p.back();
        p.pop_back();
        res->SetActorHiddenInGame(false);
        return res;
    }

    void deactivate(const FEntity id, const direction& direction)
    {
        {
            auto actor = note_map_[id];
            disable_actor(actor);
            pool_[utility::to_underlying(direction)].emplace_back(actor);
        }
        note_map_.erase(id);
    }

    game_core::systems::note::note_system note_system_;
    game_core::systems::note::locator_system locator_system_;
    judge_system judge_system_;

    simdjson::padded_string chart_info_;
    std::vector<entt::entity> notes_;
    std::unordered_map<entt::entity, AActor*> note_map_;
    const entt::basic_view<entt::entity, entt::exclude_t<>, const direction> direction_view_;

    static constexpr auto ini_cap_ = 4;

    std::array<std::vector<AActor*>, 4> pool_{};
};
