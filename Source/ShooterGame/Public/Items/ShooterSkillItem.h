// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterAssetManager.h"
#include "Items/ShooterItem.h"
#include "ShooterSkillItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterSkillItem : public UShooterItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UShooterSkillItem()
	{
		ItemType = UShooterAssetManager::SkillItemType;
	}
};
