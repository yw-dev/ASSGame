// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterGameplayAbility.h"
#include "Abilities/ShooterAbilitySystemComponent.h"
#include "Abilities/ShooterTargetType.h"
#include "ShooterCharacter.h"

/**
 * Subclass of ability blueprint type with game-specific data
 * This class uses GameplayEffectContainers to allow easier execution of gameplay effects based on a triggering tag
 * Most games will need to implement a subclass to support their game-specific code
 */
UShooterGameplayAbility::UShooterGameplayAbility() 
{
}

UAnimMontage* UShooterGameplayAbility::GetActiveAnim(FPlayerAnim AnimInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::GetActiveAnim()"));
	AActor* character = GetOwningActorFromActorInfo();
	AShooterCharacter* Pawn = Cast<AShooterCharacter>(GetOwningActorFromActorInfo());
	if (IsLocallyControlled() && IsPredictingClient())
	{
		//return Pawn->IsFirstPerson() ? AnimInfo.Pawn1P : AnimInfo.Pawn3P;
		return AnimInfo.Pawn3P;
	}
	else if (IsForRemoteClient())
	{
		return AnimInfo.Pawn3P;
	}
	else
	{
		return nullptr;
	}
	//if (Pawn)
	//{
	//	return Pawn->IsFirstPerson() ? AnimInfo.Pawn1P : AnimInfo.Pawn3P;
	//	return AnimInfo.Pawn3P;
	//}
}

FShooterGameplayEffectContainerSpec UShooterGameplayAbility::MakeEffectContainerSpecFromContainer(const FShooterGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::MakeEffectContainerSpecFromContainer()"));
	// First figure out our actor info
	FShooterGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AShooterCharacter* OwningCharacter = Cast<AShooterCharacter>(OwningActor);
	UShooterAbilitySystemComponent* OwningASC = UShooterAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const UShooterTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			AActor* AvatarActor = GetAvatarActorFromActorInfo();
			TargetTypeCDO->GetTargets(OwningCharacter, AvatarActor, EventData, HitResults, TargetActors);
			ReturnSpec.AddTargets(HitResults, TargetActors);
		}

		// If we don't have an override level, use the default ont he ability system component
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			OverrideGameplayLevel = OwningASC->GetDefaultAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}

FShooterGameplayEffectContainerSpec UShooterGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::MakeEffectContainerSpec()"));
	FShooterGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FShooterGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UShooterGameplayAbility::ApplyEffectContainerSpec(const FShooterGameplayEffectContainerSpec& ContainerSpec)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::ApplyEffectContainerSpec()"));
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UShooterGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::ApplyEffectContainer()"));
	FShooterGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}

void UShooterGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::ActivateAbility()"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UShooterGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability::EndAbility()"));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
