// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Engine/DataTable.h"
#include "ShooterWeaponEntry.generated.h"

/*
*	Weapon's Data.(武器类Entry)
*/
USTRUCT(BlueprintType)
struct FShooterWeaponEntry : public FTableRowBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties	
	FShooterWeaponEntry()
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Name")
	FName WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Type")
	EShooterWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Descrip")
	FText WeaponDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Icon")
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "IconPath")
	FName IconPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Actor")
	TAssetSubclassOf<AActor> Weapon;

	/** Icon to display */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Preview")
	FSlateBrush Preview;

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
