// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterWeaponBase.h"


// Sets default values
AShooterWeaponBase::AShooterWeaponBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CapsuleCollision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleCollision"));
	CapsuleCollision->SetCapsuleHalfHeight(64.f);
	CapsuleCollision->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	CapsuleCollision->SetCollisionObjectType(ECC_Pawn);
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(COLLISION_TC_WEAPON, ECR_Block);
	CapsuleCollision->SetCollisionResponseToChannel(COLLISION_OC_PROJECTILE, ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannel(COLLISION_OC_WEAPON, ECR_Overlap);
	CapsuleCollision->SetCollisionResponseToChannel(COLLISION_OC_MARGIC, ECR_Ignore);
	CapsuleCollision->SetupAttachment(GetRootComponent());

	Mesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetupAttachment(CapsuleCollision);

	LastFireTime = 0.0f;

	AttackDelayCount = 0;
	AttackDelayTime = 0.0f;
	bIsAttacking = false;
	bEnableAttackDelay = false;

	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

// Called when the game starts or when spawned
void AShooterWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCapsuleCollision()->IgnoreActorWhenMoving(this, true);
}

void AShooterWeaponBase::Destroyed()
{
	Super::Destroyed();
}

// Called every frame
void AShooterWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Input

/** [local + server] start weapon fire */
void AShooterWeaponBase::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::StartFire()"));
}

/** [local + server] stop weapon fire */
void AShooterWeaponBase::StopFire()
{

}

/** weapon is now equipped by owner pawn */
void AShooterWeaponBase::OnEquipFinished()
{
	
}

/** weapon is holstered by owner pawn */
void AShooterWeaponBase::OnEquip(AActor* LastWeapon)
{

}

/** weapon is holstered by owner pawn */
void AShooterWeaponBase::OnUnEquip()
{

}

void AShooterWeaponBase::BeginWeaponAttack(FGameplayTag EventTag, float InAttackDelayTime, int32 InAttackDelayCount)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::BeginWeaponAttack()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("BeginWeaponAttack()"));
	AttachEventTag = EventTag;
	AttackDelayCount = InAttackDelayCount;
	AttackDelayTime = InAttackDelayTime;
	bIsAttacking = true;
	GetCapsuleCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AShooterWeaponBase::EndWeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::EndWeaponAttack()"));
	bIsAttacking = false;
	GetCapsuleCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


//////////////////////////////////////////////////////////////////////////
// Control

/** check if weapon can fire */
bool AShooterWeaponBase::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true));
}

