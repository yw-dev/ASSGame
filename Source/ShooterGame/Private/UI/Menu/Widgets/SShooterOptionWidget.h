// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 *  游戏系统设置
 */
class SShooterOptionWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SShooterOptionWidget)
	{}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

		SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)

		/** always goes here */
		SLATE_END_ARGS()

		/** if we want to receive focus */
		virtual bool SupportsKeyboardFocus() const override { return true; }

	/** every widget needs a construction function */
	void Construct(const FArguments& InArgs);

	/**
	 * Ticks this widget.  Override in derived classes, but always call the parent implementation.
	 *
	 * @param  AllottedGeometry The space allotted for this widget
	 * @param  InCurrentTime  Current absolute real time
	 * @param  InDeltaTime  Real time passed since last tick
	 */
	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	FSlateColor GetButtonTextColor() const;

	FLinearColor GetButtonTextShadowColor() const;

	FSlateColor GetButtonBgColor() const;


private:

	/** size of standard column in pixels */
	int32 BoxWidth;

	/** active item flag */
	bool bIsActiveMenu;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;

	/** pointer to our owner PC */
	TWeakObjectPtr<class ULocalPlayer> PlayerOwner;

	/** pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;


	
	
};
