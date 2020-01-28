// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SerializationII_DemoGameModeBase.generated.h"


UCLASS()
class SERIALIZATIONII_DEMO_API ASerializationII_DemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASerializationII_DemoGameModeBase();

	//Note: Instanced implies "EditInline" 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dwarrows|SaveGame")
	class USaveGameManager* SaveGameManager;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static ASerializationII_DemoGameModeBase* GetDemoGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static class USaveGameManager* GetSaveGameManager(const UObject* WorldContextObject);
};
