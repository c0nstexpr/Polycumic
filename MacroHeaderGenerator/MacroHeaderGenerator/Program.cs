using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace MacroHeaderGenerator
{
    public static class Program
    {
        const string _macroPattern = @"\s+([\w_\d]*)";

        static readonly Regex _defineMacroRegex = new(
            @$"^\s*#define{_macroPattern}", RegexOptions.Compiled | RegexOptions.Multiline
        );

        static readonly Regex _undefMacroRegex = new(
            @$"^\s*#undef{_macroPattern}", RegexOptions.Compiled | RegexOptions.Multiline
        );

        public static async Task<IImmutableSet<string>> FindUnrealEngineMacro(string srcPath)
        {
            Console.WriteLine("Caching all file content");
            var headerPaths = Directory.GetFiles(
                Path.Combine(srcPath, "Engine", "Source", "Runtime"), "*.h", SearchOption.AllDirectories
            );
            var taskList = new List<Task>();
            var set = new HashSet<string>();

            Console.WriteLine($"Totally {headerPaths.Length} files");

            taskList.AddRange(
                from path in headerPaths
                select Task.Run(
                    async () =>
                    {
                        Console.WriteLine($"{Path.GetFileName(path)}");

                        var content = await File.ReadAllTextAsync(path)!;

                        var macros = AnalyzeHeaderMacro(content);
                        lock (set)
                            foreach (var macro in macros)
                                set.Add(macro);
                    }
                )
            );

            await Task.WhenAll(taskList);
            return set.ToImmutableHashSet()!;
        }

        static IEnumerable<string> AnalyzeHeaderMacro(string content)
        {
            var matches = _defineMacroRegex.Matches(content);
            var macros = new HashSet<string>(matches.Count);
            foreach (Match match in matches)
            {
                var macroName = match?.Groups[1].Value;
                if (!string.IsNullOrEmpty(macroName)) macros.Add(macroName);
            }

            matches = _undefMacroRegex.Matches(content);
            foreach (Match match in matches)
            {
                var macroName = match?.Groups[1].Value;
                if (!string.IsNullOrEmpty(macroName)) macros.Remove(macroName);
            }

            return macros;
        }

        static void GenerateHeaderPair(IEnumerable<string> macros)
        {
            Console.WriteLine("Generating headers");

            const string headerGuard = "#pragma once\n";
            var undefHeader = new StringBuilder(headerGuard);
            var reDefHeader = new StringBuilder(headerGuard);
            foreach (var macroName in macros)
            {
                undefHeader.AppendLine($"#pragma push_macro(\"{macroName}\")").AppendLine($"#undef {macroName}\n");
                reDefHeader.Insert(0, $"#pragma pop_macro(\"{macroName}\")\n\n");
            }

            File.WriteAllText("undef_macros.generated.h", undefHeader.ToString());
            File.WriteAllText("redef_macros.generated.h", reDefHeader.ToString());
        }

        public static async Task Main(string[] args)
        {
            var path = args?[0];
            if (!string.IsNullOrEmpty(path)) GenerateHeaderPair(await FindUnrealEngineMacro(path));
        }
    }
}