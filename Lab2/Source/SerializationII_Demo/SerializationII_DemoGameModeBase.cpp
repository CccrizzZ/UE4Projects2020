// Fill out your copyright notice in the Description page of Project Settings.


#include "SerializationII_DemoGameModeBase.h"
#include "SaveGameManager.h"
#include "Kismet/GameplayStatics.h"

ASerializationII_DemoGameModeBase::ASerializationII_DemoGameModeBase()
	: Super()
{
	SaveGameManager = CreateDefaultSubobject<USaveGameManager>(TEXT("SGMan"));
}

ASerializationII_DemoGameModeBase* ASerializationII_DemoGameModeBase::GetDemoGameMode(const UObject* WorldContextObject)
{
	ASerializationII_DemoGameModeBase* pRetVal = nullptr;

	auto* pRawGM = UGameplayStatics::GetGameMode(WorldContextObject);
	if (IsValid(pRawGM))
	{
		pRetVal = Cast<ASerializationII_DemoGameModeBase>(pRawGM);
	}

	return pRetVal;
}

USaveGameManager* ASerializationII_DemoGameModeBase::GetSaveGameManager(const UObject* WorldContextObject)
{
	USaveGameManager* retVal = nullptr;

	ASerializationII_DemoGameModeBase* pGM = GetDemoGameMode(WorldContextObject);
	if (IsValid(pGM))
	{
		return pGM->SaveGameManager;
	}

	return retVal;
}
