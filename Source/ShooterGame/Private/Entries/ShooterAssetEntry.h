// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Engine/DataTable.h"
#include "Engine/AssetManager.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "ShooterAssetEntry.generated.h"

/*
*	Assets Data.(资源Entry)
*/
USTRUCT()
struct FShooterAssetEntry
{
	GENERATED_BODY()


	UPROPERTY()
	FPrimaryAssetId AssetId;

	UPROPERTY()
	FText Name;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WeaponEntry, DisplayName = "Type")
	//EShooterWeaponType WeaponType;

	UPROPERTY()
	FText Description;

	UPROPERTY()
	TAssetSubclassOf<AActor> Actor;

	/** Icon to display */
	UPROPERTY()
	FSlateBrush Preview;

	UPROPERTY()
	FSlateBrush Icon;

	UPROPERTY()
	FName IconPath;

	void Init(FPrimaryAssetId InAssetId, FText InName, FText InDescription, FName InIconPath, TAssetSubclassOf<AActor> InActor, FSlateBrush InPreview, FSlateBrush InIcon) // Assign only the vars we wish to replicate
	{
		AssetId = InAssetId;
		Name = InName;
		Description = InDescription;
		IconPath = InIconPath;
		Actor = InActor;
		Preview = InPreview;
		Icon = InIcon;
	}

	void Destroy()
	{
		AssetId = FPrimaryAssetId(TEXT("None"));
		Name.GetEmpty();
		Description.GetEmpty();
		Actor = nullptr;
		Preview = FSlateBrush();
		Icon = FSlateBrush();
		IconPath = FName(TEXT(""));
	}
};
