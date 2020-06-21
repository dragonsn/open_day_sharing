using UnrealBuildTool;

public class NextRTS : ModuleRules
{
	public NextRTS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","RenderCore","RHI","RawMesh" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
