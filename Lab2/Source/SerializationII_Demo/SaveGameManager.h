// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SaveGame.h"

#include "ObjectAndNameAsStringProxyArchive.h"
#include "SaveGameManager.generated.h"

UINTERFACE(MinimalAPI)
class USavableObject : public UInterface
{
	GENERATED_BODY()
};

class ISavableObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent)
	bool OnAllObjectsLoaded();
	virtual bool OnAllObjectsLoaded_Implementation() { return true; }

	UFUNCTION(BlueprintNativeEvent)
	bool SkipSave();
	virtual bool SkipSave_Implementation() { return false; }
};

UCLASS(ClassGroup = (SaveGame), BlueprintType, Blueprintable, editinlinenew)
class UDemoSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	//Actors to save
	UPROPERTY(SaveGame, VisibleInstanceOnly, Category = Data)
	TArray<AActor*> WorldActors;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERIALIZATIONII_DEMO_API USaveGameManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	USaveGameManager();

public:	

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void Save(const FString& GameSlotName);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void Load(const FString& GameSlotName);

private:
	void SaveGameToSlot(UDemoSaveGame* SaveGameObj, FString SlotName, int32 UserIndex);
	USaveGame* LoadGameFromSlot(const FString& SlotName, const int32 UserIndex);
};
