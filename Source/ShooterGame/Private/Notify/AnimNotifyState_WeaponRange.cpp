// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AnimNotifyState_WeaponRange.h"


/**
 *  Melee Weapon Attach Montage Notify
 *  远程范围攻击动画通知
 */

UAnimNotifyState_WeaponRange::UAnimNotifyState_WeaponRange()
{
	EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.UseSkill"));
}


void UAnimNotifyState_WeaponRange::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp->GetOwner())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
	}
}
