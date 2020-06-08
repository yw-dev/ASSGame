// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Engine/LocalPlayer.h"

/*
*  …ÃµÍ≤Àµ•
*/
class FShooterStore : public TSharedFromThis<FShooterStore>
{
	/** sets owning player controller */
	void Construct(ULocalPlayer* PlayerOwner);

	/** toggles in game menu */
	void ToggleStoreMenu();

	/** is game menu currently active? */
	bool IsActive() const;


protected:

	/** Owning player controller */
	ULocalPlayer* PlayerOwner;

	/** game menu container widget - used for removing */
	TSharedPtr<class SWeakWidget> StoreMenuContainer;

};
