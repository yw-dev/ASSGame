// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerPhoto.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Photo Info View Widget(Player：头像面板)
 */
UShooterPlayerPhoto::UShooterPlayerPhoto(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterPlayerPhoto::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerPhoto::NativeConstruct()"));
	Super::NativeConstruct();
}

void UShooterPlayerPhoto::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerPhoto::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterPlayerPhoto::UpdatePlayerPhoto(bool IsAlive, FText InDelay, FText InLevel) const
{
	if (IsAlive)
	{
		DieCooldownPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		DieCooldownPanel->SetVisibility(ESlateVisibility::Visible);
		SetDieCooldownLabel(InDelay);
	}
	SetLevelLabel(InLevel);
}

void UShooterPlayerPhoto::SetDieCooldownLabel(FText InDelay) const
{
	DieCooldownLabel->SetText(InDelay);
}

void UShooterPlayerPhoto::SetLevelLabel(FText InLevel) const
{
	LevelLabel->SetText(InLevel);
}
/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