/** update weapon state */
void AShooterWeaponBase::SetWeaponState(EWeaponState::Type NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::SetWeaponState()"));
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void AShooterWeaponBase::OnBurstStarted()
{

}

void AShooterWeaponBase::OnBurstFinished()
{

}

/** determine current weapon state */
void AShooterWeaponBase::DetermineWeaponState()
{

}


//////////////////////////////////////////////////////////////////////////
// Reading data

/** get current weapon state */
EWeaponState::Type AShooterWeaponBase::GetCurrentState() const
{
	return CurrentState;
}

EShooterWeaponType AShooterWeaponBase::GetCurrentWeaponType() const
{
	return CommonConfig.CurrentWeaponType;
}

void AShooterWeaponBase::SetIsAttacking(bool arg)
{
	bIsAttacking = arg;
}

bool AShooterWeaponBase::GetIsAttacking() const
{
	return bIsAttacking;
}

void AShooterWeaponBase::SetOwningPawn(AShooterCharacter* NewOwner)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::SetOwningPawn()"));
	if (MyPawn != NewOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyPawn != NewOwner"));
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

class AShooterCharacter* AShooterWeaponBase::GetPawnOwner() const
{
	return MyPawn;
}

float AShooterWeaponBase::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

float AShooterWeaponBase::GetEquipDuration() const
{
	return EquipDuration;
}
//////////////////////////////////////////////////////////////////////////
// Replication & effects

void AShooterWeaponBase::OnRep_MyPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::OnRep_MyPawn()"));
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}

/** Called in network play to do the cosmetic fx for firing */
void AShooterWeaponBase::SimulateWeaponFire()
{

}

/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
void AShooterWeaponBase::StopSimulatingWeaponFire()
{

}

//////////////////////////////////////////////////////////////////////////
// Inventory

/** [server] weapon was added to pawn's inventory */
void AShooterWeaponBase::OnEnterInventory(AShooterCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

/** [server] weapon was removed from pawn's inventory */
void AShooterWeaponBase::OnLeaveInventory()
{
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}

	//if (IsAttachedToPawn())
	//{
	//	OnUnEquip();
	//}
}


/** attaches weapon mesh to pawn's mesh */
void AShooterWeaponBase::AttachMeshToPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawn()"));
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		if (GetCurrentWeaponType() == EShooterWeaponType::Sword)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawn(ShooterWeaponType::Sword)"));
			AttachPoint = MyPawn->GetSwordAttachPoint();
		}
		if (MyPawn->IsLocallyControlled() == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawn(IsLocallyControlled.)"));
			//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			//GetMesh1P()->SetHiddenInGame(true);
			GetMesh3P()->SetHiddenInGame(false);
			//GetMesh1P()->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			GetMesh3P()->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawn( not IsLocallyControlled.)"));
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AShooterWeaponBase::AttachMeshToPawnBackside()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawnBackside()"));
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		FName BacksidePoint = MyPawn->GetBacksideAttachPoint();
		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		if (MyPawn->IsLocallyControlled() == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawnBackside(IsLocallyControlled.)"));
			//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			//GetMesh1P()->SetHiddenInGame(true);
			GetMesh3P()->SetHiddenInGame(false);
			//GetMesh1P()->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
			GetMesh3P()->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase::AttachMeshToPawnBackside( not IsLocallyControlled.)"));
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

/** detaches weapon mesh from pawn */
void AShooterWeaponBase::DetachMeshFromPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::DetachMeshFromPawn()"));
	//Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	//Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
}

/** check if it's currently equipped */
bool AShooterWeaponBase::IsEquipped() const
{
	return bIsEquipped;
}

/** check if mesh is already attached */
bool AShooterWeaponBase::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

USkeletalMeshComponent* AShooterWeaponBase::GetWeaponMesh() const
{
	//return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? GetMesh1P() : GetMesh3P();
	//return (MyPawn != NULL && (MyPawn->GetPawnMode()) == EPawnMode::EFirstP) ? GetMesh1P() : GetMesh3P();
	return GetMesh3P();
}

int32 AShooterWeaponBase::GetCurrentAmmo() const
{
	return 0;
}

int32 AShooterWeaponBase::GetCurrentAmmoInClip() const
{
	return 0;
}

int32 AShooterWeaponBase::GetAmmoPerClip() const
{
	return 0;
}

int32 AShooterWeaponBase::GetMaxAmmo() const
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

void AShooterWeaponBase::HandleFiring()
{

}

/** play weapon sounds */
UAudioComponent* AShooterWeaponBase::PlayWeaponSound(USoundCue* Sound)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::PlayWeaponSound()"));
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

/** play weapon animations */
float AShooterWeaponBase::PlayWeaponAnimation(const FWeaponAnim& Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::PlayWeaponAnimation()"));
	float Duration = 0.0f;
	if (MyPawn)
	{
		//UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		UAnimMontage* UseAnim = Animation.Pawn3P;
		if (UseAnim)
		{
			Duration = MyPawn->PlayAnimMontage(UseAnim);
		}
	}

	return Duration;
}

/** stop playing weapon animations */
void AShooterWeaponBase::StopWeaponAnimation(const FWeaponAnim& Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponBase::StopWeaponAnimation()"));
	if (MyPawn)
	{
		//UAnimMontage* UseAnim = MyPawn->IsFirstPerson() ? Animation.Pawn1P : Animation.Pawn3P;
		UAnimMontage* UseAnim = Animation.Pawn3P;
		if (UseAnim)
		{
			MyPawn->StopAnimMontage(UseAnim);
		}
	}
}

void AShooterWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterWeaponBase, MyPawn);
}


