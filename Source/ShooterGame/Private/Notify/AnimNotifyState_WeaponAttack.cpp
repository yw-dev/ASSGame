// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "AnimNotifyState_WeaponAttack.h"


UAnimNotifyState_WeaponAttack::UAnimNotifyState_WeaponAttack()
{
	MaxAttackDelayCount = 2;
	AttackDelayTime = 0.04f;
	EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
}


void UAnimNotifyState_WeaponAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("WeaponAttack::NotifyBegin()"));
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

void UAnimNotifyState_WeaponAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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

