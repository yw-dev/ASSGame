// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums/ShooterPawnEnums.h"
#include "ShooterPawnEntry.generated.h"

/*
*	Pawn's Data.(角色Entry)
*/
USTRUCT(BlueprintType)
struct FShooterRaceEntry : public FTableRowBase
{

	GENERATED_BODY()

public:
	FShooterRaceEntry()
		: Health(0.f)
		, Mana(0.f)
		, PawnBP(FSoftObjectPath(""))
	{
	}

	/////////////////////////// Race Attribute ////////////////////////////////
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "RaceID")
	EShooterRaceType RaceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "RaceName")
	FName RaceName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "RaceDescrip")
	FText RaceDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "RaceIcon")
	FSlateBrush RaceIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "RaceIconPath")
	FName RaceIconPath;

	/////////////////////////// Pawn Attribute ////////////////////////////////

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnName")
	FName PawnName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "PawnCamp")
	EShooterCampType CompType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnDescrip")
	FText PawnDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "HP")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "Mana")
	float Mana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnIcon")
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnIconPath")
	FName IconPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "Skeletal")
	TAssetSubclassOf<USkeletalMesh> Mesh3P_Skeletal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "Material")
	TAssetSubclassOf<UMaterial> Mesh3P_Materials;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "Anim")
	TAssetSubclassOf<UAnimInstance> Mesh3P_Anim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "PawnActor")
	TAssetSubclassOf<AActor> Pawn;

	/** Icon to display */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = RaceEntry, DisplayName = "PawnPreview")
	FSlateBrush PawnMesh;

	// /Game/Blueprints/Pawns/HeroA
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnPath")
	FName PawnPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PawnEntry, DisplayName = "PawnBP")
	TSoftObjectPtr<AActor> PawnBP;
	

	/**
	 * Can be overridden by subclasses; Called whenever the owning data table is imported or re-imported.
	 * Allows for custom fix-ups, parsing, etc. after initial data is read in.
	 *
	 * @param InDataTable					The data table that owns this row
	 * @param InRowName						The name of the row we're performing fix-up on
	 * @param OutCollectedImportProblems	List of problems accumulated during import; Can be added to via this method
	 */
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)override 
	{

	}
};
