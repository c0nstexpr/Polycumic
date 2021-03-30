#region File Info

// Created by BlurringShadow at 2020-10-16-下午 3:35

#endregion

// ReSharper disable all

using System.Diagnostics;
using System.Collections.Generic;
using System.IO;

using UnrealBuildTool;

public class Host : ModuleRules
{
    public readonly string ProjectDir;

    public Host(ReadOnlyTargetRules target) : base(target)
    {
        ProjectDir = Path.Combine(ModuleDirectory, "..", "..");

        PublicDependencyModuleNames.AddRange(
            new[] { "Core", "CoreUObject", "Engine", "InputCore", "Debug" }
        );
        PCHUsage = PCHUsageMode.NoPCHs;
        PublicSystemIncludePaths.Add(ModuleDirectory);
        bUseUnity = false;

        SetDependency(target.Platform);
        foreach (var name in new[] { "utility", "simdjson", "game_core" }) AddProjectRef(name);
    }

    static IEnumerable<string> GetPathsByExtension(string libPath, string extension)
    {
        return Directory.GetFiles(libPath, "*." + extension);
    }

    string FindVcpkgPath()
    {
        using (var findProcess = Process.Start(
            new ProcessStartInfo("where", "vcpkg.exe")
            {
                UseShellExecute = false,
                RedirectStandardOutput = true
            }
        ))
        {
            findProcess.WaitForExit();
            return Directory.GetParent(findProcess.StandardOutput.ReadToEnd()).FullName;
        }
    }

    void AddProjectRef(string projectName)
    {
        var vcProjectPath = Path.Combine(ProjectDir, "..", "polycumic");
        var binaryDir = Path.Combine(vcProjectPath, "bin","Release-x64", projectName);
        var headerDir = Path.Combine(vcProjectPath, projectName, "include");

        PublicSystemIncludePaths.Add(headerDir);

        if (Directory.Exists(binaryDir))
            PublicAdditionalLibraries.AddRange(GetPathsByExtension(binaryDir, "lib"));
    }

    void SetDependency(UnrealTargetPlatform platform)
    {
        var vcpkgPath = Path.Combine(FindVcpkgPath(), "installed");
        var sourcePath = "";
        var libExtension = "";
        var binExtension = "";
        if (platform == UnrealTargetPlatform.Win64)
        {
            sourcePath = Path.Combine(vcpkgPath, "x64-windows");
            libExtension = "lib";
            binExtension = "dll";
        }
        /*
        else if (platform == UnrealTargetPlatform.Linux)
        {
            sourcePath = Path.Combine(vcpkgPath, "x64-linux");
            libExtension = "a";
            binExtension = "so";
        }
        */

        PublicSystemIncludePaths.Add(Path.Combine(sourcePath, "include"));

        foreach (var path in GetPathsByExtension(Path.Combine(sourcePath, "lib"), libExtension))
            if (!path.Contains("zlib"))
                PublicAdditionalLibraries.Add(path);

        foreach (var path in GetPathsByExtension(Path.Combine(sourcePath, "bin"), binExtension))
            if (!path.Contains("zlib"))
                RuntimeDependencies.Add(
                    Path.Combine("$(BinaryOutputDir)", Path.GetFileName(path)), path
                );
    }
}