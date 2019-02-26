// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MBLab_fBlah_studyEditorTarget : TargetRules
{
	public MBLab_fBlah_studyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "MBLab_fBlah_study" } );
	}
}
