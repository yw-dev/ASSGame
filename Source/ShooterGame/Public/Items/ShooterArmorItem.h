// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ShooterItem.h"
#include "ShooterAssetManager.h"
#include "ShooterArmorItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterArmorItem : public UShooterItem
{
	GENERATED_BODY()
	

public:
	/** Constructor */
	UShooterArmorItem()
	{
		ItemType = UShooterAssetManager::ArmorItemType;
	}

	
};
