// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Abilities/ShooterGameplayAbility.h"
#include "ShooterAbilityMeleeBase.generated.h"

/**
 *  ��ս����Ability����
 */
UCLASS()
class SHOOTERGAME_API UShooterAbilityMeleeBase : public UShooterGameplayAbility
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UShooterAbilityMeleeBase();

	UPROPERTY(EditDefaultsOnly, Category = AbilityMeleeBase)
	FPlayerAnim MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = AbilityMeleeBase)
	FGameplayTag TagType;

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);


};
