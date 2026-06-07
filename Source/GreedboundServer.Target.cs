// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GreedboundServerTarget : TargetRules
{
    public GreedboundServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

        bUsesSlate = false;
        bBuildWithEditorOnlyData = false;

        ExtraModuleNames.Add("Greedbound");
    }
}
