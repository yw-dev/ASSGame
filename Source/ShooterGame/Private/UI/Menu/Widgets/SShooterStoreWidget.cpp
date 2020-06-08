// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "SShooterStoreWidget.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "Online.h"
#include "OnlineExternalUIInterface.h"
#include "ShooterGameInstance.h"


//#define LOCTEXT_NAMESPACE "SShooterStoreWidget"

// Sets default values
void SShooterStoreWidget::Construct(const FArguments& InArgs)
{
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	PlayerOwner = InArgs._PlayerOwner;
	bStoreMenu = InArgs._IsStoreMenu;
	//Visibility.Bind(this, &SShooterStoreWidget::GetSlateVisibility);

	FLinearColor MenuTitleTextColor = FLinearColor(FColor(155, 164, 182));


	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(300, 200)
		[
			SNew(SBox)
			.WidthOverride(500)
			.HeightOverride(400)
			[
				SNew(SImage)
				.ColorAndOpacity(FLinearColor::White)
				.Image(&MenuStyle->HeaderBackgroundBrush)
			]
		]
	];
}

// Called every frame
void SShooterStoreWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	//Super::Tick(DeltaTime);

}

void SShooterStoreWidget::OnCloseButtonClick()
{

}