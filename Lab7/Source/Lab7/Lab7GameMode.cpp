// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Lab7GameMode.h"
#include "Lab7Character.h"
#include "UObject/ConstructorHelpers.h"

ALab7GameMode::ALab7GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
