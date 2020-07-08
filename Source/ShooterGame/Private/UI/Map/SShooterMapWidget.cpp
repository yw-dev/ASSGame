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
#include "SShooterMapWidget.h"



#define LOCTEXT_NAMESPACE "SShooterMapWidget"

class UShooterBlueprintLibrary;
class UShooterGameUserSettings;
class UAssetManager;


// Sets default values
void SShooterMapWidget::Construct(const FArguments& InArgs)
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
		.Padding(FMargin(OutlineWidth))
		[
			SNew(SBox)
			.WidthOverride(350)
			.HeightOverride(320)
			[
				SNew(SBorder)
				.BorderImage(&MenuStyle->RightBackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.Padding(FMargin(OutlineWidth))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
					.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
					.Image(&MenuStyle->RightBackgroundBrush)
				]
			]
		]
	];
}


SShooterMapWidget::~SShooterMapWidget()
{

}







#undef LOCTEXT_NAMESPACE


