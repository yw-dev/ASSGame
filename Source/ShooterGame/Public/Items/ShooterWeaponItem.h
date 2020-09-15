// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Entries/ShooterWeaponEntry.h"
#include "Items/ShooterItem.h"
#include "ShooterAssetManager.h"
#include "ShooterWeaponItem.generated.h"

/**
 *  Native base class for weapons, should be blueprinted 
 */
UCLASS()
class SHOOTERGAME_API UShooterWeaponItem : public UShooterItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UShooterWeaponItem()
	{
		ItemType = UShooterAssetManager::WeaponItemType;
	}

	/** Pawn Data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	FShooterWeaponEntry ActorEntry;

	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AShooterWeaponBase> WeaponActor;
};
