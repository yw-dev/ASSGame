// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ShooterItem.h"
#include "Entries/ShooterPawnEntry.h"
#include "ShooterAssetManager.h"
#include "Player/ShooterCharacter.h"
#include "ShooterPawnItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERGAME_API UShooterPawnItem : public UShooterItem
{
	GENERATED_BODY()


public:
	/** Constructor */
	UShooterPawnItem()
	{
		ItemType = UShooterAssetManager::PawnItemType;
	}

	/** Pawn Data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	FShooterRaceEntry PawnEntry;

	/** Pawn actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn)
	TSubclassOf<AShooterCharacter> PawnActor;

};
