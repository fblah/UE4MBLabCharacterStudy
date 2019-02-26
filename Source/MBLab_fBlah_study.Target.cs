// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MBLab_fBlah_studyTarget : TargetRules
{
	public MBLab_fBlah_studyTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "MBLab_fBlah_study" } );
	}
}
