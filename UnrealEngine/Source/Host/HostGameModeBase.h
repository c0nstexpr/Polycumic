// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameFramework/GameModeBase.h>

#include "Host.h"

#include <third_include_start.h>

#include <boost/date_time.hpp>
#include <fmt/format.h>

#include <third_include_end.h>

#include "HostGameModeBase.generated.h"

UCLASS()
class HOST_API AHostGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

    void StartPlay() override;
};
