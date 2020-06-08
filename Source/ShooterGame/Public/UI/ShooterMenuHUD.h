// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "GameFramework/HUD.h"
#include "UI/Menu/Widgets/SShooterPawnGuideWidget.h"
#include "ShooterMenuHUD.generated.h"

class SShooterPawnGuideWidget;

/**
 * Provides an implementation of the game's in-game Menu HUD.
 **/
UCLASS()
class SHOOTERGAME_API AShooterMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShooterMenuHUD(const FObjectInitializer& ObjectInitializer);

	/**
	 * Initializes the Slate UI and adds it as a widget to the game viewport.
	 **/
	virtual void PostInitializeComponents() override;

	/**
	 * Set state of current match.
	 *
	 * @param	NewState	The new match state.
	 */
	void SetMatchState(EShooterMatchState::Type NewState);

	/** Get state of current match. */
	EShooterMatchState::Type GetMatchState() const;

private:
	/**
	 * Reference to the Game HUD UI.
	 **/
	TSharedPtr<class SShooterPawnGuideWidget> MenuWidgetContainer;

	/** State of match. */
	EShooterMatchState::Type MatchState;

	
	
};
