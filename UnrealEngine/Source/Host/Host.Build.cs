// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Host : ModuleRules
{
    public Host(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "VcpkgIntegrate" });

        PCHUsage = PCHUsageMode.NoSharedPCHs;
        PrivatePCHHeaderFile = "Host.h";
        PublicSystemIncludePaths.Add(ModuleDirectory);
        CppStandard = CppStandardVersion.Cpp17;
        bUseUnity = false;
        MinFilesUsingPrecompiledHeaderOverride = 1;
    }
}