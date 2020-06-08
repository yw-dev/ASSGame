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
	GetCapsuleCollision()->IgnoreActorWhenMoving(this, true);
}

void AShooterWeapon_Sword::BeginWeaponAttack(FGameplayTag EventTag, float DelayTime, int32 DelayCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::BeginWeaponAttack()"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Weapon_Sword::BeginWeaponAttack()"));
	AttackEventTag = EventTag;
	AttackDelayCount = DelayCount;
	AttackDelayTime = DelayTime;
	IsAttacking = true;
	//GetCapsuleCollision()->SetCollisionObjectType(ECC_WorldDynamic);
	GetCapsuleCollision()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleCollision()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetCapsuleCollision()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleCollision()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleCollision()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleCollision()->SetCollisionResponseToChannel(COLLISION_TC_WEAPON, ECR_Block);
	GetCapsuleCollision()->SetCollisionResponseToChannel(COLLISION_OC_PROJECTILE, ECR_Ignore);
	GetCapsuleCollision()->SetCollisionResponseToChannel(COLLISION_OC_WEAPON, ECR_Overlap);
	GetCapsuleCollision()->SetCollisionResponseToChannel(COLLISION_OC_MARGIC, ECR_Ignore);
}

void AShooterWeapon_Sword::EndWeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::EndWeaponAttack()"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Weapon_Sword::EndWeaponAttack()"));
	IsAttacking = false;
	GetCapsuleCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



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


