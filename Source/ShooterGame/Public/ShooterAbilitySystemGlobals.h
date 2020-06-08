// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "ShooterAbilitySystemGlobals.generated.h"

/**
 * 能力分配基类
 */
UCLASS(config = Game)
class SHOOTERGAME_API UShooterAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	UShooterAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

	/** Should allocate a project specific AbilityActorInfo struct. Caller is responsible for deallocation */
	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const;
	
};
