// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Host.h"
#include <GameFramework/GameModeBase.h>
#include "HostGameModeBase.generated.h"

UCLASS()
class HOST_API AHostGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

    void StartPlay() override;
};
