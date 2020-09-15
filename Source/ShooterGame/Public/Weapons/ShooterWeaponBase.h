// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ShooterTypes.h"
#include "ShooterWeaponBase.generated.h"

class UAnimMontage;
class AShooterCharacter;
class UAudioComponent;
class UParticleSystemComponent;
class UCameraShake;
class UForceFeedbackEffect;
class USoundCue;

/** 
*	武器状态
*/
namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

/**
*	武器近战攻击碰撞信息
*/
USTRUCT()
struct FWeaponHitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};

/**
*	武器远程攻击碰撞信息
*/
USTRUCT()
struct FWeaponRangeHitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};

/*
* Weapon's common attribuites data.
* @ZH-- 武器通用属性数据
*/
USTRUCT(BlueprintType)
struct FWeaponCommonData
{
	GENERATED_USTRUCT_BODY()

	//武器类型
	UPROPERTY(EditAnywhere, Category = Common)
	EShooterWeaponType CurrentWeaponType;

	//武器最高伤害
	UPROPERTY(EditAnywhere, Category = Common)
	int32 MaxDamage;

	//武器最低伤害
	UPROPERTY(EditAnywhere, Category = Common)
	int32 MinDamage;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenShots;

	/** failsafe reload duration if weapon doesn't have any animation for it */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float NoAnimReloadDuration;

	FWeaponCommonData()
	{
		CurrentWeaponType = EShooterWeaponType::None;
		MaxDamage = 0;
		MinDamage = 0;
		TimeBetweenShots = 0.f;
		NoAnimReloadDuration = 0.f;
	}
};

USTRUCT(BlueprintType)
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn3P;
};

UCLASS(Abstract, Blueprintable)
class AShooterWeaponBase : public AActor
{
	GENERATED_BODY()

protected:

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class AShooterCharacter* MyPawn;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	uint32 bIsAttacking : 1;

	float AttackDelayTime;

	int32 AttackDelayCount;

	uint32 bEnableAttackDelay : 1;

	FGameplayTag AttachEventTag;


private:
	/** weapon mesh: 1st person view */
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(VisibleDefaultsOnly, Category = Collision)
	UCapsuleComponent* CapsuleCollision;


protected:
	/** Returns Mesh1P subobject **/
	//FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns Mesh3P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	/** Returns Mesh3P subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCapsuleComponent* GetCapsuleCollision() const { return CapsuleCollision; }

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;


	//////////////////////////////////////////////////////////////////////////
	// Common  Assets
	
	
public:
	// Sets default values for this actor's properties
	AShooterWeaponBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	/**
	 *	Event when this actor overlaps another actor, for example a player walking into a trigger.
	 *	For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 *	Event when an actor no longer overlaps another actor, and they have separated.
	 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	 */
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	/** set the weapon's owning pawn */
	void SetOwningPawn(AShooterCharacter* AShooterCharacter);

	/** weapon data */
	UPROPERTY(EditAnywhere, Category = Config)
	FWeaponCommonData CommonConfig;

	/** get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AShooterCharacter* GetPawnOwner() const;

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get current weapon state */
	//EWeaponState::Type GetCurrentState() const;

	/** get current ammo amount (total) */
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	int32 GetMaxAmmo() const;

	/** get weapon mesh (needs pawn owner to determine variant) */
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** icon displayed on the HUD when weapon is equipped as primary */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon PrimaryIcon;

	/** icon displayed on the HUD when weapon is secondary */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon SecondaryIcon;

	/** bullet icon used to draw current clip (left side) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon PrimaryClipIcon;

	/** bullet icon used to draw secondary clip (left side) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon SecondaryClipIcon;

	/** how many icons to draw per clip */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	float AmmoIconsCount;

	/** defines spacing between primary ammo icons (left side) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	int32 PrimaryClipIconOffset;

	/** defines spacing between secondary ammo icons (left side) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	int32 SecondaryClipIconOffset;

	/** crosshair parts icons (left, top, right, bottom and center) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon Crosshair[5];

	/** crosshair parts icons when targeting (left, top, right, bottom and center) */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon AimingCrosshair[5];

	/** only use red colored center part of aiming crosshair */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	bool UseLaserDot;

	/** false = default crosshair */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	bool UseCustomCrosshair;

	/** false = use custom one if set, otherwise default crosshair */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	bool UseCustomAimingCrosshair;

	/** true - crosshair will not be shown unless aiming with the weapon */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	bool bHideCrosshairWhileNotAiming;

	/** gets last time when this weapon was switched to */
	float GetEquipStartedTime() const;

	/** gets the duration of equipping weapon*/
	float GetEquipDuration() const;

	//UFUNCTION(BlueprintCallable, Category = "Game|Collision")
	//void OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	//UFUNCTION(BlueprintCallable, Category = "Game|Collision")
	//void OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	//////////////////////////////////////////////////////////////////////////
	// Input

public:
	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(AActor* LastWeapon);

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	void BeginWeaponAttack(FGameplayTag EventTag, float InAttackDelayTime, int32 InAttackDelayCount);

	UFUNCTION(BlueprintNativeEvent, Category = "Game|Weapon")
	void EndWeaponAttack();
	

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(AShooterCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	/** attaches weapon mesh to pawn's mesh */
	virtual void AttachMeshToPawn();

	/** attaches weapon mesh to pawn's mesh Backside*/
	virtual void AttachMeshToPawnBackside();

	/** detaches weapon mesh from pawn */
	virtual void DetachMeshFromPawn();


	//////////////////////////////////////////////////////////////////////////
	// Control

	/** check if weapon can fire */
	bool CanFire() const;

	/** update weapon state */
	virtual void SetWeaponState(EWeaponState::Type NewState);

	/** determine current weapon state */
	virtual void DetermineWeaponState();


	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get current weapon state */
	EWeaponState::Type GetCurrentState() const;

	EShooterWeaponType GetCurrentWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combo")
	void SetIsAttacking(bool arg);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combo")
	bool GetIsAttacking() const;

	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	UFUNCTION()
	void OnRep_MyPawn();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local + server] handle weapon fire */
	virtual void HandleFiring();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AShooterWeaponBase::FireWeapon, );


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** play weapon animations */
	float PlayWeaponAnimation(const FWeaponAnim& Animation);

	/** stop playing weapon animations */
	void StopWeaponAnimation(const FWeaponAnim& Animation);



};
