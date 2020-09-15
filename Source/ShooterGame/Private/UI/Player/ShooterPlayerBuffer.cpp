// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerBuffer.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Photo Info View Widget(Player：Buffer和DeBuffer面板)
 */
UShooterPlayerBuffer::UShooterPlayerBuffer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterPlayerBuffer::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerBuffer::NativeConstruct()"));
	Super::NativeConstruct();
}

void UShooterPlayerBuffer::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerBuffer::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE







