// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ShooterAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** This holds all of the attributes used by abilities, it instantiates a copy of this on every character */
UCLASS()
class SHOOTERGAME_API UShooterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UShooterAttributeSet();
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly, Category = "CharacterLevel", ReplicatedUsing = OnRep_CharacterLevel)
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, CharacterLevel)

	/** Current Health, when 0 we expect owner to die. Capped by MaxHealth */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, Health)

	/** MaxHealth is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, MaxHealth)

	/** RestoreHealth is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_RestoreHealth)
	FGameplayAttributeData RestoreHealth;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, RestoreHealth)

	/** Current Mana, used to execute special abilities. Capped by MaxMana */
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, Mana)

	/** MaxMana is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, MaxMana)

	/** RestoreMana is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_RestoreMana)
	FGameplayAttributeData RestoreMana;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, RestoreMana)

	/** Coins of the Player Pawn , since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Coin", ReplicatedUsing = OnRep_Coins)
	FGameplayAttributeData Coins;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, Coins)

	/** DeathCooldown of the PlayerPawn to Die, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Die", ReplicatedUsing = OnRep_DeathCooldown)
	FGameplayAttributeData DeathCooldown;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, DeathCooldown)

	/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, AttackPower)

	/** Base Damage is divided by DefensePower to get actual damage done, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, DefensePower)

	/** MoveSpeed affects how fast characters can move */
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, MoveSpeed)

	/** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into -Health */
	UPROPERTY(BlueprintReadOnly, Category = "Mana", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, Damage)

	/** when your character die, your character will provide experience for killer */
	UPROPERTY(BlueprintReadOnly, Category = "ProvideEXP", ReplicatedUsing = OnRep_ProvideEXP)
	FGameplayAttributeData ProvideEXP;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, ProvideEXP)

	/** experience for Level up you need  */
	UPROPERTY(BlueprintReadOnly, Category = "MaxEXP", ReplicatedUsing = OnRep_MaxEXP)
	FGameplayAttributeData MaxEXP;
	ATTRIBUTE_ACCESSORS(UShooterAttributeSet, MaxEXP)

protected:
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	// These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	UFUNCTION()
	virtual void OnRep_CharacterLevel();

	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_MaxHealth();

	UFUNCTION()
	virtual void OnRep_RestoreHealth();

	UFUNCTION()
	virtual void OnRep_Mana();

	UFUNCTION()
	virtual void OnRep_MaxMana();

	UFUNCTION()
	virtual void OnRep_RestoreMana();

	UFUNCTION()
	virtual void OnRep_Coins();

	UFUNCTION()
	virtual void OnRep_DeathCooldown();

	UFUNCTION()
	virtual void OnRep_AttackPower();

	UFUNCTION()
	virtual void OnRep_DefensePower();

	UFUNCTION()
	virtual void OnRep_MoveSpeed();

	UFUNCTION()
	virtual void OnRep_ProvideEXP();

	UFUNCTION()
	virtual void OnRep_MaxEXP();
};
