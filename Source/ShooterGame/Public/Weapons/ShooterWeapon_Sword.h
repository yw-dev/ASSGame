// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "Weapons/ShooterMeleeWeapon.h"
#include "ShooterWeapon_Sword.generated.h"

class AShooterImpactEffect;

/*
USTRUCT()
struct FSwordHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};
*/

USTRUCT()
struct FSwordWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** base weapon spread (degrees) */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	float WeaponSpread;

	/** targeting spread modifier */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	float TargetingSpreadMod;

	/** continuous firing: spread increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	float FiringSpreadIncrement;

	/** continuous firing: max increment */
	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
	float FiringSpreadMax;

	/** weapon range */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float WeaponRange;

	/** damage amount */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 HitDamage;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** hit verification: scale for bounding box of hit actor */
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
	float ClientSideHitLeeway;

	/** hit verification: threshold for dot product between view direction and hit direction */
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
	float AllowedViewDotHitDir;

	/** defaults */
	FSwordWeaponData()
	{
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		DamageType = UDamageType::StaticClass();
		ClientSideHitLeeway = 200.0f;
		AllowedViewDotHitDir = 0.8f;
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class SHOOTERGAME_API AShooterWeapon_Sword : public AShooterMeleeWeapon
{
	GENERATED_BODY()

	AShooterWeapon_Sword(const FObjectInitializer& ObjectInitializer);

protected:

	/** initial setup */
	virtual void BeginPlay() override;

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FSwordWeaponData SwordConfig;

	/** impact effects */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<AShooterImpactEffect> ImpactTemplate;

	FGameplayTag AttackEventTag;

	int32 AttackDelayCount;

	bool EnableAttackDelay;

	bool IsAttacking;

	float AttackDelayTime;

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

public:

	/** Begin Fire on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireBegin(FGameplayTag EventTag, float DelayTime, int32 DelayCount);

	/** End Fire on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireEnd();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	//virtual void FireWeapon() override;
	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void BeginWeaponAttack(FGameplayTag EventTag, float InAttackDelayTime, int32 InAttackDelayCount) override;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void EndWeaponAttack() override;


	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* Actor,class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* Actor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
