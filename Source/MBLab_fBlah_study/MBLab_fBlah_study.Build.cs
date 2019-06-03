// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MBLab_fBlah_study : ModuleRules
{
    public MBLab_fBlah_study(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
           new string[]
           {
               "Runtime/MovieSceneTracks/Public/Tracks/",
                "Runtime/MovieSceneTracks/Public/Sections/",
                "Runtime/MovieScene/Public/Sections/",
           });
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Networking",
                "Sockets",
                "LevelSequence",
            }
            );

        PrivateDependencyModuleNames.AddRange(new string[] {
            "MovieScene",
            "MovieSceneTracks",
        }
        );
        if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Sequencer",
            }
            );
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd",
            }
            );
        }
    }
}
