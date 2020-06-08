// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/ShooterCharacter.h"
#include "Abilities/ShooterAbilityTypes.h"
#include "Abilities/ShooterAbilitySystemComponent.h"


bool FShooterGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetGameplayEffectSpecs.Num() > 0;
}

bool FShooterGameplayEffectContainerSpec::HasValidTargets() const
{
	return TargetData.Num() > 0;
}

void FShooterGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

void FShooterAbilityActorInfo::InitFromActor(AActor *OwnerActor, AActor *AvatarActor, UAbilitySystemComponent* InAbilitySystemComponent)
{
	/*
	check(InOwnerActor);
	check(InAbilitySystemComponent);

	OwnerActor = InOwnerActor;
	AvatarActor = InAvatarActor;
	AbilitySystemComponent = InAbilitySystemComponent;
	//SkeletalMeshComponents.Reset();

	APlayerController* OldPC = PlayerController.Get();

	// Look for a player controller or pawn in the owner chain.
	AActor *TestActor = InOwnerActor;
	while (TestActor)
	{
		if (APlayerController * CastPC = Cast<APlayerController>(TestActor))
		{
			PlayerController = CastPC;
			break;
		}

		if (APawn * Pawn = Cast<APawn>(TestActor))
		{
			PlayerController = Cast<APlayerController>(Pawn->GetController());
			break;
		}

		TestActor = TestActor->GetOwner();
	}

	// Notify ASC if PlayerController was found for first time
	if (OldPC == nullptr && PlayerController.IsValid())
	{
		InAbilitySystemComponent->OnPlayerControllerSet();
	}

	if (AActor* const AvatarActorPtr = AvatarActor.Get())
	{
		//PlayerController->GetPawn()
		AShooterCharacter* Pawn = Cast<AShooterCharacter>(AvatarActorPtr);
		// Grab Components that we care about
		TArray<USkeletalMeshComponent*> ComponentsByClass = AvatarActorPtr->GetComponentsByClass(USkeletalMeshComponent::StaticClass());
		if (Pawn->IsFirstPerson())
		{
			SkeletalMeshComponent = ComponentsByClass[0];
		}
		else
		{
			SkeletalMeshComponent = ComponentsByClass[1];
		}

		//SkeletalMeshComponents.Reserve(ComponentsByClass.Num());
		//for (int i = 0; i < ComponentsByClass.Num(); ++i)
		//{
		//	TWeakObjectPtr<USkeletalMeshComponent> Component = ComponentsByClass[i];
		//	SkeletalMeshComponents.Push(Component);
		//}
		//SkeletalMeshComponents = AvatarActorPtr->GetComponentsByClass<USkeletalMeshComponent>();
		MovementComponent = AvatarActorPtr->FindComponentByClass<UMovementComponent>();
	}
	else
	{
		MovementComponent = nullptr;
	}
	*/
}
