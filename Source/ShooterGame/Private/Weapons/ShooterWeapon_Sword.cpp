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

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AShooterWeapon_Sword::WeaponHit(const FHitResult& Impact, const FVector& Origin, const FVector& HitDir, int32 RandomSeed, float ReticleSpread)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::WeaponHit()"));
	
	ProcessHit(Impact, Origin, HitDir, RandomSeed, ReticleSpread);
}

void AShooterWeapon_Sword::WeaponDistanceHit()
{

}

void AShooterWeapon_Sword::OnBurstFinished()
{

}

void AShooterWeapon_Sword::ProcessHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	switch (GetNetMode())
	{
	case NM_Client:
		UE_LOG(LogTemp, Warning, TEXT("NetMode::Client"));
		break;
	case NM_DedicatedServer:
		UE_LOG(LogTemp, Warning, TEXT("NetMode::DedicatedServer"));
		break;
	case NM_Standalone:
		UE_LOG(LogTemp, Warning, TEXT("NetMode::Standalone"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("NetMode::NM_MAX"));
		break;
	}
	switch (Impact.GetActor()->GetRemoteRole())
	{
	case ROLE_Authority:
		UE_LOG(LogTemp, Warning, TEXT("NetRole::Authority"));
		break;
	case ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("NetRole::AutonomousProxy"));
		break;
	case ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Warning, TEXT("NetRole::SimulatedProxy"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("NetRole::ROLE_None"));
		break;
	}
	if (MyPawn && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			ServerNotifyHit(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == NULL)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				ServerNotifyHit(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
			}
			else
			{
				// notify server of the miss
				ServerNotifyMiss(ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}

	// process a confirmed hit
	ProcessHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void AShooterWeapon_Sword::ProcessHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}
	/*
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = Origin;
		HitNotify.RandomSeed = RandomSeed;
		HitNotify.ReticleSpread = ReticleSpread;
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

		SpawnTrailEffect(EndPoint);
		SpawnImpactEffects(Impact);
	}*/
}

bool AShooterWeapon_Sword::ServerNotifyHit_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal HitDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AShooterWeapon_Sword::ServerNotifyHit_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal Origin, FVector_NetQuantizeNormal HitDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	// if we have an instigator, calculate dot between the view and the shot
	if (Instigator && (Impact.GetActor() || Impact.bBlockingHit))
	{
		//const FVector Origin = GetMuzzleLocation();
		//const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();

		// is the angle between the hit and the view within allowed limits (limit + weapon max angle)
		//const float ViewDotHitDir = FVector::DotProduct(Instigator->GetViewRotation().Vector(), ViewDir);
		//if (ViewDotHitDir > InstantConfig.AllowedViewDotHitDir - WeaponAngleDot)
		//{
			if (CurrentState != EWeaponState::Idle)
			{
				if (Impact.GetActor() == NULL)
				{
					if (Impact.bBlockingHit)
					{
						ProcessHit_Confirmed(Impact, Origin, HitDir, RandomSeed, ReticleSpread);
					}
				}
				// assume it told the truth about static things because the don't move and the hit 
				// usually doesn't have significant gameplay implications
				else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessHit_Confirmed(Impact, Origin, HitDir, RandomSeed, ReticleSpread);
				}
				else
				{
					// Get the component bounding box
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					// calculate the box extent, and increase by a leeway
					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= SwordConfig.ClientSideHitLeeway;

					// avoid precision errors with really thin objects
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessHit_Confirmed(Impact, Origin, HitDir, RandomSeed, ReticleSpread);
					}
					else
					{
						UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s (outside bounding box tolerance)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
					}
				}
			}
		/*}
		else if (ViewDotHitDir <= InstantConfig.AllowedViewDotHitDir)
		{
			UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s (facing too far from the hit direction)"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}
		else
		{
			UE_LOG(LogShooterWeapon, Log, TEXT("%s Rejected client side hit of %s"), *GetNameSafe(this), *GetNameSafe(Impact.GetActor()));
		}*/
	}
}

bool AShooterWeapon_Sword::ServerNotifyMiss_Validate(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	return true;
}

void AShooterWeapon_Sword::ServerNotifyMiss_Implementation(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	/*
	const FVector Origin = GetMuzzleLocation();

	// play FX on remote clients
	HitNotify.Origin = Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
		SpawnTrailEffect(EndTrace);
	}*/
}

bool AShooterWeapon_Sword::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->Role == ROLE_Authority ||
			TestActor->GetTearOff())
		{
			return true;
		}
	}

	return false;
}

void AShooterWeapon_Sword::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = SwordConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = SwordConfig.HitDamage;

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, MyPawn->Controller, this);
}

/*
void AShooterWeapon_Sword::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//Super::NotifyActorBeginOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::NotifyActorBeginOverlap()"));
	if (GetInstigator()->GetClass() != OtherActor->GetClass() && IsAttacking == true)
	{
		//this->GetInstigator();
		FGameplayEventData EventData;
		EventData.Instigator = GetInstigator();
		EventData.Target = OtherActor;
		FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), AttackEventTag, EventData);
	}
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
}*/

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
	/*
	if (this->GetInstigator()->GetClass() != OtherActor->GetClass() && IsAttacking == true)
	{
		//this->GetInstigator();
		FGameplayEventData EventData;
		EventData.Instigator = GetInstigator();
		EventData.Target = OtherActor;
		FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), EventTag, EventData);
	}*/
	
}

void AShooterWeapon_Sword::OnOverlapEnd(class UPrimitiveComponent* Actor, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon_Sword::OnOverlapEnd()"));
	/*
	FGameplayEventData EventData;
	EventData.Instigator = GetInstigator();
	EventData.Target = OtherActor;
	FGameplayTag EventTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Event.Montage.Shared.WeaponHit"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), EventTag, EventData);
	*/
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

float AShooterWeapon_Sword::GetCurrentSpread() const
{
	float FinalSpread = SwordConfig.WeaponSpread + CurrentFiringSpread;
	if (MyPawn && MyPawn->IsTargeting())
	{
		FinalSpread *= SwordConfig.TargetingSpreadMod;
	}

	return FinalSpread;
}

