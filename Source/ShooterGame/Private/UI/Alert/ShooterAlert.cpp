// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterAlert.h"


#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

void FShooterAlert::Construct(TWeakObjectPtr<ULocalPlayer> InPlayerOwner, const FText& Message)
{
	PlayerOwner = InPlayerOwner;
	/*
	if (ensure(GameInstance.IsValid()))
	{
		UShooterGameViewportClient* ShooterViewport = Cast<UShooterGameViewportClient>(GameInstance->GetGameViewportClient());

		if (ShooterViewport)
		{
			// Hide the previous dialog
			ShooterViewport->HideDialog();

			// Show the new one
			ShooterViewport->ShowDialog(
				PlayerOwner,
				Message,
			);
		}
	}*/
}

void FShooterAlert::RemoveFromGameViewport()
{
	/*
	if (ensure(GameInstance.IsValid()))
	{
		UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GameInstance->GetGameViewportClient());

		if (ShooterViewport)
		{
			// Hide the previous dialog
			ShooterViewport->HideDialog();
		}
	}*/
}


#undef LOCTEXT_NAMESPACE
