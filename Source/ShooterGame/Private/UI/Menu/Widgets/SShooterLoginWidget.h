// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterMenuHelper.h"

class SShooterLoginWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShooterLoginWidget)
		: _PlayerOwner()
		, _IsGameMenu(false)
	{
	}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

		/** is this main menu or in game menu? */
		SLATE_ARGUMENT(bool, IsGameMenu)

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

	/** to have the mouse cursor show up at all times, we need the widget to handle all mouse events */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** key down handler */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** disable/enable moving around menu */
	void LockControls(bool bEnable);


private:

	/**
	 * Called when the 'Find' context button is clicked
	 */
	void OnConnection();

	int32 GetOwnerUserIndex();

	/** gets header image color */
	FSlateColor GetBackgroundColor() const;

	/** weak pointer to our parent PC */
	TWeakObjectPtr<class ULocalPlayer> PlayerOwner;

	/** if this is in game menu, do not show background or logo */
	bool bGameMenu;

	/** if moving around menu is currently locked */
	bool bControlsLocked;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;
	
};
