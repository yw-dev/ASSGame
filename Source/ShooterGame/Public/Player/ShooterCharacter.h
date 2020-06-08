// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterTypes.h"
#include "ShooterInventoryInterface.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Notify/AnimNotifyState_JumpSection.h"
#include "Items/ShooterItem.h"
#include "ShooterCharacterBase.h"
#include "Abilities/ShooterAbilityTypes.h"
#include "Abilities/ShooterAttributeSet.h"
#include "Abilities/ShooterGameplayAbility.h"
#include "Abilities/ShooterAbilitySystemComponent.h"
#include "ShooterCharacter.generated.h"

namespace EPawnState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

UENUM(BlueprintType)
enum class EPawnMode : uint8
{
	EFirstP UMETA(DisplayName = "第一人称"),
	EThirdP UMETA(DisplayName = "第三人称"),
	EMax,
};


DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnShooterCharacterWeaponChange, AShooterCharacter*, AShooterWeaponBase* /* new */, AShooterWeaponBase* /*old */);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnShooterCharacterWeaponTypeChange, AShooterCharacter*, EShooterWeaponType);


class UShooterGameplayAbility;
class UGameplayEffect;

USTRUCT()
struct FCharacterAnim
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn1PHit;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn3PHit;
};

UCLASS()
class AShooterCharacter : public AShooterCharacterBase
{
	GENERATED_UCLASS_BODY()

		
public:

	virtual void BeginDestroy() override;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

	/** Update the character. (Running, health etc). */
	virtual void Tick(float DeltaSeconds) override;

	/** cleanup inventory */
	virtual void Destroyed() override;

