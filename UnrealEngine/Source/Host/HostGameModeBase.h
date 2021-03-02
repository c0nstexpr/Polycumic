// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>

// Third party
#include <third_include_start.h>

#include <boost/date_time.hpp>
#include <fmt/format.h>

// ReSharper disable once CppWrongIncludesOrder
#include <third_include_end.h>

#include "HostGameModeBase.generated.h"

UCLASS()
class HOST_API AHostGameModeBase final : public AGameModeBase
{
    GENERATED_BODY()

    void StartPlay() override;
};
