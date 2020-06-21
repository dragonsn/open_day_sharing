using UnrealBuildTool;
using System.Collections.Generic;

public class NextRTSTarget : TargetRules
{
	public NextRTSTarget(TargetInfo Target): base(Target)
	{
		Type = TargetType.Game;
        
		ExtraModuleNames.AddRange( new string[] { "NextRTS" } );
    }

}
