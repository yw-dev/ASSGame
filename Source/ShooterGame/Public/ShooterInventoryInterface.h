// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ShooterItem.h"
#include "ShooterTypes.h"
#include "ShooterInventoryInterface.generated.h"

/**
 * Interface for actors that provide a set of ShooterItems bound to ItemSlots
 * This exists so ShooterCharacterBase can query inventory without doing hacky player controller casts
 * It is designed only for use by native classes
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UShooterInventoryInterface : public UInterface
{
	GENERATED_BODY()
	
};

class SHOOTERGAME_API IShooterInventoryInterface
{
	GENERATED_BODY()

public:
	/** Returns the map of items to data */
	virtual const TMap<UShooterItem*, FShooterItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	virtual const TMap<FShooterItemSlot, UShooterItem*>& GetSlottedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory item changes */
	//virtual FOnInventorySoulsChangedNative& GetInventorySoulsChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
