// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Abilities/ShooterGameplayAbility.h"
#include "ShooterCharacter.h"
#include "AbilitySystemGlobals.h"
#include "ShooterAbilitySystemComponent.h"



UShooterAbilitySystemComponent::UShooterAbilitySystemComponent() {}

void UShooterAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UShooterGameplayAbility*>& ActiveAbilities)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("GetActiveAbilitiesWithTags()"));
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("GetActiveAbilitiesWithTags()"));
		if (Spec)
		{
			// Iterate all instances on this ability spec
			TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

			for (UGameplayAbility* ActiveAbility : AbilityInstances)
			{
				ActiveAbilities.Add(Cast<UShooterGameplayAbility>(ActiveAbility));
			}
		}
	}
}

int32 UShooterAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	AShooterCharacter* OwningCharacter = Cast<AShooterCharacter>(OwnerActor);

	if (OwningCharacter)
	{
		//return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

UShooterAbilitySystemComponent* UShooterAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UShooterAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}


