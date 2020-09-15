// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ShooterWeaponBase.h"
#include "ShooterMeleeWeapon.generated.h"

/**
*	近战武器基本数据
*/
USTRUCT()
struct FWeaponMeleeData
{
	GENERATED_USTRUCT_BODY()

	/** inifite ammo for reloads */
	UPROPERTY(EditDefaultsOnly, Category = MeleeWeapon)
	bool bAttacking;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category = MeleeWeapon)
	bool EnableAttackDelay;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = MeleeWeapon)
	int32 AttackDelayCount;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = MeleeWeapon)
	float AttackDelayTime;

	/** defaults */
	FWeaponMeleeData()
	{
		bAttacking = false;
		EnableAttackDelay = true;
		AttackDelayCount = 1;
		AttackDelayTime = 0.06f;
	}
};

/**
 *  近战类道具基类
 */
UCLASS(Blueprintable)
class AShooterMeleeWeapon : public AShooterWeaponBase
{
	GENERATED_BODY()

private:

	/** weapon mesh: 1st person view */
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* Mesh3P;

	//UPROPERTY(VisibleDefaultsOnly, Category = Collision)
	//UCapsuleComponent* CapsuleCollision;

protected:

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponMeleeData MeleeConfig;

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	/*
	/** name of bone/socket for muzzle in weapon mesh */
	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//FName SwordAttachBegin;

	/** name of bone/socket for muzzle in weapon mesh */
	//UPROPERTY(EditDefaultsOnly, Category = Effects)
	//FName SwordAttachEnd;
	
	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<UCameraShake> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect *FireForceFeedback;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* EquipSound;

	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim FireAnim;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;


public:
	AShooterMeleeWeapon(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void BeginWeaponAttack(FGameplayTag EventTag, float AttackDelayTime, int32 AttackDelayCount) override;

	//UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	//void EndWeaponAttack() override;


	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(AActor* LastWeapon) override;

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished() override;

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip() override;

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory() override;

	void AttachActorToPawn(AActor* Weapon, FName AttachPoint);

	/** attaches weapon mesh to pawn's mesh Back*/
	virtual void AttachMeshToPawn() override;

	/** attaches weapon mesh to pawn's mesh Backside*/
	virtual void AttachMeshToPawnBackside() override;

	/** detaches weapon mesh from pawn */
	virtual void DetachMeshFromPawn() override;


	//////////////////////////////////////////////////////////////////////////
	// Input

	/** [local + server] start weapon fire */
	virtual void StartFire() override;

	/** [local + server] stop weapon fire */
	virtual void StopFire() override;

	//////////////////////////////////////////////////////////////////////////
	// Control

	/** weapon is refiring */
	uint32 bRefiring;



	//////////////////////////////////////////////////////////////////////////
	// Reading data


	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();


	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	//UFUNCTION()
	//void OnRep_MyPawn();

	UFUNCTION()
	void OnRep_BurstCounter();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire() override;

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire() override;


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AShooterMeleeWeapon::FireWeapon, );

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFiring();

	/** [local + server] handle weapon fire */
	virtual void HandleFiring() override;

	/** [local + server] firing started */
	virtual void OnBurstStarted() override;

	/** [local + server] firing finished */
	virtual void OnBurstFinished() override;

	/** update weapon state */
	virtual void SetWeaponState(EWeaponState::Type NewState) override;

	/** determine current weapon state */
	virtual void DetermineWeaponState() override;

};
