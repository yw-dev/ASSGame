// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterAssetManager.h"
#include "Items/ShooterItem.h"
#include "ShooterPotionItem.generated.h"

/**
 *  Native base class for potions, should be blueprinted 
 */
UCLASS()
class SHOOTERGAME_API UShooterPotionItem : public UShooterItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UShooterPotionItem()
	{
		ItemType = UShooterAssetManager::PotionItemType;
	}
};
