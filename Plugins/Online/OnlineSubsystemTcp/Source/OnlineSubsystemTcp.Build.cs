// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class OnlineSubsystemLLL : ModuleRules
{
	public OnlineSubsystemLLL(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDefinitions.Add("ONLINESUBSYSTEM_LLL_PACKAGE=1");

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Private");

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Sockets",
                "Json",
                "OnlineSubsystem",
                "PacketHandler",
            }
        );

        if (Target.bCompileAgainstEngine)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "Engine",
                    "OnlineSubsystemUtils"
                }
            );
        }
    }
}
