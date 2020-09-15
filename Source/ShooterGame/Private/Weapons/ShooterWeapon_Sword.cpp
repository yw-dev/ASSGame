// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeapon_Sword.h"
#include "GameplayAbilityTypes.h"
#include "ShooterBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Effects/ShooterImpactEffect.h"


AShooterWeapon_Sword::AShooterWeapon_Sword(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AttackDelayCount = 0;
	AttackDelayTime = 0.f;
	EnableAttackDelay = false;
	IsAttacking = false;

	//GetCapsuleCollision()->OnComponentBeginOverlap.AddDynamic(this, &AShooterWeapon_Sword::OnOverlapBegin);
	//GetCapsuleCollision()->OnComponentEndOverlap.AddDynamic(this, &AShooterWeapon_Sword::OnOverlapEnd);
}

void AShooterWeapon_Sword::BeginPlay()
{
	Super::BeginPlay();
	//GetCapsuleCollision()->IgnoreActorWhenMoving(this, true);
}

//////////////////////////////////////////////////////////////////////////
// Weapon Overlaped

bool AShooterWeapon_Sword::ServerFireBegin_Validate(FGameplayTag EventTag, float DelayTime, int32 DelayCount)
{
	return true;
}

void AShooterWeapon_Sword::ServerFireBegin_Implementation(FGameplayTag EventTag, float DelayTime, int32 DelayCount)
{
	BeginWeaponAttack(EventTag, DelayTime, DelayCount);
}

bool AShooterWeapon_Sword::ServerFireEnd_Validate()
{
	return true;
}

void AShooterWeapon_Sword::ServerFireEnd_Implementation()
{
	EndWeaponAttack();
}

void AShooterWeapon_Sword::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::NotifyActorBeginOverlap()"));
	/*
	if (GetInstigator()->GetClass() != OtherActor->GetClass() && IsAttacking == true)
	{
		//this->GetInstigator();
		FGameplayEventData EventData;
		EventData.Instigator = GetInstigator();
		EventData.Target = OtherActor;
		FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), AttackEventTag, EventData);
	}*/
}

void AShooterWeapon_Sword::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::NotifyActorEndOverlap()"));
	//FGameplayEventData EventData;
	//EventData.Instigator = GetInstigator();
	//EventData.Target = OtherActor;
	//FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), AttackEventTag, EventData);
}

/*
void AShooterWeapon_Sword::BeginWeaponAttack_Implementation(FGameplayTag EventTag, float InAttackDelayTime, int32 InAttackDelayCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::BeginWeaponAttack()"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Weapon_Sword::BeginWeaponAttack()"));
	Super::BeginWeaponAttack(EventTag, InAttackDelayTime, InAttackDelayCount);
}

void AShooterWeapon_Sword::EndWeaponAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::EndWeaponAttack()"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Weapon_Sword::EndWeaponAttack()"));
	Super::EndWeaponAttack();
}
*/
void AShooterWeapon_Sword::OnOverlapBegin(class UPrimitiveComponent* Actor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::OnOverlapBegin()"));
	if (this->GetInstigator()->GetClass() != OtherActor->GetClass() && IsAttacking == true)
	{
		//this->GetInstigator();
		FGameplayEventData EventData;
		EventData.Instigator = GetInstigator();
		EventData.Target = OtherActor;
		FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), EventTag, EventData);
	}
	
}

void AShooterWeapon_Sword::OnOverlapEnd(class UPrimitiveComponent* Actor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::OnOverlapEnd()"));
	FGameplayEventData EventData;
	EventData.Instigator = GetInstigator();
	EventData.Target = OtherActor;
	FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), EventTag, EventData);
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AShooterWeapon_Sword::FireWeapon()
{

}