	/** update mesh for first person view */
	virtual void PawnClientRestart() override;

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) override;

	/** [client] perform PlayerState related setup */
	virtual void OnRep_PlayerState() override;

	/** [server] called to determine if we should pause replication this actor to a specific player */
	virtual bool IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer) override;

	/** [client] called when replication is paused for this actor */
	virtual void OnReplicationPausedChanged(bool bIsReplicationPaused) override;

	/**
	* Add camera pitch to first person mesh.
	*
	*	@param	CameraLocation	Location of the Camera.
	*	@param	CameraRotation	Rotation of the Camera.
	*/
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);

	/** get aim offsets */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	FRotator GetAimOffsets() const;

	/**
	* Check if pawn is enemy if given controller.
	*
	* @param	TestPC	Controller to check against.
	*/
	bool IsEnemyFor(AController* TestPC) const;

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/**
	* [server] add weapon to inventory
	*
	* @param Weapon	Weapon to add.
	*/
	void AddWeapon(FShooterItemSlot ItemSlot, AShooterWeaponBase* Weapon);

	/**
	* [server] remove weapon from inventory
	*
	* @param Weapon	Weapon to remove.
	*/
	void RemoveWeapon(FShooterItemSlot ItemSlot, AShooterWeaponBase* Weapon);

	/**
	* Find in inventory
	*
	* @param WeaponClass	Class of weapon to find.
	*/
	//class AShooterWeaponBase* FindWeapon(TSubclassOf<class AShooterWeaponBase> WeaponClass);

	/**
	* [server + local] equips weapon from inventory
	*
	* @param Weapon	Weapon to equip
	*/
	void EquipWeapon(class AShooterWeaponBase* Weapon);

	//////////////////////////////////////////////////////////////////////////
	// Weapon / Ability usage

	/** [local] starts weapon fire */
	void StartAbilityFire();

	/** [local] stops weapon fire */
	void StopAbilityFire();

	virtual void AbilityFire();

	/** [local] starts weapon fire */
	void StartWeaponFire();

	virtual void DoMeleeAttack() override;

	virtual void DoSkillAttack() override;

	void WeaponAttack();

	/** [local] stops weapon fire */
	void StopWeaponFire();

	/** check if pawn can Use any Melee Ability */
	bool IsUsingMelee();

	/** check if pawn can Use any Ranged Ability */
	bool IsUsingRanged();

	/** check if pawn can Use any Ability */
	bool IsUsingSkill();

	/** check if pawn can Use any Ability */
	bool CanUseAbility();

	/** [server + local] change targeting state */
	void SetTargeting(bool bNewTargeting);

	/** check if pawn can fire weapon */
	bool CanFire() const;

	/** check if pawn can reload weapon */
	bool CanReload() const;

	bool HasWeapon() const;

	//////////////////////////////////////////////////////////////////////////
	// Getters  and  Setters

	/** get mesh component */
	UFUNCTION(BlueprintCallable, Category = "Game|PawnMesh")
	USkeletalMeshComponent* GetPawnMesh() const;

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AShooterWeaponBase* GetWeapon() const;

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** get Sword attach point */
	FName GetSwordAttachPoint() const;

	/** get Sword attach point */
	FName GetBacksideAttachPoint() const;

	/** get weapon taget modifier speed	*/
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	float GetTargetingSpeedModifier() const;

	/** get running state */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	EShooterWeaponType GetCurrentWeaponType();

	/** get running state */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	EPawnMode GetPawnMode();

	/** get the modifier value for running speed */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	float GetRunningSpeedModifier() const;

	/** returns percentage of health when low health effects should start */
	float GetLowHealthPercentage() const;

	//int32 GetMaxHealth() const;

	//////////////////////////////////////////////////////////////////////////
	// Movement

	/** [server + local] change running state */
	void SetRunning(bool bNewRunning, bool bToggle);

	//////////////////////////////////////////////////////////////////////////
	// Animation  Montage

	/** play anim montage */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	/** stop playing montage */
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;

	/** stop playing all montages */
	void StopAllAnimMontages();
	
	UFUNCTION(BlueprintCallable)
	void SetEnableComboPeriod(bool Enable);

	UFUNCTION(BlueprintCallable)
	bool GetEnableComboPeriod();

	UFUNCTION(BlueprintCallable)
	void SetJumpSectionNS(UAnimNotifyState_JumpSection* InNotify);

	UFUNCTION(BlueprintCallable)
	UAnimNotifyState_JumpSection* GetJumpSectionNS();

	// 动作组合(Montage 序列)方式的近战攻击：根据Montage的Section名称随机调用
	UFUNCTION(BlueprintCallable, Category = Animation)
	void JumpSectionForCombo();

	//virtual void JumpSectionForCombo_Implementation();

	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();

	//UFUNCTION(reliable, server, WithValidation)
	//void ServerStartReload();

	//UFUNCTION(reliable, server, WithValidation)
	//void ServerStopReload();

	/** update weapon state */
	void SetPawnState(EPawnState::Type NewState);

	/** determine current weapon state */
	void DeterminePawnState();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	/** setup pawn specific input handlers */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/**
	* Move forward/back
	*
	* @param Val Movment input to apply
	*/
	void MoveForward(float Val);

	/**
	* Strafe right/left
	*
	* @param Val Movment input to apply
	*/
	void MoveRight(float Val);

	/**
	* Move Up/Down in allowed movement modes.
	*
	* @param Val Movment input to apply
	*/
	void MoveUp(float Val);

	/* Frame rate independent turn */
	void TurnAtRate(float Val);

	/* Frame rate independent lookup */
	void LookUpAtRate(float Val);

	/** player pressed start fire action */
	void OnStartFire();

	/** player released start fire action */
	void OnStopFire();

	/** player pressed targeting action */
	void OnStartTargeting();

	/** player released targeting action */
	void OnStopTargeting();

	/** player pressed next weapon action */
	void OnNextWeapon();

	/** player pressed prev weapon action */
	void OnPrevWeapon();

	/** player pressed reload action */
	void OnReload();

	/** player pressed jump action */
	void OnStartJump();

	/** player released jump action */
	void OnStopJump();

	/** player pressed run action */
	void OnStartRunning();

	/** player pressed toggled run action */
	void OnStartRunningToggle();

	/** player released run action */
	void OnStopRunning();

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get current weapon state */
	EPawnState::Type GetCurrentState() const;

	/** Global notification when a pawns weapon changes. Needed for replication graph. Use OnWeaponEquippedDelegate for actor notification */
	static FOnShooterCharacterWeaponChange NotifyWeaponChange;

	/** Global notification when a pawns weapon changes. Needed for replication graph. Use OnWeaponEquippedDelegate for actor notification */
	static FOnShooterCharacterWeaponTypeChange NotifyWeaponTypeChange;

	/** get targeting state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsTargeting() const;

	/** get firing state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsFiring() const;

	/** get running state */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsRunning() const;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category = Mesh)
	virtual bool IsFirstPerson() const;

	/** check if pawn is still alive */
	//UFUNCTION(BlueprintCallable, Category = Pawn)
	//bool IsAlive() const;

	/*
	* Get either first or third person mesh.
	*
	* @param	WantFirstPerson		If true returns the first peron mesh, else returns the third
	*/
	USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;

	/** Update the team color of all player meshes. */
	void UpdateTeamColorsAllMIDs();

