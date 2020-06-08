// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WeaponAttack.generated.h"

/**
 *  
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories, meta = (ShowWorldContextPin, DisplayName = "ShooterWeaponAttack"))
class SHOOTERGAME_API UAnimNotifyState_WeaponAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponAttack)
	int32 MaxAttackDelayCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponAttack)
	float AttackDelayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponAttack)
	FGameplayTag EventTag;

public:
	UAnimNotifyState_WeaponAttack();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	//virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
