// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterMenuHelper.h"


class SShooterStoreWidget : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SShooterStoreWidget)
		: _PlayerOwner()
		, _IsStoreMenu(false)
	{
	}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

		/** is this main menu or in game menu? */
		SLATE_ARGUMENT(bool, IsStoreMenu)

		/** always goes here */
		SLATE_END_ARGS()

		/** delegate declaration */
		DECLARE_DELEGATE(FOnMenuHidden);

	/** external delegate to call when in-game menu should be hidden using controller buttons -
	it's workaround as when joystick is captured, even when sending FReply::Unhandled, binding does not recieve input :( */
	DECLARE_DELEGATE(FOnToggleMenu);

	/** called when user is going back from submenu, useful for resetting changes if they were not confirmed */
	DECLARE_DELEGATE_OneParam(FOnMenuGoBack, MenuPtr);

	/** every widget needs a construction function */
	void Construct(const FArguments& InArgs);

	/** update function. Kind of a hack. Allows us to only start fading in once we are done loading. */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;


private:

	/** sets hit test invisibility when console is up */
	//EVisibility GetSlateVisibility() const;

	void OnCloseButtonClick();

	/** if this is in game menu, do not show background or logo */
	bool bStoreMenu;

	/** weak pointer to our parent PC */
	TWeakObjectPtr<class ULocalPlayer> PlayerOwner;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;
	
};
