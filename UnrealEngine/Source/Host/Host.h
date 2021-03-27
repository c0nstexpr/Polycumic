// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FEntity.h"

#include <Engine/Engine.h>
#include <Modules/ModuleManager.h>

#include <third_include_start.h>

#include <components/transform.h>

#include <third_include_end.h>

#define UE_PURE_VIRTUAL(name) PURE_VIRTUAL(name, throw std::runtime_error("not implemented");)

using namespace polycumic;

using transform_component_t = game_core::components::transform;

template<typename T>
FVector to_FVector(const glm::vec<3, T> vec)
{
    return {
        static_cast<float>(vec.x),
        static_cast<float>(vec.y),
        static_cast<float>(vec.z)
    };
}

inline auto to_FTransform(const transform_component_t component)
{
    auto t = component.value;

    constexpr auto converter = [](const glm::dvec4 vec)
    {
        return to_FVector(static_cast<glm::dvec3>(vec));
    };

    return FTransform(FMatrix(converter(t[0]), converter(t[1]), converter(t[2]), converter(t[3])));
}

inline void disable_actor(AActor* actor)
{
    actor->SetActorTickEnabled(false);
    actor->SetActorHiddenInGame(true);
    actor->SetActorEnableCollision(false);
}
