// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <GameFramework/GameModeBase.h>
#include "Host.h"

#include "HostGameModeBase.generated.h"

UCLASS()
class HOST_API AHostGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

    void StartPlay() override;
};
