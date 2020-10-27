// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class SwitcherooEditor : ModuleRules
{
	public SwitcherooEditor(ReadOnlyTargetRules Target)
		 : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_4_20_OR_LATER
		bLegacyPublicIncludePaths = false;
#endif

		PrivateIncludePaths.Add("SwitcherooEditor/Private");

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"UnrealEd",
			"Switcheroo",
		});
	}
}