private:
	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	
	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName SwordAttachPoint;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName BacksideAttachPoint;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName SwordEffectBegin;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName SwordEffectEnd;

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AShooterWeaponBase>> DefaultInventoryClasses;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	//TMap<FShooterItemSlot, UShooterItem*> SlottedItems;

	/** weapons in inventory , Elements count  only  two.  武器实例化数组，同一角色仅可同时拥有2把武器。 */
	UPROPERTY(Transient, Replicated)
	TMap<FShooterItemSlot, AShooterWeaponBase*> InventoryItems;
	//TArray<class AShooterWeaponBase*> Inventory;

	UPROPERTY(Transient, Replicated)
	TArray<class AShooterWeaponBase*> WaponEquips;

	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AShooterWeaponBase* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
	class AShooterWeaponBase* ActivateWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory)
	FShooterItemSlot ActivateWeaponSlot;

	/** Replicate where this pawn was last hit and damaged */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/** current weapon state */
	EPawnState::Type CurrentState;

	/** Character Field of vision mode */
	EPawnMode CurrentPawnMode;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	float TargetingSpeedModifier;

	/** current targeting state */
	UPROPERTY(Transient, Replicated)
	uint8 bIsTargeting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combo)
	uint8 bEnableComboPeriod : 1;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	float RunningSpeedModifier;

	/** current running state */
	UPROPERTY(Transient, Replicated)
	uint8 bWantsToRun : 1;

	/** from gamepad running is toggled */
	uint8 bWantsToRunToggled : 1;

	/** current firing state */
	uint8 bWantsToFire : 1;

	/** is weapon currently equipped? */
	uint8 bIsEquipped : 1;

	/** is reload animation playing? */
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	uint8 bPendingReload : 1;

	/** is equip animation playing? */
	uint8 bPendingEquip : 1;

	/** weapon is refiring */
	uint8 bRefiring : 1;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** when low health effects should start */
	float LowHealthPercentage;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
	float BaseLookUpRate;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

	/** material instances for setting team color in mesh (3rd person view) */
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> MeshMIDs;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimNotifyState_JumpSection*  JumpSectionNS;

	/** animation played on death */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* DeathAnim;

	/** animation played on Hit */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* HitAnim;

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FCharacterAnim ReactHitAnim;

	/** sound played on death, local player only */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* DeathSound;

	/** effect played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UParticleSystem* RespawnFX;

	/** sound played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RespawnSound;

	/** sound played when health is low */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* LowHealthSound;

	/** sound played when running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RunLoopSound;

	/** sound played when stop running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* RunStopSound;

	/** sound played when targeting state changes */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* TargetingSound;

	/** used to manipulate with run loop sound */
	UPROPERTY()
	UAudioComponent* RunLoopAC;

	/** hook to looped low health sound used to stop/adjust volume */
	UPROPERTY()
	UAudioComponent* LowHealthWarningPlayer;

	/** handles sounds for running */
	void UpdateRunSounds();

	/** handle mesh visibility and updates */
	void UpdatePawnMeshes();

	/** handle mesh colors on specified material instance */
	void UpdateTeamColors(UMaterialInstanceDynamic* UseMID);

	/** Responsible for cleaning up bodies on clients. */
	virtual void TornOff();

	/** Called when slotted items change, bound to delegate on interface */
	virtual void OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item) override;

	virtual void RefreshSlottedGameplayAbilities() override;

