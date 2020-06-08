// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterAbilitySystemGlobals.h"


UShooterAbilitySystemGlobals::UShooterAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FGameplayAbilityActorInfo* UShooterAbilitySystemGlobals::AllocAbilityActorInfo() const
{

	return nullptr;
}
