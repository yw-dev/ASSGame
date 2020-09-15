// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class FShooterAlert : public TSharedFromThis<FShooterAlert>
{

public:
	/** build menu */
	void Construct(TWeakObjectPtr<ULocalPlayer> InPlayerOwner, const FText& Message);

	/** Remove from the gameviewport. */
	void RemoveFromGameViewport();

private:

	/** Local player that will have focus of the dialog box (can be NULL) */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	
	
};
