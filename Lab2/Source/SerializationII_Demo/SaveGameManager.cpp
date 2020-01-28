// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameManager.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"
#include "MemoryReader.h"
#include "MemoryWriter.h"

//////////////////////////////////////////////////////////////////////////
// FSaveGameHeader
static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"

struct FSaveGameFileVersion
{
	enum Type
	{
		InitialVersion = 1,
		// serializing custom versions into the savegame data to handle that type of versioning
		AddedCustomVersions = 2,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
};

struct FSaveGameHeader
{
	FSaveGameHeader();
	FSaveGameHeader(TSubclassOf<USaveGame> ObjectType);

	void Empty();
	bool IsEmpty() const;

	void Read(FMemoryReader& MemoryReader);
	void Write(FMemoryWriter& MemoryWriter);

	int32 FileTypeTag;
	int32 SaveGameFileVersion;
	int32 PackageFileUE4Version;
	FEngineVersion SavedEngineVersion;
	int32 CustomVersionFormat;
	FCustomVersionContainer CustomVersions;
	FString SaveGameClassName;
};

FSaveGameHeader::FSaveGameHeader()
	: FileTypeTag(0)
	, SaveGameFileVersion(0)
	, PackageFileUE4Version(0)
	, CustomVersionFormat(static_cast<int32>(ECustomVersionSerializationFormat::Unknown))
{}

FSaveGameHeader::FSaveGameHeader(TSubclassOf<USaveGame> ObjectType)
	: FileTypeTag(UE4_SAVEGAME_FILE_TYPE_TAG)
	, SaveGameFileVersion(FSaveGameFileVersion::LatestVersion)
	, PackageFileUE4Version(GPackageFileUE4Version)
	, SavedEngineVersion(FEngineVersion::Current())
	, CustomVersionFormat(static_cast<int32>(ECustomVersionSerializationFormat::Latest))
	, CustomVersions(FCustomVersionContainer::GetRegistered())
	, SaveGameClassName(ObjectType->GetPathName())
{}

void FSaveGameHeader::Empty()
{
	FileTypeTag = 0;
	SaveGameFileVersion = 0;
	PackageFileUE4Version = 0;
	SavedEngineVersion.Empty();
	CustomVersionFormat = (int32)ECustomVersionSerializationFormat::Unknown;
	CustomVersions.Empty();
	SaveGameClassName.Empty();
}

bool FSaveGameHeader::IsEmpty() const
{
	return (FileTypeTag == 0);
}

void FSaveGameHeader::Read(FMemoryReader& MemoryReader)
{
	Empty();

	MemoryReader << FileTypeTag;

	if (FileTypeTag != UE4_SAVEGAME_FILE_TYPE_TAG)
	{
		// this is an old saved game, back up the file pointer to the beginning and assume version 1
		MemoryReader.Seek(0);
		SaveGameFileVersion = FSaveGameFileVersion::InitialVersion;

		// Note for 4.8 and beyond: if you get a crash loading a pre-4.8 version of your savegame file and 
		// you don't want to delete it, try uncommenting these lines and changing them to use the version 
		// information from your previous build. Then load and resave your savegame file.
		//MemoryReader.SetUE4Ver(MyPreviousUE4Version);				// @see GPackageFileUE4Version
		//MemoryReader.SetEngineVer(MyPreviousEngineVersion);		// @see FEngineVersion::Current()
	}
	else
	{
		// Read version for this file format
		MemoryReader << SaveGameFileVersion;

		// Read engine and UE4 version information
		MemoryReader << PackageFileUE4Version;

		MemoryReader << SavedEngineVersion;

		MemoryReader.SetUE4Ver(PackageFileUE4Version);
		MemoryReader.SetEngineVer(SavedEngineVersion);

		if (SaveGameFileVersion >= FSaveGameFileVersion::AddedCustomVersions)
		{
			MemoryReader << CustomVersionFormat;

			CustomVersions.Serialize(MemoryReader, static_cast<ECustomVersionSerializationFormat::Type>(CustomVersionFormat));
			MemoryReader.SetCustomVersions(CustomVersions);
		}
	}

	// Get the class name
	MemoryReader << SaveGameClassName;
}

void FSaveGameHeader::Write(FMemoryWriter& MemoryWriter)
{
	// write file type tag. identifies this file type and indicates it's using proper versioning
	// since older UE4 versions did not version this data.
	MemoryWriter << FileTypeTag;

	// Write version for this file format
	MemoryWriter << SaveGameFileVersion;

	// Write out engine and UE4 version information
	MemoryWriter << PackageFileUE4Version;
	MemoryWriter << SavedEngineVersion;

	// Write out custom version data
	MemoryWriter << CustomVersionFormat;
	CustomVersions.Serialize(MemoryWriter, static_cast<ECustomVersionSerializationFormat::Type>(CustomVersionFormat));

	// Write the class name so we know what class to load to
	MemoryWriter << SaveGameClassName;
}
//////////////////////////////////////////////////////////////////////////

// Sets default values for this component's properties
USaveGameManager::USaveGameManager()
{
	PrimaryComponentTick.bCanEverTick = false; //No. Don't waste my time.
}


void USaveGameManager::Save(const FString& GameSlotName)
{
	// TODO: [Advanced] Handle waiting for garbage collection and Async loading

	//Find all actors that want to be saved

	UDemoSaveGame* saveGame = NewObject<UDemoSaveGame>();
	if (ensure(saveGame))
	{
		//Retrieve all actors to save and populate SaveGame actors
		TArray<AActor*> savableActors;
		UGameplayStatics::GetAllActorsWithInterface(this, USavableObject::StaticClass(), savableActors);
		for (AActor* actor : savableActors)
		{
			if (!IsValid(actor) || ISavableObject::Execute_SkipSave(actor))
			{
				continue; //Doesn't want or doesn't deserve saving
			}

			// TODO: [Advanced] Consider sorting the order of actors to reduce 'inter-weaving' actor dependency
			saveGame->WorldActors.Add(actor);

			// TODO: Find all components on the actor that are savable
		}

		SaveGameToSlot(saveGame, GameSlotName, 0); // User index on multi-player games or consoles will not always be 0
	}
}

void USaveGameManager::Load(const FString& GameSlotName)
{
	// TODO: [Advanced] Add safety waiting for garbage collection, world begun play, purging streaming levels, pending visibility requests

	//Destroy existing actors, we'll re-create them

	LoadGameFromSlot(GameSlotName, 0);
}

void USaveGameManager::SaveGameToSlot(UDemoSaveGame* SaveGameObj, FString SlotName, int32 UserIndex)
{
	UGameplayStatics::SaveGameToSlot(SaveGameObj, SlotName, UserIndex);
}

USaveGame* USaveGameManager::LoadGameFromSlot(const FString& SlotName, const int32 UserIndex)
{
	return UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
}
