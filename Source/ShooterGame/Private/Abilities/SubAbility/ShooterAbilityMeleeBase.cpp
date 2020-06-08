// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "ShooterAT_PlayMontage.h"
#include "ShooterAbilityMeleeBase.h"



UShooterAbilityMeleeBase::UShooterAbilityMeleeBase() 
{
	TagType = UGameplayTagsManager::Get().RequestGameplayTag(FName(TEXT("Event.Montage")));
}

void UShooterAbilityMeleeBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AbilityMeleeBase::ActivateAbility()"));
	//Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))		// ..then commit the ability...
	{
		AShooterCharacter* Pawn = Cast<AShooterCharacter>(ActorInfo->OwnerActor);
		if (Pawn)
		{
			UAnimMontage* UseAnim = Pawn->IsFirstPerson() ? MontageToPlay.Pawn1P : MontageToPlay.Pawn3P;
			if (UseAnim)
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(TagType);
				// FName(NAME_None) = FName(TEXT("")
				UShooterAT_PlayMontage*  AT_PlayMontage = UShooterAT_PlayMontage::PlayMontageAndWaitForEvent(this, FName(TEXT("None")), UseAnim, TagContainer, 1.f, FName(TEXT("None")), true, 1.f);
				//AT_PlayMontage->OnCompleted.AddUObject(this, &UShooterAbilityMeleeBase::EndAbility);
				//AT_PlayMontage->OnBlendOut.AddUObject(this, &UShooterAbilityMeleeBase::EndAbility);
				//AT_PlayMontage->OnInterrupted.AddUObject(this, &UShooterAbilityMeleeBase::EndAbility);
				//AT_PlayMontage->OnCancelled.AddUObject(this, &UShooterAbilityMeleeBase::EndAbility);
				//AT_PlayMontage->EventReceived.AddUObject(this, &UShooterAbilityMeleeBase::OnGameplayEvent);
			}
		}
	}
}

void UShooterAbilityMeleeBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UShooterAbilityMeleeBase::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	K2_EndAbility();
}

void UShooterAbilityMeleeBase::OnAbilityCancelled()
{

}

void UShooterAbilityMeleeBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void UShooterAbilityMeleeBase::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	FGameplayEventData TempData = *Payload;
	TempData.EventTag = EventTag;

	ApplyEffectContainer(EventTag, TempData, -1);
}
