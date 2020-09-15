// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Engine/DataTable.h"
#include "ShooterSourceCategoryEntry.generated.h"

USTRUCT(BlueprintType)
struct FShooterSourceCategoryEntry : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties	
	FShooterSourceCategoryEntry()
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CategoryEntry, DisplayName = "Name")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CategoryEntry, DisplayName = "Type")
	FPrimaryAssetType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CategoryEntry, DisplayName = "Description")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CategoryEntry, DisplayName = "Icon")
	FSlateBrush Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = CategoryEntry, DisplayName = "IconPath")
	FName IconPath;
		
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