private:

	/** Whether or not the character is moving (based on movement input). */
	bool IsMoving();

	//////////////////////////////////////////////////////////////////////////
	// Damage & death

public:

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint8 bIsDying : 1;

	// Current health of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	// Current health of the Pawn
	UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing = OnRep_CurrentWeaponType)
	EShooterWeaponType CurrentWeaponType;

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	/** Pawn suicide */
	virtual void Suicide();

	/** Kill this pawn */
	virtual void KilledBy(class APawn* EventInstigator);

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/**
	* Kills pawn.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	//////////////////////////////////////////////////////////////////////////
	// public Inventory

	virtual const TMap<FShooterItemSlot, class AShooterWeaponBase*>& GetInventoryList() const
	{
		return InventoryItems;
	}

	//TMap<UShooterItem*, class AShooterWeaponBase*> GetInventoryList() const;

	int32 GetInventoryCount() const;

	bool GetInventoryItemSlot(AShooterWeaponBase* Weapon, FShooterItemSlot& ItemSlot) const;

	AShooterWeaponBase* FindWeapon(FShooterItemSlot NewSlot);

	AShooterWeaponBase* FindWeapon(TSubclassOf<AShooterWeaponBase> WeaponCLass);

	//AShooterWeaponBase* GetInventoryWeapon(AShooterWeaponBase* WeaponCLass);

	bool UpdateInventoryActors(class UShooterItem* NewItem, bool bAdd);

	/** [server] spawns Player Inventory Props (生成道具实例) */

	UFUNCTION(BlueprintCallable)
	void SpawnPropsActors();

	/** [server] spawns Player Inventory Props (生成道具实例) */
	void SpawnInventoryActor(UShooterItem* NewItem);

	/** [server] spawns Player Inventory Props (移除道具实例) */
	void RemoveInventoryActor(UShooterItem* removeItem);

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventoryItem(UShooterItem* removeItem);

	void DestroyInventory();



protected:
	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** switch to ragdoll */
	void SetRagdollPhysics();

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	//////////////////////////////////////////////////////////////////////////
	// protected Inventory

	/** updates current weapon */
	UFUNCTION()
	void SetCurrentWeapon(class AShooterWeaponBase* NewWeapon, class AShooterWeaponBase* LastWeapon = NULL);

	UFUNCTION()
	void SetCurrentWeaponType(EShooterWeaponType ActivePose = EShooterWeaponType::None);

	UFUNCTION()
	/** current weapon Rep handler */
	void OnRep_CurrentWeapon(class AShooterWeaponBase* LastWeapon);

	UFUNCTION()
	void OnRep_CurrentWeaponType(EShooterWeaponType weaponType);

	UFUNCTION()
	void OnRep_BurstCounter();

	/** [server] spawns default inventory */
	// void SpawnDefaultInventory();

	/** server Remove weapon */
	UFUNCTION(reliable, server, WithValidation)
	void ServerRemoveWeapon(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon);

	/** Server add weapon */
	UFUNCTION(reliable, server, WithValidation)
	void ServerAddWeapon(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon);

	/** server equip weapon */
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class AShooterWeaponBase* NewWeapon);

	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetTargeting(bool bNewTargeting);

	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetRunning(bool bNewRunning, bool bToggle);

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFiring();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	/** Builds list of points to check for pausing replication for a connection*/
	void BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints);

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

