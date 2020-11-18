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

	/** get current spread */
	float GetCurrentSpread() const;

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

	/** current spread from continuous firing */
	float CurrentFiringSpread;

	/** [local] weapon Melee physic attack specific fire implementation */
	virtual void WeaponHit(const FHitResult& Impact, const FVector& Origin, const FVector& HitDir, int32 RandomSeed, float ReticleSpread) override;

	/** [local] weapon long distance attack specific fire implementation */
	virtual void WeaponDistanceHit() override;

	/** [local + server] update spread on firing */
	virtual void OnBurstFinished() override;


public:

	/** Begin Fire on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireBegin(FGameplayTag EventTag, float DelayTime, int32 DelayCount);

	/** End Fire on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireEnd();

	//virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	//virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal HitDir, int32 RandomSeed, float ReticleSpread);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	/** [local] weapon specific fire implementation */
	//virtual void FireWeapon() override;
	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void BeginWeaponAttack(FGameplayTag EventTag, float InAttackDelayTime, int32 InAttackDelayCount) override;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void EndWeaponAttack() override;

	/** process the instant hit and notify the server if necessary */
	void ProcessHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** continue processing the instant hit, as if it has been confirmed by the server */
	void ProcessHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** check if weapon should deal damage to actor */
	bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);


	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* Actor,class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* Actor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
