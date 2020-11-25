// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTeamBar.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Team Header Bar Widget(Player：HUD Header 团队)
 */
UShooterTeamBar::UShooterTeamBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterTeamBar::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTeamBar::NativeConstruct()"));
	Super::NativeConstruct();

	SetGameTime(FText::FromString(TEXT("00:00")));
	SetOwnerScore(FText::AsNumber(0));
	SetTargetScore(FText::AsNumber(0));
}

void UShooterTeamBar::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTeamBar::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}


void UShooterTeamBar::Update(FString InTime, float InOwnerScore, float InTargetScore)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTeamBar::SynchronizeProperties()"));

	SetGameTime(FText::FromString(InTime));
	SetOwnerScore(FText::AsNumber(InOwnerScore));
	SetTargetScore(FText::AsNumber(InTargetScore));
}

void UShooterTeamBar::SetGameTime(FText InTime)
{
	if (TimeLabel)
	{
		TimeLabel->SetText(InTime);
	}	
}

void UShooterTeamBar::SetOwnerScore(FText InScore) 
{
	if (OwnerScoreLabel)
	{
		OwnerScoreLabel->SetText(InScore);
	}
}


void UShooterTeamBar::SetTargetScore(FText InScore)
{
	if (TargetScoreLabel)
	{
		TargetScoreLabel->SetText(InScore);
	}
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE





