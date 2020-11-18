// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "ShooterPlayerState.generated.h"

UCLASS()
class AShooterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()


	// Begin APlayerState interface
	/** clear scores */
	virtual void Reset() override;

	/**
	 * Set the team 
	 *
	 * @param	InController	The controller to initialize state with
	 */
	virtual void ClientInitialize(class AController* InController) override;

	virtual void UnregisterPlayerWithSession() override;

	// End APlayerState interface

	/**
	 * Set new team and update pawn. Also updates player character team colors.
	 *
	 * @param	NewTeamNumber	Team we want to be on.
	 */
	void SetTeamNum(int32 NewTeamNumber);

	/** player killed someone */
	void ScoreKill(AShooterPlayerState* Victim, int32 Points);

	/** player died */
	void ScoreDeath(AShooterPlayerState* KilledBy, int32 Points);

	/** player Coins earnings by Props */
	void CalculateCoins(int32 Bounty);

	/** player Coins earnings by Enemy */
	void CalculateCoins(AShooterPlayerState* KilledBy, int32 Bounty);

	/** Returns current health, will be 0 if dead */
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	virtual float GetMaxHealth() const;

	/** Returns maximum health, health will never be greater than this */
	virtual float GetRestoreHealth() const;

	/** Returns current mana */
	virtual float GetMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	virtual float GetMaxMana() const;

	/** Returns maximum mana, mana will never be greater than this */
	virtual float GetRestoreMana() const;

	/** Returns Current Level Exp */
	virtual float GetMaxEXP() const;

	/** Returns Current ProvideExp */
	virtual float GetProvideEXP() const;

	/** Returns current CharacterLevel */
	virtual float GetCharacterLevel() const;

	/** get current team */
	int32 GetTeamNum() const;

	/** get number of kills */
	int32 GetKills() const;

	/** get number of deaths */
	int32 GetDeaths() const;

	/** get number of points */
	float GetScore() const;

	/** get number of bullets fired this match */
	int32 GetNumBulletsFired() const;

	/** get number of rockets fired this match */
	int32 GetNumRocketsFired() const;

	/** get number of Money */
	int32 GetNumCoins() const;

	/** get number of Weapon Items */
	int32 GetNumWeaponItems() const;

	/** get number of Weapon Items */
	int32 GetNumSkillItems() const;

	/** get number of Inventory Items */
	int32 GetNumInventoryItems() const;

	/** get whether the player quit the match */
	bool IsQuitter() const;

	/** gets truncated player name to fit in death log and scoreboards */
	FString GetShortPlayerName() const;

	/** Sends Inventory changed (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutInventory(class AShooterPlayerState* OwnerPlayerState, const UShooterItem* Item, bool bAdd = false);

	/** Sends Coins (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutCoins(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

	/** Sends kill (excluding self) to clients */
	UFUNCTION(Reliable, Client)
	void InformAboutKill(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastDeath(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

	/** broadcast death to local clients */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastConnected(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState);

	/** replicate team colors. Updated the players mesh colors appropriately */
	UFUNCTION()
	void OnRep_TeamColor();

	/** find item Data is existed */
	void FindItemData(UShooterItem* Item, FShooterItemData& ItemData) const;
	/** find item slot is existed */
	void FindItemSlot(UShooterItem* Item, FShooterItemSlot& ItemSlot) const;
	//We don't need stats about amount of ammo fired to be server authenticated, so just increment these with local functions
	void AddBulletsFired(int32 NumBullets);
	void AddRocketsFired(int32 NumRockets);
	void AddSlottedItems(FShooterItemSlot NewSlot, UShooterItem* NewItem);
	void AddInventoryItems(FShooterItemData NewData, UShooterItem* NewItem);
	void AddWeaponItems(FShooterItemData NewData, UShooterItem* NewItem);
	void AddSkillItems(FShooterItemData NewData, UShooterItem* NewItem);
	void RemoveSlottedItems(FShooterItemSlot NewSlot, UShooterItem* NewItem);
	void RemoveInventoryItems(UShooterItem* NewItem);
	void RemoveWeaponItems(UShooterItem* NewItem);
	void RemoveSkillItems(UShooterItem* NewItem);



	/** Set whether the player is a quitter */
	void SetQuitter(bool bInQuitter);

	virtual void CopyProperties(class APlayerState* PlayerState) override;

public:
	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UShooterAttributeSet* GetAttributeSetBase() const;


protected:

	UPROPERTY()
	class UShooterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UShooterAttributeSet* AttributeSetBase;

	/** Set the mesh colors based on the current teamnum variable */
	void UpdateTeamColors();

	/** Set the mesh colors based on the current teamnum variable */
	void UpdateInventorySlotted(FShooterItemSlot NewSlot, UShooterItem* NewItem);

	/** team number */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_TeamColor)
	int32 TeamNumber;

	/** number of Money */
	UPROPERTY(Transient, Replicated)
	int32 NumCoins;

	/** number of kills */
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/** number of deaths */
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/** number of bullets fired this match */
	UPROPERTY()
	int32 NumBulletsFired;

	/** number of rockets fired this match */
	UPROPERTY()
	int32 NumRocketsFired;

	/** whether the user quit the match */
	UPROPERTY()
	uint8 bQuitter : 1;

	UPROPERTY()
	TMap<FShooterItemSlot, UShooterItem*> SlottedItems;

	/** Map of Inventory items owned by this player, from definition to data */
	UPROPERTY()
	TMap<UShooterItem*, FShooterItemData> InventorySlot;

	/** Map of Weapon items owned by this player, from definition to data */
	UPROPERTY()
	TMap<UShooterItem*, FShooterItemData> WeaponSlot;

	/** Map of Ability items owned by this player, from definition to data */
	UPROPERTY()
	TMap<UShooterItem*, FShooterItemData> SkillSlot;

	/** helper for scoring points */
	void ScorePoints(int32 Points);
};
