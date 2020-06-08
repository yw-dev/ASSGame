// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMeleeWeapon.h"


AShooterMeleeWeapon::AShooterMeleeWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AShooterMeleeWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AShooterMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShooterMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterMeleeWeapon::Destroyed()
{
	Super::Destroyed();
}


//////////////////////////////////////////////////////////////////////////
// Inventory

/** weapon is being equipped by owner pawn */
void AShooterMeleeWeapon::OnEquip(AActor* LastWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::OnEquip()"));
	Super::OnEquip(LastWeapon);
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	// Only play animation if last weapon is valid
	if (LastWeapon && MyPawn)
	{
		FName AttachPoint = MyPawn->GetBacksideAttachPoint();
		UE_LOG(LogTemp, Warning, TEXT("AttachPoint = %s"), *AttachPoint.ToString());
		AttachActorToPawn(LastWeapon, AttachPoint);
		float Duration = PlayWeaponAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			// fail safe
			Duration = 0.5f;
		}
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AShooterMeleeWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}
}

/** weapon is now equipped by owner pawn */
void AShooterMeleeWeapon::OnEquipFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::OnEquipFinished()"));
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponState();
}

/** weapon is holstered by owner pawn */
void AShooterMeleeWeapon::OnUnEquip()
{

}

/** [server] weapon was removed from pawn's inventory */
void AShooterMeleeWeapon::OnLeaveInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::OnLeaveInventory()"));
	if (HasAuthority())
	{
		SetOwningPawn(NULL);
	}

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}
}

void AShooterMeleeWeapon::AttachActorToPawn(AActor* Weapon, FName AttachPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::AttachActorToPawn()"));
	// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
	AShooterMeleeWeapon* MW = Cast<AShooterMeleeWeapon>(Weapon);
	if (MyPawn && MyPawn->IsLocallyControlled() == true)
	{
		//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
		USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(true);
		//GetMesh1P()->SetHiddenInGame(false);
		MW->GetWeaponMesh()->SetHiddenInGame(false);
		//MW->GetMesh1P()->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		MW->GetWeaponMesh()->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	}
	else
	{
		USkeletalMeshComponent* UseWeaponMesh = MW->GetWeaponMesh();
		USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
		UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		UseWeaponMesh->SetHiddenInGame(false);
	}
}

/** attaches weapon mesh to pawn's mesh */
void AShooterMeleeWeapon::AttachMeshToPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::AttachMeshToPawn()"));
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetSwordAttachPoint();
		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		if (MyPawn->IsLocallyControlled() == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::AttachMeshToPawn( IsLocallyControlled. )"));
				//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
				//GetMesh1P()->SetHiddenInGame(true);
				//GetMesh1P()->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
				USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(true);
				GetMesh3P()->SetHiddenInGame(false);
				GetMesh3P()->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::AttachMeshToPawn( Not IsLocallyControlled. )"));
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AShooterMeleeWeapon::AttachMeshToPawnBackside()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::AttachMeshToPawnBackside()"));
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		FName BacksidePoint = MyPawn->GetBacksideAttachPoint();
		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		if (MyPawn->IsLocallyControlled() == true)
		{
			//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			//GetMesh1P()->SetHiddenInGame(true);
			GetMesh3P()->SetHiddenInGame(false);
			//GetMesh1P()->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
			GetMesh3P()->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, BacksidePoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

/** detaches weapon mesh from pawn */
void AShooterMeleeWeapon::DetachMeshFromPawn()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::DetachMeshFromPawn()"));
	//GetMesh1P()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	//GetMesh1P()->SetHiddenInGame(true);
	GetMesh3P()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	GetMesh3P()->SetHiddenInGame(false);
}

//////////////////////////////////////////////////////////////////////////
// Input

/** [local + server] start weapon fire */
void AShooterMeleeWeapon::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::StartFire()"));
	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

/** [local + server] stop weapon fire */
void AShooterMeleeWeapon::StopFire()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::StopFire()"));
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

//////////////////////////////////////////////////////////////////////////
// Reading data


//////////////////////////////////////////////////////////////////////////
// Input - server side

bool AShooterMeleeWeapon::ServerStartFire_Validate()
{
	return true;
}

void AShooterMeleeWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AShooterMeleeWeapon::ServerStopFire_Validate()
{
	return true;
}

void AShooterMeleeWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

UFUNCTION()
void AShooterMeleeWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

/** Called in network play to do the cosmetic fx for firing */
void AShooterMeleeWeapon::SimulateWeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::SimulateWeaponFire()"));
	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	/*
	USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
	// Split screen requires we create 2 effects. One that we see and one that the other player sees.
	if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
	{
		AController* PlayerCon = MyPawn->GetController();
		if (PlayerCon != NULL)
		{
			Mesh1P->GetSocketLocation(MuzzleAttachPoint);
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
			MuzzlePSC->bOwnerNoSee = false;
			MuzzlePSC->bOnlyOwnerSee = true;

			Mesh3P->GetSocketLocation(MuzzleAttachPoint);
			MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
			MuzzlePSCSecondary->bOwnerNoSee = true;
			MuzzlePSCSecondary->bOnlyOwnerSee = false;
		}
	}
	*/

	if (!bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	PlayWeaponSound(FireSound);

	AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
		if (FireCameraShake != NULL)
		{
			PC->ClientPlayCameraShake(FireCameraShake, 1);
		}
		if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
		{
			PC->ClientPlayForceFeedback(FireForceFeedback, false, false, "Weapon");
		}
	}
}

/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
void AShooterMeleeWeapon::StopSimulatingWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;

		PlayWeaponSound(FireFinishSound);
	}
}


//////////////////////////////////////////////////////////////////////////
// Weapon usage

/** [local + server] handle weapon fire */
void AShooterMeleeWeapon::HandleFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::HandleFiring()"));
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}
		
		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && MeleeConfig.AttackDelayTime > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterMeleeWeapon::HandleFiring, MeleeConfig.AttackDelayTime, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool AShooterMeleeWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AShooterMeleeWeapon::ServerHandleFiring_Implementation()
{
	// update firing FX on remote clients
	BurstCounter++;
}

/** [local + server] firing started */
void AShooterMeleeWeapon::OnBurstStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::OnBurstStarted()"));
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && MeleeConfig.AttackDelayTime > 0.0f &&
		LastFireTime + MeleeConfig.AttackDelayTime > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterMeleeWeapon::HandleFiring, LastFireTime + MeleeConfig.AttackDelayTime - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

/** [local + server] firing finished */
void AShooterMeleeWeapon::OnBurstFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::OnBurstFinished()"));
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}

/** update weapon state */
void AShooterMeleeWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::SetWeaponState()"));
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

/** determine current weapon state */
void AShooterMeleeWeapon::DetermineWeaponState()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeWeapon::DetermineWeaponState()"));
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if ((bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void AShooterMeleeWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AShooterMeleeWeapon, BurstCounter, COND_SkipOwner);
}
