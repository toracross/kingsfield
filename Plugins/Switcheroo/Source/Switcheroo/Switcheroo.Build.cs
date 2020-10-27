// Copyright 2018-2020 Francesco Camarlinghi. All rights reserved.

using UnrealBuildTool;

public class Switcheroo : ModuleRules
{
	public Switcheroo(ReadOnlyTargetRules Target)
		 : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_4_20_OR_LATER
		bLegacyPublicIncludePaths = false;
#endif

		PrivateIncludePaths.Add("Switcheroo/Private");

		PrivateDependencyModuleNames.AddRange(new string[] {
			"SlateCore",
			"Slate",
			"InputCore",
		});

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
		});
	}
}
