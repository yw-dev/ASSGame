// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTargetPlayer.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player TargetPlayer Widget(Player：目标player)
 */
UShooterTargetPlayer::UShooterTargetPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterTargetPlayer::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTargetPlayer::NativeConstruct()"));
	Super::NativeConstruct();
}

void UShooterTargetPlayer::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterTargetPlayer::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE





