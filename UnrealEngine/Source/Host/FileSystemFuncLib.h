// Created by BlurringShadow at 2021-03-27-下午 5:14

#pragma once

#include "Host.h"

#include <filesystem>
#include <Kismet/BlueprintFunctionLibrary.h>

#include "FileSystemFuncLib.generated.h"

UCLASS()
class UFileSystemFuncLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    static std::filesystem::path to_path(const FString& str)
    {
        return std::filesystem::path(ToCStr(str));
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    static FString get_directory(const FString& str)
    {
        return to_path(str).parent_path().c_str();
    }
};