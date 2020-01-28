// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SerializationII_DemoEditorTarget : TargetRules
{
	public SerializationII_DemoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "SerializationII_Demo" } );
	}
}