public:
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns current health, will be 0 if dead */
	//UFUNCTION(BlueprintCallable)
	//virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetCurMaxHealth() const;

	/** Returns current mana */
	//UFUNCTION(BlueprintCallable)
	//virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	//UFUNCTION(BlueprintCallable)
	//virtual float GetMaxMana() const;

	/** Returns current movement speed */
	//UFUNCTION(BlueprintCallable)
	//virtual float GetMoveSpeed() const;

	/** Returns the character level that is passed to the ability system */
	//UFUNCTION(BlueprintCallable)
	//virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	//UFUNCTION(BlueprintCallable)
	//virtual bool SetCharacterLevel(int32 NewLevel);

	/**
		* Attempts to activate any ability in the specified item slot. Will return false if no activatable ability found or activation fails
		* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
		* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
		*/
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//bool ActivateAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities bound to the item slot. This only returns if the ability is currently running */
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//void GetActiveAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, TArray<UShooterGameplayAbility*>& ActiveAbilities);

	/**
		* Attempts to activate all abilities that match the specified tags
		* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
		* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
		*/
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UShooterGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	//UFUNCTION(BlueprintCallable, Category = "Abilities")
	//bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

protected:
	/** The level of this character, should not be modified directly once it has already spawned */
	//UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
	//int32 CharacterLevel;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	//TArray<TSubclassOf<UShooterGameplayAbility>> GameplayAbilities;

	/** Map of item slot to gameplay ability class, these are bound before any abilities added by the inventory */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	//TMap<FShooterItemSlot, TSubclassOf<UShooterGameplayAbility>> DefaultSlottedAbilities;

	/** Passive gameplay effects applied on creation */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	//TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** The component used to handle ability system interactions */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	//UShooterAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	//UPROPERTY()
	//UShooterAttributeSet* AttributeSet;

	/** Cached pointer to the inventory source for this character, can be null */
	//UPROPERTY()
	//TScriptInterface<IShooterInventoryInterface> InventorySource;

	/** If true we have initialized our abilities */
	//UPROPERTY()
	//int32 bAbilitiesInitialized;

	/** Map of slot to ability granted by that slot. I may refactor this later */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	//TMap<FShooterItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	/** Delegate handles */
	//FDelegateHandle InventoryUpdateHandle;
	//FDelegateHandle InventoryLoadedHandle;

	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	//virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser) override;
	//virtual void OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	//virtual void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;
	//virtual void OnHealthChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;


	/**
	 * Called when mana is changed, either from healing or from being used as a cost
	 *
	 * @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	//virtual void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;
	//virtual void OnManaChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;

	/**
	 * Called when movement speed is changed
	 *
	 * @param DeltaValue Change in move speed
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	//virtual void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;
	//virtual void OnMoveSpeedChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;

	/** Called when slotted items change, bound to delegate on interface */
	//void OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item);
	//void RefreshSlottedGameplayAbilities();

	/** Apply the startup gameplay abilities and effects */
	//void AddStartupGameplayAbilities();

	/** Attempts to remove any startup gameplay abilities */
	//void RemoveStartupGameplayAbilities();

	/** Adds slotted item abilities if needed */
	//void AddSlottedGameplayAbilities();

	/** Fills in with ability specs, based on defaults and inventory */
	//void FillSlottedAbilitySpecs(TMap<FShooterItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	/** Remove slotted gameplay abilities, if force is false it only removes invalid ones */
	//void RemoveSlottedGameplayAbilities(bool bRemoveAll);

	// Called from ShooterAttributeSet, these call BP events above
	//virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser) override;
	//virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;
	//virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;
	//virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags) override;

	// Friended to allow access to handle functions above
	//friend UShooterAttributeSet;

};


