// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Slate.h"
#include "ShooterTypes.h"
#include "ShooterMenuHUD.h"
#include "ShooterGameInstance.h"


class SShooterPlayerWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SShooterPlayerWidget)
	{
	}

	//SLATE_ARGUMENT(TWeakObjectPtr<class AShooterMenuHUD>, OwnerHUD);
	SLATE_ARGUMENT(TWeakObjectPtr<APlayerController>, PlayerOwner)

	SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)

	SLATE_ATTRIBUTE(EShooterMatchState::Type, MatchState)

	SLATE_END_ARGS()


public:

	/** play menu item delegate */
	DECLARE_DELEGATE(FOnBeginPlayMenu);

	DECLARE_DELEGATE(FOnCategoryChanged)
	DECLARE_DELEGATE(FOnContentSourcesChanged);
	DECLARE_DELEGATE(FOnSelectedContentSourceChanged);

	/**
	 * Constructs and lays out the Player HUD UI Widget.
	 *
	 * \args Arguments structure that contains widget-specific setup information.
	 **/
	void Construct(const FArguments& args);

	/** Destructor */
	~SShooterPlayerWidget();


protected:

	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** Owning player */
	TWeakObjectPtr<class APlayerController> PlayerOwner;

	/** pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;

	/** get state of current match */
	EShooterMatchState::Type MatchState;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;

	float OutlineWidth;
	float PlayerPanelWidth;
	float PlayerPanelHeight;

public:	


	
	
};
