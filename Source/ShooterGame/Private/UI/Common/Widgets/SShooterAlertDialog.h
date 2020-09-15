// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterStyle.h"
#include "ShooterGame.h"
#include "ShooterTypes.h"

/**
*	Server/Client Message Alert View Widget.
*	消息弹出框
*/
class SShooterAlertDialog : public SCompoundWidget
{

public:
	/** The player that owns the dialog. */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	SLATE_BEGIN_ARGS(SShooterAlertDialog)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

		SLATE_ARGUMENT(FText, MessageText)

		SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

private:

	/** style for the message menu widget */
	const struct FShooterMenuStyle *MenuStyle;
	
	
};
