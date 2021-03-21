#pragma once

#include "Host.h"

#include "UNoteSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOST_API UUNoteSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    UUNoteSystem() { PrimaryComponentTick.bCanEverTick = true; }

    void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;
};
