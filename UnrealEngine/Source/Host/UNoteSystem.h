#pragma once

#include "Host.h"

#include <Kismet/GameplayStatics.h>

#include <third_include_start.h>

#include <fstream>
#include <systems/note/judge_system.h>
#include <systems/note/locator_system.h>
#include <systems/note/note_system.h>

#include <third_include_end.h>

#include "UNoteSystem.generated.h"

using game_core::components::note::direction;
using game_core::components::note::coordinate;
using game_core::components::note::surface;
using game_core::systems::note::judge_system;

UENUM()
enum class NoteDirection
{
    none = direction::none,
    up = direction::up,
    down = direction::down,
    left = direction::left,
    right = direction::right
};

UENUM()
enum class Coordinate
{
    left = coordinate::left,
    right = coordinate::right,
    middle = coordinate::middle
};

UENUM()
enum class CubeSurface
{
    front = surface::front,
    back = surface::back,
    left = surface::left,
    right = surface::right,
    up = surface::up,
    down = surface::down,
};

UENUM()
enum class JudgeResult
{
    miss = judge_system::result::miss,
    good = judge_system::result::good,
    perfect = judge_system::result::perfect,
};

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOST_API UUNoteSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UUNoteSystem() { PrimaryComponentTick.bCanEverTick = false; }

    UFUNCTION()
    void destroy(const FEntity id)
    {
        game_core::registry.destroy(id);
        note_map_.erase(id);
    }

    UFUNCTION()
    void move(const float audio_time);

    static constexpr auto to_duration(const float audio_time)
    {
        using std_time_t = std::chrono::milliseconds;

        return std_time_t(static_cast<std_time_t::rep>(audio_time * 1000));
    }

    UFUNCTION()
    void judge_update(const float audio_time)
    {
        judge_system_.judge_from_timestamp(to_duration(audio_time));

        judge_system_.flush(
            [this](const auto& results)
            {
                for(const auto [entity, result] : results)
                    perform_judge_res(note_map_[entity], static_cast<JudgeResult>(result));
            }
        );
    }

    UFUNCTION()
    void update(const float audio_time)
    {
        move(audio_time);
        judge_update(audio_time);
    }

    UFUNCTION()
    virtual AActor* instantiate(const NoteDirection direction)
    UE_PURE_VIRTUAL(instantiate)

    UFUNCTION()
    virtual void perform_judge_res(AActor* note, const JudgeResult result)
    UE_PURE_VIRTUAL(perform_judge_res)

    UFUNCTION()
    void load_chart_path(const FString& path_sv)
    {
        using namespace std;

        note_map_.reserve(
            [&path_sv, &note_system = note_system_]
            {
                ifstream fs(ToCStr(path_sv));
                return note_system.create_from_json(
                    [&fs]
                    {
                        string content;
                        fs >> content;
                        return content;
                    }()
                );
            }().size()
        );
    }

    UFUNCTION()
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
    }

private:
    game_core::systems::note::note_system note_system_;
    game_core::systems::note::locator_system locator_system_;
    judge_system judge_system_;

    std::unordered_map<entt::entity, AActor*> note_map_;
};
