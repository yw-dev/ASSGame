// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_JumpSection.generated.h"

/**
 *  Anim Montage  Section dispatcher Notify
 *  角色动画片段跳转通知（技能连击动画）
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories, meta = (ShowWorldContextPin, DisplayName = "ShooterJumpSection"))
class SHOOTERGAME_API UAnimNotifyState_JumpSection : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JumpSection)
	uint8 bInstantJump : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JumpSection)
	TArray<FName> JumpSections;
	
public:
	UAnimNotifyState_JumpSection();

	UFUNCTION(BlueprintCallable)
	TArray<FName> GetJumpSetctions();

	UFUNCTION(BlueprintCallable)
	bool IsInstantJump();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	
};
