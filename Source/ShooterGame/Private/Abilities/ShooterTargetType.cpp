// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Engine/EngineTypes.h"
#include "GameplayAbilityTypes.h"
#include "Abilities/ShooterGameplayAbility.h"
#include "Player/ShooterCharacter.h"
#include "ShooterTargetType.h"



void UShooterTargetType::GetTargets_Implementation(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UShooterTargetType_UseOwner::GetTargets_Implementation(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void UShooterTargetType_UseEventData::GetTargets_Implementation(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}
}

