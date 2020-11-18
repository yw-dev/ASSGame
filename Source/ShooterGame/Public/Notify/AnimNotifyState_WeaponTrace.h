// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WeaponTrace.generated.h"

/**
 *  Weapon Anim Montage Trace Notify
 *  Weapon动画射线检测通知（Weapon动画射线检测通知）
 */
UCLASS(editinlinenew, Blueprintable, const, hidecategories = Object, collapsecategories, meta = (ShowWorldContextPin, DisplayName = "ShooterWeaponTrace"))
class SHOOTERGAME_API UAnimNotifyState_WeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	TArray<struct FHitResult> HitResults;//保存命中结果//

	TArray<AActor*>HitActors;//保存命中的Actor//

	AController*EventInstigator;//ApplyDamage函数参数//

	TSubclassOf<UDamageType>DamageTypeClass;//ApplyDamage函数参数//

	FVector TraceLocation1;

	FVector TraceLocation2;

	FVector TraceLocation3;

	FVector TraceLocation4;

	class AShooterCharacter* Player;//当前玩家//

	class AShooterWeaponBase* CurrentWeapon;//当前武器//

	class USkeletalMeshComponent* WeaponMesh;

	TArray<AActor*>ActorsToIgnore;//射线忽略目标//

	//自带函数重写//
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)override;//通知开始//

	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)override;//Tick//

	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)override;//通知结束//
};
