// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SShooterTabItem.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"


void SShooterTabItem::Construct(const FArguments& InArgs)
{
	ItemStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuItemStyle>("DefaultShooterMenuItemStyle");

	PlayerOwner = InArgs._PlayerOwner;
	Text = InArgs._Text;
	OptionText = InArgs._OptionText;
	OnClicked = InArgs._OnClicked;
	OnArrowPressed = InArgs._OnArrowPressed;
	bIsMultichoice = InArgs._bIsMultichoice;
	bIsActiveMenuItem = false;
	TopArrowVisible = EVisibility::Collapsed;
	BottomArrowVisible = EVisibility::Collapsed;
	//if attribute is set, use its value, otherwise uses default
	InactiveTextAlpha = InArgs._InactiveTextAlpha.Get(1.0f);

	const float ArrowMargin = 3.0f;
	ItemMargin = 10.0f;
	TextColor = FLinearColor(FColor(155, 164, 182));

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(374.0f)
			.HeightOverride(23.0f)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SShooterTabItem::GetButtonBgColor)
				.Image(&ItemStyle->BackgroundBrush)
			]
		]
		+ SOverlay::Slot()
		.HAlign(bIsMultichoice ? HAlign_Left : HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(ItemMargin, 0, 0, 0))
		[
			SAssignNew(TextWidget, STextBlock)
			.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuTextStyle")
			.ColorAndOpacity(this, &SShooterTabItem::GetButtonTextColor)
			.ShadowColorAndOpacity(this, &SShooterTabItem::GetButtonTextShadowColor)
			.Text(Text)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				.Padding(FMargin(0, 0, ArrowMargin, 0))
				.Visibility(this, &SShooterTabItem::GetTopArrowVisibility)
				.OnMouseButtonDown(this, &SShooterTabItem::OnTopArrowDown)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
						.Image(&ItemStyle->TopArrowImage)
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(TAttribute<FMargin>(this, &SShooterTabItem::GetOptionPadding))
			[
				SNew(STextBlock)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuTextStyle")
				.Visibility(bIsMultichoice ? EVisibility::Visible : EVisibility::Collapsed)
				.ColorAndOpacity(this, &SShooterTabItem::GetButtonTextColor)
				.ShadowColorAndOpacity(this, &SShooterTabItem::GetButtonTextShadowColor)
				.Text(OptionText)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				.Padding(FMargin(ArrowMargin, 0, ItemMargin, 0))
				.Visibility(this, &SShooterTabItem::GetBottomArrowVisibility)
				.OnMouseButtonDown(this, &SShooterTabItem::OnBottomArrowDown)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SImage)
						.Image(&ItemStyle->BottomArrowImage)
					]
				]
			]
		]

	];
}

FReply SShooterTabItem::OnBottomArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 MoveRight = 1;
	if (OnArrowPressed.IsBound() && bIsActiveMenuItem)
	{
		OnArrowPressed.Execute(MoveRight);
		Result = FReply::Handled();
	}
	return Result;
}

FReply SShooterTabItem::OnTopArrowDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 MoveLeft = -1;
	if (OnArrowPressed.IsBound() && bIsActiveMenuItem)
	{
		OnArrowPressed.Execute(MoveLeft);
		Result = FReply::Handled();
	}
	return Result;
}

EVisibility SShooterTabItem::GetTopArrowVisibility() const
{
	return TopArrowVisible;
}

EVisibility SShooterTabItem::GetBottomArrowVisibility() const
{
	return BottomArrowVisible;
}

FMargin SShooterTabItem::GetOptionPadding() const
{
	return BottomArrowVisible == EVisibility::Visible ? FMargin(0) : FMargin(0, 0, ItemMargin, 0);
}

FSlateColor SShooterTabItem::GetButtonTextColor() const
{
	FLinearColor Result;
	if (bIsActiveMenuItem)
	{
		Result = TextColor;
	}
	else
	{
		Result = FLinearColor(TextColor.R, TextColor.G, TextColor.B, InactiveTextAlpha);
	}
	return Result;
}

FLinearColor SShooterTabItem::GetButtonTextShadowColor() const
{
	FLinearColor Result;
	if (bIsActiveMenuItem)
	{
		Result = FLinearColor(0, 0, 0, 1);
	}
	else
	{
		Result = FLinearColor(0, 0, 0, InactiveTextAlpha);
	}
	return Result;
}


FSlateColor SShooterTabItem::GetButtonBgColor() const
{
	const float MinAlpha = 0.1f;
	const float MaxAlpha = 1.f;
	const float AnimSpeedModifier = 1.5f;

	float AnimPercent = 0.f;
	ULocalPlayer* const Player = PlayerOwner.Get();
	if (Player)
	{
		// @fixme, need a world get delta time?
		UWorld* const World = Player->GetWorld();
		if (World)
		{
			const float GameTime = World->GetRealTimeSeconds();
			AnimPercent = FMath::Abs(FMath::Sin(GameTime*AnimSpeedModifier));
		}
	}

	const float BgAlpha = bIsActiveMenuItem ? FMath::Lerp(MinAlpha, MaxAlpha, AnimPercent) : 0.f;
	return FLinearColor(1.f, 1.f, 1.f, BgAlpha);
}

FReply SShooterTabItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//execute our "OnClicked" delegate, if we have one
	if (OnClicked.IsBound() == true)
	{
		return OnClicked.Execute();
	}

	return FReply::Handled();
}


FReply SShooterTabItem::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

void SShooterTabItem::SetMenuItemActive(bool bIsMenuItemActive)
{
	this->bIsActiveMenuItem = bIsMenuItemActive;
}

void SShooterTabItem::UpdateItemText(const FText& UpdatedText)
{
	Text = UpdatedText;
	if (TextWidget.IsValid())
	{
		TextWidget->SetText(Text);
	}
}



