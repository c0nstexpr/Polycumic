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

public:
    using std_time_t = std::chrono::milliseconds;

    UNoteSystem() { PrimaryComponentTick.bCanEverTick = false; }

    void return_to_pool(const FEntity id, const direction direction)
    {
        deactivate(id, direction);
        game_core::registry.destroy(id);
    }

    void move(const std_time_t);

    static constexpr auto to_duration(const float audio_time)
    {
        return std_time_t(static_cast<std_time_t::rep>(audio_time * 1000));
    }

    void judge_update(const std_time_t audio_time)
    {
        judge_system_.judge_from_timestamp(audio_time);
        flush_judge_results();
    }

    UFUNCTION(BlueprintCallable)
    void update(const float audio_time);

    UFUNCTION(BlueprintCallable)
    CubeSurface rotate(const NoteDirection d);

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
                    const auto& actor_it = note_map_.find(entity);
                    if(actor_it != note_map_.cend())
                    {
                        perform_judge_res(actor_it->second, static_cast<JudgeResult>(result));
                        return_to_pool({entity}, game_core::registry.get<const direction>(entity));
                    }
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
    );

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool is_loaded() const;

    void OnComponentDestroyed(bool bDestroyingHierarchy) override
    {
        if(is_loaded()) release();
    }

    UFUNCTION(BlueprintCallable)
    void reload();

    UFUNCTION(BlueprintCallable)
    void set_reaction_time(const float t);

private:
    void release();

    gsl::not_null<AActor*> get_from_pool(const direction direction)
    {
        auto& p = pool_[utility::to_underlying(direction)];
        if(p.empty()) return instantiate(static_cast<NoteDirection>(direction));
        auto&& res = std::move(p.back());
        p.pop_back();
        res->SetActorHiddenInGame(false);
        return res;
    }

    void deactivate(const FEntity id, const direction direction)
    {
        {
            auto& actor = note_map_.at(id);
            disable_actor(actor);
            pool_[utility::to_underlying(direction)].emplace_back(std::move(actor));
        }
        note_map_.erase(id);
    }

    game_core::systems::note::note_system note_system_;
    game_core::systems::note::locator_system locator_system_;
    judge_system judge_system_;

    simdjson::padded_string chart_info_;
    std::unordered_map<entt::entity, gsl::not_null<AActor*>> note_map_;

    static constexpr auto ini_cap_ = 4;

    std::array<std::vector<gsl::not_null<AActor*>>, 5> pool_{};
};
