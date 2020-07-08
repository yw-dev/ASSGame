// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"
#include "Player/ShooterCharacter.h"
#include "SShooterPlayerWidget.h"


#define LOCTEXT_NAMESPACE "SShooterPlayerWidget"

class UShooterBlueprintLibrary;
class UShooterGameUserSettings;
class UAssetManager;


// Sets default values
void SShooterPlayerWidget::Construct(const FArguments& InArgs)
{

	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	PlayerOwner = InArgs._PlayerOwner;
	OwnerWidget = InArgs._OwnerWidget;
	MatchState = InArgs._MatchState.Get();

	//Viewport Size
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	//Viewport Center!
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	PlayerPanelWidth = ViewportSize.X / 3;
	PlayerPanelHeight = ViewportSize.Y / 7;
	OutlineWidth = 10;

	// Layout
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(PlayerPanelWidth)
			.HeightOverride(PlayerPanelHeight)
			[
				SNew(SImage)
				.ColorAndOpacity(FLinearColor::Black)
				.Image(&MenuStyle->HeaderBackgroundBrush)
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(PlayerPanelWidth)
			.HeightOverride(PlayerPanelHeight)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Fill)
				.Padding(FMargin(15, 5, 0, 5))
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(PlayerPanelHeight)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
						.Padding(FMargin(OutlineWidth))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SImage)
							.ColorAndOpacity(FLinearColor(FColor(0.02f, 0.04f, 0.08f, 0.75f)))
							.Image(&MenuStyle->RightBackgroundBrush)
						]
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(5, OutlineWidth))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
									.ColorAndOpacity(FLinearColor::Yellow)
									.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
					]
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(0, OutlineWidth, OutlineWidth, OutlineWidth))
				[
					SNew(SBox)
					.WidthOverride(PlayerPanelHeight)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.BorderImage(&MenuStyle->RightBackgroundBrush)
							.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
							.Padding(FMargin(OutlineWidth))
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
								.ColorAndOpacity(FLinearColor::Yellow)
								.Text(LOCTEXT("MatchRestartTimeString", "2250"))
							]
						]
					]
				]
			]
		]
	];
}


SShooterPlayerWidget::~SShooterPlayerWidget()
{

}







#undef LOCTEXT_NAMESPACE