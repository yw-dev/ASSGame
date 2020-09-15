// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "SShooterAlertDialog.h"


// Sets default values
void SShooterAlertDialog::Construct(const FArguments& InArgs)
{
	PlayerOwner = InArgs._PlayerOwner;
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	const FShooterMenuItemStyle* ItemStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuItemStyle>("DefaultShooterMenuItemStyle");

	ChildSlot
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SImage)
			.ColorAndOpacity(FLinearColor(FColor(0.02f, 0.04f, 0.08f, 0.75f)))
			.Image(&MenuStyle->HeaderBackgroundBrush)
		]
	];
}

bool SShooterAlertDialog::SupportsKeyboardFocus() const
{
	return true;
}

FReply SShooterAlertDialog::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}


