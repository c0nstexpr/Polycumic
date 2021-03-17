#region File Info

// Created by BlurringShadow at 2020-10-16-下午 3:35

#endregion

using UnrealBuildTool;

using System.Collections.Generic;

public abstract class TargetBase : TargetRules
{
    public const string ProjectModuleName = "Host";

    protected TargetBase(
        TargetInfo target,
        UnrealBuildTool.TargetType type,
        IList<string> extraModuleNames = null
    ) :
        base(target)
    {
        Type = type;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        CppStandard = CppStandardVersion.Cpp17;

        // to enable exception
        bOverrideBuildEnvironment = true;
        bForceEnableExceptions = true;

        ExtraModuleNames.Add(ProjectModuleName);
        if (extraModuleNames != null) ExtraModuleNames.AddRange(extraModuleNames);
    }
}

public class HostTarget : TargetBase
{
    public HostTarget(TargetInfo target) : base(target, TargetType.Game)
    {
    }
}