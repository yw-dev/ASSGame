// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "AnimNotifyState_WeaponMelee.h"


/**
 *  Melee Weapon Attach Montage Notify
 *  近战攻击动画通知
 */
UAnimNotifyState_WeaponMelee::UAnimNotifyState_WeaponMelee()
{
	MaxAttackDelayCount = 2;
	AttackDelayTime = 0.04f;
	EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
}


void UAnimNotifyState_WeaponMelee::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AShooterCharacter* Pawn = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (Pawn)
	{
		AShooterWeaponBase* Weapon = Pawn->GetWeapon();
		if (Weapon)
		{
			Weapon->SetIsAttacking(true);
			Weapon->BeginWeaponAttack(EventTag, AttackDelayTime, MaxAttackDelayCount);
		}
	}
}

void UAnimNotifyState_WeaponMelee::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AShooterCharacter* Pawn = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (Pawn)
	{
		AShooterWeaponBase* Weapon = Pawn->GetWeapon();
		if (Weapon)
		{
			Weapon->EndWeaponAttack();
		}
	}
}


