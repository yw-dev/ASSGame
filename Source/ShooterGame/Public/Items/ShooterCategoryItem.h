// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ShooterItem.h"
#include "ShooterAssetManager.h"
#include "Entries/ShooterSourceCategoryEntry.h"
#include "ShooterCategoryItem.generated.h"

UCLASS()
class SHOOTERGAME_API UShooterCategoryItem : public UShooterItem
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UShooterCategoryItem()
	{
		ItemType = UShooterAssetManager::CategoryItemType;
	}

	/** Pawn Data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Category)
	FShooterSourceCategoryEntry Category;

};
