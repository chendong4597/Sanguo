// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SanguoTarget : TargetRules
{
	public SanguoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		//ExtraModuleNames.Add("Sanguo");
        ExtraModuleNames.AddRange(new string[] { "Sanguo", "ThirdGameModel" });
    }
}
