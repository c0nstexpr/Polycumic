// Created by BlurringShadow at 2021-03-22-下午 9:56

#pragma once

#include <third_include_start.h>

#include <utils.h>

#include <third_include_end.h>

#include "UObject/ObjectMacros.h"

#include "FEntity.generated.h"

USTRUCT()
struct FEntity
{
    GENERATED_BODY()

    entt::entity value;

    constexpr auto operator()() const noexcept { return value; }

    constexpr operator entt::entity() const noexcept { return value; }
};
