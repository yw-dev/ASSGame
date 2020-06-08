// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "ShooterTypes.h"
#include "ShooterInventoryInterface.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "AbilitySystemGlobals.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Abilities/ShooterAbilitySystemComponent.h"
#include "Abilities/ShooterAttributeSet.h"
#include "ShooterCharacterBase.generated.h"

class UShooterGameplayAbility;
class UGameplayEffect;

UCLASS()
class SHOOTERGAME_API AShooterCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()


	/*	GENERATED_UCLASS_BODY(): not need Constructor, because child class can extends parent class Constructor.
	*	GENERATED_BODY(): must be need Constructor, because child class can't extends parent class Constructor.
	*/

public:
	// Constructor and overrides
	//AShooterCharacterBase();

	virtual void BeginDestroy() override;

	/** cleanup inventory */
	virtual void Destroyed() override;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

	/** Update the character. (Running, health etc). */
	virtual void Tick(float DeltaSeconds) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ReceivePossessed_Implementation(AController* NewController);

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** check if pawn is still alive */
	UFUNCTION(BlueprintCallable, Category = Health)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool IsUsingMelee();

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool IsUsingSkill();

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool CanUseAnyAbility();

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool IsPlayHighPriorityMontage();

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

	/** Returns current mana */
	UFUNCTION(BlueprintCallable)
	virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana() const;

	/** Returns current movement speed */
	UFUNCTION(BlueprintCallable)
	virtual float GetMoveSpeed() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

	/** Called when slotted items change, bound to delegate on interface */
	virtual void OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item);

	/** Called when slotted items change, bound to delegate on interface */
	virtual void OnInventoryItemChanged(UShooterItem* item, bool bAdded);

	virtual void RefreshSlottedGameplayAbilities();

	/**
	 * Attempts to activate any ability in the specified item slot. Will return false if no activatable ability found or activation fails
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities bound to the item slot. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, TArray<UShooterGameplayAbility*>& ActiveAbilities);

	/**
	 * Attempts to activate all abilities that match the specified tags
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UShooterGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);

protected:

	/** currently equipped weapon */
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	//class AShooterWeaponBase* CurrentWeapon;

	// Current health of the Pawn
	//UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing = OnRep_CurrentWeaponType)
	//EShooterWeaponType CurrentWeaponType;

	UPROPERTY(EditAnywhere, Category = Attack)
	FShooterItemSlot CurrentWeaponSlot;

	UPROPERTY(EditAnywhere, Category = Attack)
	int32 AttackDelayCount;

	UPROPERTY(EditAnywhere, Category = Attack)
	float AttackDelayTime;

	UPROPERTY(EditAnywhere, Category = Attack)
	FName MeleeStartSection;

	/* 是否受盾保护 */
	UPROPERTY(EditAnywhere, Category = Health)
	uint8 IsProtectedByShield : 1;

	/* 是否无法战胜 */
	UPROPERTY(EditAnywhere, Category = Health)
	uint8 bInvincible : 1;

	/* 高优先级动画 */
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* HighPriorityMontage;

	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
	int32 CharacterLevel;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UShooterGameplayAbility>> GameplayAbilities;

	/** Map of item slot to gameplay ability class, these are bound before any abilities added by the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<FShooterItemSlot, TSubclassOf<UShooterGameplayAbility>> DefaultSlottedAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** The component used to handle ability system interactions */
	UPROPERTY()
	UShooterAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UShooterAttributeSet* AttributeSet;

	/** Cached pointer to the inventory source for this character, can be null */
	UPROPERTY()
	TScriptInterface<IShooterInventoryInterface> InventorySource;

	/** If true we have initialized our abilities */
	UPROPERTY()
	int32 bAbilitiesInitialized;

	/** Map of slot to ability granted by that slot. I may refactor this later */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FShooterItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	/** Delegate handles */
	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;

	UFUNCTION(BlueprintCallable, Category = Animation)
	void PlayHighPriorityMontage(UAnimMontage* Montage, FName SectionName);

	/** current weapon rep handler */
	//UFUNCTION()
	//virtual void OnRep_CurrentWeapon(class AShooterWeaponBase* LastWeapon);

	/** current weaponType rep handler */
	//UFUNCTION()
	//virtual void OnRep_CurrentWeaponType(EShooterWeaponType weaponType);

	virtual void DoMeleeAttack();

	virtual void DoSkillAttack();

	/** updates current weapon */
	//virtual void SetCurrentWeapon(class AShooterWeaponBase* NewWeapon, class AShooterWeaponBase* LastWeapon = NULL);

	//virtual void SetCurrentWeaponType(EShooterWeaponType activePose = EShooterWeaponType::None);

	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param EventInstigator The player that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Game|Player")
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser);
	//void OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser);

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Game|Player")
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	//void OnHealthChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called when mana is changed, either from healing or from being used as a cost
	 *
	 * @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Game|Player")
	void OnManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	//void OnManaChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called when movement speed is changed
	 *
	 * @param DeltaValue Change in move speed
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Game|Player")
	void OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	//void OnMoveSpeedChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/** Apply the startup gameplay abilities and effects */
	void AddStartupGameplayAbilities();

	/** Attempts to remove any startup gameplay abilities */
	void RemoveStartupGameplayAbilities();

	/** Adds slotted item abilities if needed */
	void AddSlottedGameplayAbilities();

	/** Fills in with ability specs, based on defaults and inventory */
	void FillSlottedAbilitySpecs(TMap<FShooterItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	/** Remove slotted gameplay abilities, if force is false it only removes invalid ones */
	void RemoveSlottedGameplayAbilities(bool bRemoveAll);

	// Called from ShooterAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	virtual void HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Friended to allow access to handle functions above
	friend UShooterAttributeSet;
	
};
