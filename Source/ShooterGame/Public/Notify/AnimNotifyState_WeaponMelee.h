// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WeaponMelee.generated.h"

/**
 *  Melee Weapon Attach Montage Notify
 *  ��ս��������֪ͨ
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories, meta = (ShowWorldContextPin, DisplayName = "ShooterWeaponMelee"))
class SHOOTERGAME_API UAnimNotifyState_WeaponMelee : public UAnimNotifyState
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeAttack)
	int32 MaxAttackDelayCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeAttack)
	float AttackDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeleeAttack)
	FGameplayTag EventTag;

public:
	UAnimNotifyState_WeaponMelee();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	//virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
