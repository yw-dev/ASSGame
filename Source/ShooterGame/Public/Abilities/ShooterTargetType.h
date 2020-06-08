// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayAbilityTypes.h"
#include "Abilities/ShooterAbilityTypes.h"
#include "UObject/NoExportTypes.h"
#include "ShooterTargetType.generated.h"

class AShooterCharacter;
class AActor;
struct FGameplayEventData;

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class SHOOTERGAME_API UShooterTargetType : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UShooterTargetType() {}	

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class SHOOTERGAME_API UShooterTargetType_UseOwner : public UShooterTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UShooterTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class SHOOTERGAME_API UShooterTargetType_UseEventData : public UShooterTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UShooterTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AShooterCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
