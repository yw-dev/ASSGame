// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Menu/Widgets/SShooterConnectionWidget.h"
#include "UI/Menu/Widgets/SShooterSessionsWidget.h"
#include "UI/Menu/Widgets/SShooterMainViewWidget.h"
#include "ShooterLoginHUD.generated.h"

/**
 *  Server Connection HUD
 */
UCLASS()
class SHOOTERGAME_API AShooterLoginHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShooterLoginHUD(const FObjectInitializer& ObjectInitializer);

	/**
	 * Initializes the Slate UI and adds it as a widget to the game viewport.
	 **/
	virtual void PostInitializeComponents() override;

private:

	void UpdateViewContainer();

private:
	/**
	 * Reference to the Game Connection server HUD UI.
	 **/
	TSharedPtr<class SShooterConnectionWidget> ConnectionWidgetContainer;

	/**
	 * Reference to the Game Connection server HUD UI.
	 **/
	TSharedPtr<class SShooterSessionsWidget> SessionWidgetContainer;

	/**
	 * Reference to the Game Connection server HUD UI.
	 **/
	TSharedPtr<class SShooterMainViewWidget> MainViewContainer;

	/** Scoreboard widget container - used for removing */
	TSharedPtr<class SWidget> ViewWidgetContainer;


	
	
	
	
};
