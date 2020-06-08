// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterAssetManager.h"
#include "Items/ShooterItem.h"
#include "ShooterTokenItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterTokenItem : public UShooterItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UShooterTokenItem()
	{
		ItemType = UShooterAssetManager::TokenItemType;
	}
	
};
