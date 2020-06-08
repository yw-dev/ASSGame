// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/ShooterPickup.h"
#include "ShooterPickup_Equips.generated.h"

class AShooterCharacter;
class AShooterWeaponBase;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class SHOOTERGAME_API AShooterPickup_Equips : public AShooterPickup
{
	GENERATED_BODY()

	AShooterPickup_Equips(const FObjectInitializer& ObjectInitializer);


protected:

	/** which weapon gets ammo? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	TSubclassOf<AShooterWeaponBase> WeaponType;

	/** give pickup */
	virtual void GivePickupTo(AShooterCharacter* Pawn) override;
	
	
};
