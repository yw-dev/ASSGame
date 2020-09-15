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
}

void UShooterTeamBar::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTeamBar::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE





