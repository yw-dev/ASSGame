// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Weapons/ShooterRangedWeapon.h"
#include "Weapons/ShooterMeleeWeapon.h"
#include "Weapons/ShooterDamageType.h"
#include "Items/ShooterWeaponItem.h"
#include "Items/ShooterSkillItem.h"
#include "Items/ShooterPotionItem.h"
#include "Engine/AssetManager.h"
#include "ShooterAssetManager.h"
#include "UI/ShooterHUD.h"
#include "Online/ShooterPlayerState.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundNodeLocalPlayer.h"
#include "AudioThread.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "AbilitySystemGlobals.h"

static int32 NetVisualizeRelevancyTestPoints = 0;
FAutoConsoleVariableRef CVarNetVisualizeRelevancyTestPoints(
	TEXT("p.NetVisualizeRelevancyTestPoints"),
	NetVisualizeRelevancyTestPoints,
	TEXT("")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);


static int32 NetEnablePauseRelevancy = 1;
FAutoConsoleVariableRef CVarNetEnablePauseRelevancy(
	TEXT("p.NetEnablePauseRelevancy"),
	NetEnablePauseRelevancy,
	TEXT("")
	TEXT("0: Disable, 1: Enable"),
	ECVF_Cheat);

FOnShooterCharacterWeaponChange AShooterCharacter::NotifyWeaponChange;
FOnShooterCharacterWeaponTypeChange AShooterCharacter::NotifyWeaponTypeChange;

AShooterCharacter::AShooterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_OC_PROJECTILE, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_TC_WEAPON, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_OC_WEAPON, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_OC_MARGIC, ECR_Block);
	/*
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetHiddenInGame(true);
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	*/
	//GetMesh()->bOnlyOwnerSee = false;
	//GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_TC_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_OC_PROJECTILE, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_OC_WEAPON, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_OC_MARGIC, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create ability system component, and set it to be explicitly replicated
	//AbilitySystemComponent = CreateDefaultSubobject<UShooterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	//AttributeSet = CreateDefaultSubobject<UShooterAttributeSet>(TEXT("AttributeSet"));
	ActivateWeaponSlot = FShooterItemSlot(UShooterAssetManager::SkillItemType, -1);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// some params or flag default value
	CharacterLevel = 1;
	bAbilitiesInitialized = false;

	bEnableComboPeriod = true;

	TargetingSpeedModifier = 0.5f;
	bIsTargeting = false;
	RunningSpeedModifier = 1.5f;
	bWantsToRun = false;
	bWantsToFire = false;
	bIsEquipped = false;
	bPendingReload = false;
	bPendingEquip = false;
	bRefiring = false;
	LowHealthPercentage = 0.5f;
	LastFireTime = 0.0f;
	
	//  init State or Mode Data for Pawn
	CurrentState = EPawnState::Idle;
	CurrentPawnMode = EPawnMode::EThirdP;
	CurrentWeaponType = EShooterWeaponType::None;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	Health = 0.f;
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::PostInitializeComponents( Health = %f  MaxHealth = %f)"), GetHealth(), GetMaxHealth());
		Health = GetHealth();
		//SpawnDefaultInventory();
		//SpawnPropsActors();
	}

	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	// create material instance for setting team colors (3rd person view)
	for (int32 iMat = 0; iMat < GetMesh()->GetNumMaterials(); iMat++)
	{
		MeshMIDs.Add(GetMesh()->CreateAndSetMaterialInstanceDynamic(iMat));
	}

	// play respawn effects
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (RespawnFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, GetActorLocation(), GetActorRotation());
		}

		if (RespawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		}
	}
}

void AShooterCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}

void AShooterCharacter::PawnClientRestart()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::PawnClientRestart()"));
	Super::PawnClientRestart();

	// switch mesh to 1st person view
	UpdatePawnMeshes();

	// reattach weapon if needed
	SetCurrentWeapon(CurrentWeapon);

	// set team colors for 1st person view
	//UMaterialInstanceDynamic* Mesh1PMID = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);
	UMaterialInstanceDynamic* Mesh1PMID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	UpdateTeamColors(Mesh1PMID);
}

void AShooterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local 
	if (PlayerState != NULL)
	{
		UpdateTeamColorsAllMIDs();
	}
}

FRotator AShooterCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool AShooterCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	AShooterPlayerState* TestPlayerState = Cast<AShooterPlayerState>(TestPC->PlayerState);
	AShooterPlayerState* MyPlayerState = Cast<AShooterPlayerState>(PlayerState);

	bool bIsEnemy = true;
	if (GetWorld()->GetGameState())
	{
		const AShooterGameMode* DefGame = GetWorld()->GetGameState()->GetDefaultGameMode<AShooterGameMode>();
		if (DefGame && MyPlayerState && TestPlayerState)
		{
			bIsEnemy = DefGame->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}

	return bIsEnemy;
}

//////////////////////////////////////////////////////////////////////////
// Meshes

void AShooterCharacter::UpdatePawnMeshes()
{
	bool const bFirstPerson = IsFirstPerson();
	/*
	Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);
	Mesh1P->SetHiddenInGame(true);
	*/
	GetMesh()->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	//GetMesh()->SetOwnerNoSee(bFirstPerson);
}

void AShooterCharacter::UpdateTeamColors(UMaterialInstanceDynamic* UseMID)
{
	if (UseMID)
	{
		AShooterPlayerState* MyPlayerState = Cast<AShooterPlayerState>(PlayerState);
		if (MyPlayerState != NULL)
		{
			float MaterialParam = (float)MyPlayerState->GetTeamNum();
			UseMID->SetScalarParameterValue(TEXT("Team Color Index"), MaterialParam);
		}
	}
}

void AShooterCharacter::OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnItemSlotChanged()"));
	Super::OnItemSlotChanged(ItemSlot, Item);
	RefreshSlottedGameplayAbilities();
}

void AShooterCharacter::RefreshSlottedGameplayAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::RefreshSlottedGameplayAbilities()"));
	Super::RefreshSlottedGameplayAbilities();
}

void AShooterCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	/*
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->RelativeRotation, DefMesh1P->RelativeLocation);
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;
	*/
	//Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}


//////////////////////////////////////////////////////////////////////////
// Damage & death


void AShooterCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::FellOutOfWorld()"));
	Die(GetHealth(), FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void AShooterCharacter::Suicide()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::Suicide()"));
	KilledBy(this);
}

void AShooterCharacter::KilledBy(APawn* EventInstigator)
{
	if (HasAuthority() && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(GetHealth(), FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

float AShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::TakeDamage()"));
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->HasGodMode())
	{
		return 0.f;
	}

	if (GetHealth() <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health = GetHealth() - ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}


bool AShooterCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	UE_LOG(LogTemp, Warning, TEXT("Character::CanDie()"));
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode<AShooterGameMode>() == NULL
		|| GetWorld()->GetAuthGameMode<AShooterGameMode>()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}


bool AShooterCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::Die()"));
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, GetHealth());

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AShooterGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AShooterCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}


void AShooterCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnDeath()"));
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	TearOff();
	bIsDying = true;

	if (HasAuthority())
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		AShooterPlayerController* PC = Cast<AShooterPlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UShooterDamageType *DamageType = Cast<UShooterDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback && PC->IsVibrationEnabled())
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, false, "Damage");
			}
		}
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	//if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible())
	if (GetNetMode() != NM_DedicatedServer && DeathSound && GetMesh() && GetMesh()->IsVisible())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all Inventory Actors.
	DestroyInventory();

	// switch back to 3rd person view
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
		LowHealthWarningPlayer->Stop();
	}

	if (RunLoopAC)
	{
		RunLoopAC->Stop();
	}

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		// Trigger ragdoll a little before the animation early so the character doesn't
		// blend back to its normal position.
		const float TriggerRagdollTime = DeathAnimDuration - 0.7f;

		// Enable blend physics so the bones are properly blending against the montage.
		GetMesh()->bBlendPhysics = true;

		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterCharacter::SetRagdollPhysics, FMath::Max(0.1f, TriggerRagdollTime), false);
	}
	else
	{
		SetRagdollPhysics();
	}

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AShooterCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::PlayHit()"));
	if (HasAuthority())
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		AShooterPlayerController* PC = Cast<AShooterPlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UShooterDamageType *DamageType = Cast<UShooterDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback && PC->IsVibrationEnabled())
			{
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, false, "Damage");
			}
		}
	}

	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->NotifyWeaponHit(DamageTaken, DamageEvent, PawnInstigator);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		AShooterPlayerController* InstigatorPC = Cast<AShooterPlayerController>(PawnInstigator->Controller);
		AShooterHUD* InstigatorHUD = InstigatorPC ? Cast<AShooterHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstigatorHUD)
		{
			InstigatorHUD->NotifyEnemyHit();
		}
	}
}


void AShooterCharacter::SetRagdollPhysics()
{
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}


void AShooterCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ReplicateHit()"));
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AShooterCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void AShooterCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

//Pawn::PlayDying sets this lifespan, but when that function is called on client, dead pawn's role is still SimulatedProxy despite bTearOff being true. 
void AShooterCharacter::TornOff()
{
	SetLifeSpan(25.f);
}

bool AShooterCharacter::IsMoving()
{
	return FMath::Abs(GetLastMovementInputVector().Size()) > 0.f;
}

//////////////////////////////////////////////////////////////////////////
// Inventory
/*
void AShooterCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AShooterWeaponBase* NewWeapon = GetWorld()->SpawnActor<AShooterWeaponBase>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}*/

bool AShooterCharacter::UpdateInventoryActors(class UShooterItem* NewItem, bool bAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::UpdateInventoryActors()"));
	const TMap<FShooterItemSlot, AShooterWeaponBase*>& Inventory = GetInventoryList();
	if (!NewItem)
	{
		return false;
	}
	if (bAdd)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor will add"));
		SpawnInventoryActor(NewItem);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor will remove"));
		RemoveInventoryActor(NewItem);
	}
	return true;
}

void AShooterCharacter::SpawnPropsActors()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::SpawnPropsActors()"));
	if (!HasAuthority())
	{
		return;
	}
	//InventoryItems.Reset();
	AShooterPlayerController* PC = Cast<AShooterPlayerController>(Controller);

	const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems = PC->GetSlottedItemMap();
	UE_LOG(LogTemp, Warning, TEXT("Character::SpawnPropsActors( SlottedItems：%d)"), SlottedItems.Num());
	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Value)
		{
			FPrimaryAssetId AssetId = Pair.Value->GetPrimaryAssetId();

			if (AssetId.PrimaryAssetType == UShooterAssetManager::WeaponItemType)
			{
				//Items.Add(Pair.Key);
				UShooterWeaponItem* WeaponItem = Cast<UShooterWeaponItem>(Pair.Value);

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AShooterWeaponBase* NewWeapon = UShooterBlueprintLibrary::SpawnActor<AShooterWeaponBase>(GetWorld(), WeaponItem->WeaponActor, SpawnInfo);
				AddWeapon(Pair.Key, NewWeapon);
				EquipWeapon(NewWeapon);
			}
		}
	}
}

void AShooterCharacter::SpawnInventoryActor(class UShooterItem* NewItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::SpawnInventoryActor()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_Authority"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_AutonomousProxy"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_SimulatedProxy"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_None"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}

	FShooterItemSlot NewSlot;
	UShooterWeaponItem* WeaponItem = Cast<UShooterWeaponItem>(NewItem);
	AShooterPlayerController* PC = Cast<AShooterPlayerController>(Controller);
	PC->GetInventoryItemSlot(WeaponItem, NewSlot);

	if (!NewSlot.IsValid())
	{
		NewSlot = FShooterItemSlot(WeaponItem->ItemType, 0);
	}
	const TMap<FShooterItemSlot, UShooterItem*>& SlottedItem = PC->GetSlottedItemMap();
	int32 num = SlottedItem.Num();

	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : PC->GetSlottedItemMap())
	{
		if (Pair.Value)
		{
			FPrimaryAssetId AssetId = Pair.Value->GetPrimaryAssetId();

			if (Pair.Value->ItemType == UShooterAssetManager::WeaponItemType)
			{
				//Items.Add(Pair.Key);
				UShooterWeaponItem* Weapon = Cast<UShooterWeaponItem>(Pair.Value);

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AShooterWeaponBase* NewWeapon = UShooterBlueprintLibrary::SpawnActor<AShooterWeaponBase>(GetWorld(), Weapon->WeaponActor, SpawnInfo);
				AddWeapon(Pair.Key, NewWeapon);
				EquipWeapon(NewWeapon);
			}
		}
	}/*
	if (!FindWeapon(NewSlot) && IsLocallyControlled())
	{
		// server execu...
		//SpawnInventoryActor(item);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AShooterWeaponBase* NewWeapon = UShooterBlueprintLibrary::SpawnActor<AShooterWeaponBase>(GetWorld(), WeaponItem->WeaponActor, SpawnInfo);

		AddWeapon(NewSlot, NewWeapon);
		EquipWeapon(NewWeapon);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Already has this weapon!"));
	}*/
}

void AShooterCharacter::RemoveInventoryActor(UShooterItem* removeItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::RemoveInventoryActor()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	//UShooterWeaponItem* WeaponItem = Cast<UShooterWeaponItem>(removeItem);
	if (IsLocallyControlled())
	{
		if (!HasAuthority())
		{
			DestroyInventoryItem(removeItem);
			//UClass* WeaponClass = WeaponItem->WeaponActor::StaticClass();
			//AShooterWeaponBase* NewWeapon = WeaponItem->WeaponActor;
			//NewWeapon->K2_DetachFromActor()
			//AddWeapon(NewWeapon);
			//EquipWeapon(NewWeapon);
		}
		else if (HasAuthority() && removeItem)
		{
			//...
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Weapon Actor has Added!")));
		}
	}
}

int32 AShooterCharacter::GetInventoryCount() const
{
	return InventoryItems.Num();
}

AShooterWeaponBase* AShooterCharacter::FindWeapon(FShooterItemSlot NewSlot)
{
	AShooterWeaponBase* const* FoundWeapon = InventoryItems.Find(NewSlot);

	if (FoundWeapon)
	{
		return *FoundWeapon;
	}
	return nullptr;
}

AShooterWeaponBase* AShooterCharacter::FindWeapon(TSubclassOf<AShooterWeaponBase> WeaponCLass)
{
	for (const TPair<FShooterItemSlot, AShooterWeaponBase*>& Pair : GetInventoryList())
	{
		if (Pair.Value && Pair.Value->IsA(WeaponCLass))
		{
			return Cast<AShooterWeaponBase>(Pair.Value);
		}
	}
	return NULL;
}

bool AShooterCharacter::GetInventoryItemSlot(AShooterWeaponBase* Weapon, FShooterItemSlot& ItemSlot) const
{
	//const TMap<UShooterItem*, class AShooterWeaponBase*>& InventoryList = GetInventoryList();
	//const UShooterItem* FoundItem = Inventory.FindKey(Weapon);
	const FShooterItemSlot* FoundSlot = GetInventoryList().FindKey(Weapon);

	if (FoundSlot)
	{
		ItemSlot = *FoundSlot;
		return true;
	}
	return false;
}

void AShooterCharacter::DestroyInventoryItem(UShooterItem* removeItem)
{
	if (InventoryItems.Num() <= 0)
	{
		return;
	}
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);

	FShooterItemSlot ItemSlot;
	MyPC->GetInventoryItemSlot(removeItem, ItemSlot);
	

	if (ItemSlot.IsValid() && GetInventoryList().Contains(ItemSlot))
	{
		// remove all weapons from inventory and destroy them
		AShooterWeaponBase* removeWeapon = FindWeapon(ItemSlot);
		RemoveWeapon(ItemSlot, removeWeapon);
		removeWeapon->Destroy();
	}
}

void AShooterCharacter::DestroyInventory()
{
	if (!HasAuthority())
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (TPair<FShooterItemSlot, AShooterWeaponBase*>& Pair : InventoryItems)
	{
		//FShooterItemSlot ItemSlot = Cast<FShooterItemSlot>(Pair.key);
		AShooterWeaponBase* removeWeapon = Pair.Value;
		//RemoveWeapon(Pair.key, Pair.Value);
	}
	//InventoryItems.Empty();
}

void AShooterCharacter::AddWeapon(FShooterItemSlot ItemSlot, AShooterWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::AddWeapon()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	if (Weapon)
	{
		Weapon->OnEnterInventory(this);
		InventoryItems.Emplace(ItemSlot, Weapon);
		WaponEquips.Add(Weapon);
	}
	else
	{
		ServerAddWeapon(ItemSlot, Weapon);
		UE_LOG(LogTemp, Warning, TEXT("Not Has Authority."));
	}
}

void AShooterCharacter::RemoveWeapon(FShooterItemSlot ItemSlot, AShooterWeaponBase* Weapon)
{
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	if (Weapon && ItemSlot.IsValid())
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->OnLeaveInventory();
		InventoryItems.Remove(ItemSlot);
		WaponEquips.RemoveSingle(Weapon);
		Weapon->Destroy();
	}
	else
	{
		//ServerRemoveWeapon(ItemSlot, Weapon);
	}
}

void AShooterCharacter::EquipWeapon(AShooterWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::EquipWeapon()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	if (Weapon)
	{
		if (HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Character::EquipWeapon((Role == ROLE_Authority) -> SetCurrentWeapon())"));
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("(Role != ROLE_Authority) ->ServerEquipWeapon()"));
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AShooterCharacter::ServerEquipWeapon_Validate(AShooterWeaponBase* Weapon)
{
	return true;
}

void AShooterCharacter::ServerEquipWeapon_Implementation(AShooterWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ServerEquipWeapon()"));
	EquipWeapon(Weapon);
}

bool AShooterCharacter::ServerAddWeapon_Validate(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon)
{
	return true;
}

void AShooterCharacter::ServerAddWeapon_Implementation(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ServerAddWeapon()"));
	AddWeapon(ItemSlot, Weapon);
}

bool AShooterCharacter::ServerRemoveWeapon_Validate(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon)
{
	return true;
}

void AShooterCharacter::ServerRemoveWeapon_Implementation(FShooterItemSlot ItemSlot, class AShooterWeaponBase* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ServerRemoveWeapon()"));
	RemoveWeapon(ItemSlot, Weapon);
}

void AShooterCharacter::OnRep_CurrentWeaponType(EShooterWeaponType weaponType)
{
	SetCurrentWeaponType(weaponType);
}

void AShooterCharacter::OnRep_CurrentWeapon(AShooterWeaponBase* LastWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnRep_CurrentWeapon()"));
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AShooterCharacter::SetCurrentWeaponType(EShooterWeaponType ActiveType)
{
	CurrentWeaponType = ActiveType;
	NotifyWeaponTypeChange.Broadcast(this, ActiveType);
}

void AShooterCharacter::SetCurrentWeapon(AShooterWeaponBase* NewWeapon, AShooterWeaponBase* LastWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::SetCurrentWeapon()"));
	if (!NewWeapon)
	{
		return;
	}
	AShooterWeaponBase* LocalLastWeapon = nullptr;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;
	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);
	}
	GetInventoryItemSlot(CurrentWeapon, ActivateWeaponSlot);
	//ActivateWeaponSlot = FShooterItemSlot(UShooterAssetManager::WeaponItemType, GetInventoryItem(CurrentWeapon));
	//UE_LOG(LogTemp, Warning, TEXT("GetActiveWeaponType() = %u"), (uint32)CurrentWeapon->GetActiveWeaponType());
	//SetActiveType(CurrentWeapon->GetActiveWeaponType());
	NotifyWeaponChange.Broadcast(this, CurrentWeapon, LocalLastWeapon);
	SetCurrentWeaponType(CurrentWeapon->GetCurrentWeaponType());
}

/*
void AShooterCharacter::SetActivePose(enum EShooterWeaponType ActivePose)
{
	//UE_LOG(LogTemp, Warning, TEXT("SetActiveType()"));
	NotifyWeaponTypeChange.Broadcast(this, CurrentWeaponType);
}*/

//////////////////////////////////////////////////////////////////////////
// Ability usage

void AShooterCharacter::StartAbilityFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		FShooterItemSlot CurrentItemSlot = FShooterItemSlot(FPrimaryAssetType(TEXT("Ability.Skill")), 2);
		if (CurrentItemSlot.IsValid())
		{
			ActivateAbilitiesWithItemSlot(CurrentItemSlot, true);
		}
	}
}

void AShooterCharacter::StopAbilityFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CanUseAbility() && IsUsingSkill())
		{
			CurrentWeapon->StopFire();
		}
	}
}

bool AShooterCharacter::IsUsingMelee()
{
	TArray<UShooterGameplayAbility*> ActiveAbilities;

	FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Melee"));
	GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);
	if (ActiveAbilities.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::Using Melee.--------"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::Not Using Melee.--------"));
	}
	return ActiveAbilities.Num() > 0;
}

bool AShooterCharacter::IsUsingRanged()
{
	TArray<UShooterGameplayAbility*> ActiveAbilities;

	FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Ranged"));
	GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);

	return ActiveAbilities.Num() > 0;
}

bool AShooterCharacter::IsUsingSkill()
{
	TArray<UShooterGameplayAbility*> ActiveAbilities;

	FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Skill"));
	GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);

	return ActiveAbilities.Num() > 0;
}

bool AShooterCharacter::CanUseAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::CanUseAbility()"));
	return IsAlive() && (IsUsingSkill() == false);
}

void AShooterCharacter::SetEnableComboPeriod(bool Enable)
{
	bEnableComboPeriod = Enable;
}

bool AShooterCharacter::GetEnableComboPeriod()
{
	return bEnableComboPeriod;
}

void AShooterCharacter::SetJumpSectionNS(UAnimNotifyState_JumpSection* InNotify)
{
	JumpSectionNS = InNotify;
}

UAnimNotifyState_JumpSection* AShooterCharacter::GetJumpSectionNS()
{
	return JumpSectionNS;
}

void AShooterCharacter::AbilityFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CanUseAbility())
		{
			CurrentWeapon->StopFire();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input - server side

EPawnState::Type AShooterCharacter::GetCurrentState() const
{
	return CurrentState;
}

void AShooterCharacter::OnRep_BurstCounter()
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

bool AShooterCharacter::ServerStartFire_Validate()
{
	return true;
}

void AShooterCharacter::ServerStartFire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ServerStartFire()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("ServerStartFire()"));
	StartWeaponFire();
}

bool AShooterCharacter::ServerStopFire_Validate()
{
	return true;
}

void AShooterCharacter::ServerStopFire_Implementation()
{
	//StopWeaponFire();
}

void AShooterCharacter::SetPawnState(EPawnState::Type NewState)
{
	const EPawnState::Type PrevState = CurrentState;

	if (PrevState == EPawnState::Firing && NewState != EPawnState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EPawnState::Firing && NewState == EPawnState::Firing)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("PawnState : Firing"));
		OnBurstStarted();
	}
}

/** determine current weapon state */
void AShooterCharacter::DeterminePawnState()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::DeterminePawnState()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("DeterminePawnState()"));
	EPawnState::Type NewState = EPawnState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			//if (CanReload() == false)
			//{
			//	NewState = CurrentState;
			//}
			//else
			//{
			//	NewState = EWeaponState::Reloading;
			//}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EPawnState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EPawnState::Equipping;
	}

	SetPawnState(NewState);
}

void AShooterCharacter::OnBurstStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnBurstStarted()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("OnBurstStarted()"));
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && CurrentWeapon->CommonConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + CurrentWeapon->CommonConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterCharacter::HandleFiring, LastFireTime + CurrentWeapon->CommonConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}
void AShooterCharacter::OnBurstFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnBurstFinished()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("OnBurstFinished()"));
	// stop firing FX on remote clients
	BurstCounter = 0;
	bWantsToFire = false;
	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}

void AShooterCharacter::HandleFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::HandleFiring()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("HandleFiring()"));
	if (CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (IsLocallyControlled())
		{
			DoMeleeAttack();
			
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (IsLocallyControlled())
	{
		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (IsLocallyControlled())
	{
		// local client will notify server
		if (!HasAuthority())
		{
			ServerHandleFiring();
		}

		// setup refire timer
		bRefiring = (CurrentState == EPawnState::Firing && CurrentWeapon->CommonConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AShooterCharacter::HandleFiring, CurrentWeapon->CommonConfig.TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

bool AShooterCharacter::ServerHandleFiring_Validate()
{
	return true;
}

void AShooterCharacter::ServerHandleFiring_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ServerHandleFiring()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("ServerHandleFiring()"));
	HandleFiring();

	if (CanFire())
	{
		// update firing FX on remote clients
		BurstCounter++;
	}
}

void AShooterCharacter::SimulateWeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::SimulateWeaponFire()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("SimulateWeaponFire()"));
	if (HasAuthority() && CurrentState != EWeaponState::Firing)
	{
		return;
	}
	DoMeleeAttack();
}

void AShooterCharacter::StopSimulatingWeaponFire()
{

}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AShooterCharacter::StartWeaponFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::StartWeaponFire()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("StartWeaponFire()"));

	switch (GetNetMode())
	{
	case ENetMode::NM_DedicatedServer:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  NM_DedicatedServer"));
		break;
	case ENetMode::NM_ListenServer:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  NM_ListenServer"));
		break;
	case ENetMode::NM_Client:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  NM_Client"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  NM_Standalone"));
		break;
	}

	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Role < ROLE_Authority"));
		ServerStartFire();
	}
	if (CurrentWeapon && HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon && HasAuthority()"));
		//Role = ROLE_Authority;
		//bWantsToFire = true;
		//bIsEquipped = true;
		DoMeleeAttack();
		//WeaponAttack();
		//JumpSectionForCombo();
		//DeterminePawnState();
	}
}

void AShooterCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void AShooterCharacter::WeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::WeaponAttack()"));
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("WeaponAttack :"));
	if (CanUseAbility())
	{
		TArray<UShooterGameplayAbility*> ActiveAbilities;

		FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Melee"));
		GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);
		if (ActiveAbilities.Num() > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ActiveAbilities : %d"), ActiveAbilities.Num()));
			ActivateAbilitiesWithItemSlot(ActivateWeaponSlot, true);
			ActivateAbilitiesWithTags(FGameplayTagContainer(AbilityTag), true);
			//return true;
		}
	}
}

void AShooterCharacter::DoMeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::DoMeleeAttack()"));
	Super::DoMeleeAttack();
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("DoMeleeAttack_Implementation :"));
	if (CanUseAbility())
	{
		UE_LOG(LogTemp, Warning, TEXT("Any Abilities Can Use."));
		if (IsUsingMelee() == true) {
			UE_LOG(LogTemp, Warning, TEXT("Character::JumpSectionForCombo--------"));
			//bEnableComboPeriod = true;
			JumpSectionForCombo();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character::ActivateAbilitiesWithItemSlot--------"));
			ActivateAbilitiesWithItemSlot(ActivateWeaponSlot, true);
			/*
			TArray<UShooterGameplayAbility*> ActiveAbilities;
			FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Melee"));
			GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);
			if (ActiveAbilities.Num() > 0)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ActiveAbilities : %d"), ActiveAbilities.Num()));
				return ActivateAbilitiesWithItemSlot(ActivateWeaponSlot, true);
				//return ActivateAbilitiesWithTags(FGameplayTagContainer(AbilityTag), true);
				//return true;
			}*/
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Use any Ability."));
	}
}

void AShooterCharacter::DoSkillAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::DoSkillAttack()"));
	Super::DoSkillAttack();
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("DoMeleeAttack_Implementation :"));
	if (CanUseAbility())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::ActivateAbilitiesWithItemSlot--------"));
		TArray<UShooterGameplayAbility*> ActiveAbilities;
		FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Skill"));
		GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Use any Ability."));
	}
}

bool AShooterCharacter::HasWeapon() const
{
	return CurrentWeapon != nullptr;
}

bool AShooterCharacter::CanFire() const
{
	return IsAlive();
}

bool AShooterCharacter::CanReload() const
{
	return true;
}

void AShooterCharacter::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;

	if (TargetingSound)
	{
		UGameplayStatics::SpawnSoundAttached(TargetingSound, GetRootComponent());
	}

	if (!HasAuthority())
	{
		ServerSetTargeting(bNewTargeting);
	}
}

bool AShooterCharacter::ServerSetTargeting_Validate(bool bNewTargeting)
{
	return true;
}

void AShooterCharacter::ServerSetTargeting_Implementation(bool bNewTargeting)
{
	SetTargeting(bNewTargeting);
}

//////////////////////////////////////////////////////////////////////////
// Movement

void AShooterCharacter::SetRunning(bool bNewRunning, bool bToggle)
{
	bWantsToRun = bNewRunning;
	bWantsToRunToggled = bNewRunning && bToggle;

	if (!HasAuthority())
	{
		ServerSetRunning(bNewRunning, bToggle);
	}
}

bool AShooterCharacter::ServerSetRunning_Validate(bool bNewRunning, bool bToggle)
{
	return true;
}

void AShooterCharacter::ServerSetRunning_Implementation(bool bNewRunning, bool bToggle)
{
	SetRunning(bNewRunning, bToggle);
}

void AShooterCharacter::UpdateRunSounds()
{
	const bool bIsRunSoundPlaying = RunLoopAC != nullptr && RunLoopAC->IsActive();
	const bool bWantsRunSoundPlaying = IsRunning() && IsMoving();

	// Don't bother playing the sounds unless we're running and moving.
	if (!bIsRunSoundPlaying && bWantsRunSoundPlaying)
	{
		if (RunLoopAC != nullptr)
		{
			RunLoopAC->Play();
		}
		else if (RunLoopSound != nullptr)
		{
			RunLoopAC = UGameplayStatics::SpawnSoundAttached(RunLoopSound, GetRootComponent());
			if (RunLoopAC != nullptr)
			{
				RunLoopAC->bAutoDestroy = false;
			}
		}
	}
	else if (bIsRunSoundPlaying && !bWantsRunSoundPlaying)
	{
		RunLoopAC->Stop();
		if (RunStopSound != nullptr)
		{
			UGameplayStatics::SpawnSoundAttached(RunStopSound, GetRootComponent());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Animations

float AShooterCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::PlayAnimMontage( StartSectionName = %s )"), *StartSectionName.ToString());
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}

	return 0.0f;
}

void AShooterCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOut.GetBlendTime(), AnimMontage);
	}
}

void AShooterCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

void AShooterCharacter::JumpSectionForCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::JumpSectionForCombo(%s)"), *FString::Printf(TEXT("bEnableComboPeriod = %s"), bEnableComboPeriod == true?TEXT("true"):TEXT("false")));
	
	if (bEnableComboPeriod == true) {
		//USkeletalMeshComponent* UseMesh = GetPawnMesh();
		USkeletalMeshComponent* UseMesh = GetPawnMesh();

		if (UseMesh && UseMesh->AnimScriptInstance && JumpSectionNS && bEnableComboPeriod)
		{
			int32 RandIndex = FMath::RandRange(0, (JumpSectionNS->GetJumpSetctions().Num()-1));

			UE_LOG(LogTemp, Warning, TEXT("Character::JumpSectionForCombo(%s)"), *FString::Printf(TEXT("JumpSetctionsNum = %d, RandIndex=%d"), JumpSectionNS->GetJumpSetctions().Num(), RandIndex));
		
			//UAnimInstance* AnimInstance = GetPawnMesh()->GetAnimInstance();
			UAnimMontage* AnimMontage = UseMesh->AnimScriptInstance->GetCurrentActiveMontage();
			if (UseMesh->AnimScriptInstance->GetCurrentActiveMontage() != NULL) 
			{
				UE_LOG(LogTemp, Warning, TEXT("JumpSectionForCombo(GetCurrentActiveMontage())"));
			}

			//UseMesh->AnimScriptInstance->Montage_GetCurrentSection(AnimMontage);
			FName CurSection = UseMesh->AnimScriptInstance->Montage_GetCurrentSection(AnimMontage);
			FName NextSection = JumpSectionNS->GetJumpSetctions()[RandIndex];

			UE_LOG(LogTemp, Warning, TEXT("Character::JumpSectionForCombo(CurSection = %s, NextSection = %s)"), *CurSection.ToString(), *NextSection.ToString());

			UseMesh->AnimScriptInstance->Montage_SetNextSection(CurSection, NextSection, UseMesh->AnimScriptInstance->GetCurrentActiveMontage());
			SetEnableComboPeriod(false);
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("can't Set NextSection...."));
		}
		bEnableComboPeriod = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AShooterCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::OnStopFire);

	PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &AShooterCharacter::OnStartTargeting);
	PlayerInputComponent->BindAction("Targeting", IE_Released, this, &AShooterCharacter::OnStopTargeting);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AShooterCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AShooterCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::OnReload);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterCharacter::OnStopJump);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShooterCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("RunToggle", IE_Pressed, this, &AShooterCharacter::OnStartRunningToggle);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShooterCharacter::OnStopRunning);
}


void AShooterCharacter::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		if (CurrentPawnMode == EPawnMode::EFirstP)
		{
			// Limit pitch when walking or falling
			const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
			const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
			AddMovementInput(Direction, Val);
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Val);
		}
	}
}

void AShooterCharacter::MoveRight(float Val)
{
	if ((Controller != NULL) && Val != 0.f)
	{
		if (CurrentPawnMode == EPawnMode::EFirstP)
		{
			const FQuat Rotation = GetActorQuat();
			const FVector Direction = FQuatRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
			AddMovementInput(Direction, Val);
		}
		else
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Val);
		}
	}
}

void AShooterCharacter::MoveUp(float Val)
{
	if (Val != 0.f)
	{
		// Not when walking or falling.
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			return;
		}

		AddMovementInput(FVector::UpVector, Val);
	}
}

void AShooterCharacter::TurnAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Val)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::OnStartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnStartFire()"));
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	//	DoMeleeAttack();
	//}
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsRunning())
		{
			SetRunning(false, false);
		}
		StartWeaponFire();
		//DoMeleeAttack();
	}
}

void AShooterCharacter::OnStopFire()
{
	//StopWeaponFire();
}

void AShooterCharacter::OnStartTargeting()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsRunning())
		{
			SetRunning(false, false);
		}
		SetTargeting(true);
	}
}

void AShooterCharacter::OnStopTargeting()
{
	SetTargeting(false);
}

void AShooterCharacter::OnNextWeapon()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (InventoryItems.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = WaponEquips.IndexOfByKey(CurrentWeapon);
			AShooterWeaponBase* NextWeapon = WaponEquips[(CurrentWeaponIdx + 1) % InventoryItems.Num()];
			GetInventoryItemSlot(NextWeapon, ActivateWeaponSlot);
			SetCurrentWeaponType(NextWeapon->CommonConfig.CurrentWeaponType);
			EquipWeapon(NextWeapon);
		}
	}
}

void AShooterCharacter::OnPrevWeapon()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (InventoryItems.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = WaponEquips.IndexOfByKey(CurrentWeapon);
			AShooterWeaponBase* PrevWeapon = WaponEquips[(CurrentWeaponIdx - 1 + InventoryItems.Num()) % InventoryItems.Num()];
			GetInventoryItemSlot(PrevWeapon, ActivateWeaponSlot);
			//ActivateWeaponSlot = FShooterItemSlot(UShooterAssetManager::WeaponItemType, Inventory.IndexOfByKey(PrevWeapon));
			SetCurrentWeaponType(PrevWeapon->CommonConfig.CurrentWeaponType);
			EquipWeapon(PrevWeapon);
		}
	}
}

void AShooterCharacter::OnReload()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (CurrentWeapon->CommonConfig.CurrentWeaponType != EShooterWeaponType::Gun)
	{
		return;
	}
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (CurrentWeapon)
		{
			((AShooterRangedWeapon*)CurrentWeapon)->StartReload();
		}
	}
}

void AShooterCharacter::OnStartRunning()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsTargeting())
		{
			SetTargeting(false);
		}
		StopWeaponFire();
		SetRunning(true, false);
	}
}

void AShooterCharacter::OnStartRunningToggle()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		if (IsTargeting())
		{
			SetTargeting(false);
		}
		StopWeaponFire();
		SetRunning(true, true);
	}
}

void AShooterCharacter::OnStopRunning()
{
	SetRunning(false, false);
}

bool AShooterCharacter::IsRunning() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return (bWantsToRun || bWantsToRunToggled) && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1;
}

EShooterWeaponType AShooterCharacter::GetCurrentWeaponType()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetCurrentWeaponType();
	}
	return EShooterWeaponType::None;
}

void AShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bWantsToRunToggled && !IsRunning())
	{
		SetRunning(false, false);
	}
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->HasHealthRegen())
	{
		if (this->GetHealth() < this->GetMaxHealth())
		{
			this->Health = this->GetHealth() + 5 * DeltaSeconds;
			if (Health > this->GetMaxHealth())
			{
				Health = this->GetMaxHealth();
			}
		}
	}

	if (GEngine->UseSound())
	{
		if (LowHealthSound)
		{
			if ((this->GetHealth() > 0 && this->GetHealth() < this->GetMaxHealth() * LowHealthPercentage) && (!LowHealthWarningPlayer || !LowHealthWarningPlayer->IsPlaying()))
			{
				LowHealthWarningPlayer = UGameplayStatics::SpawnSoundAttached(LowHealthSound, GetRootComponent(),
					NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true);
				LowHealthWarningPlayer->SetVolumeMultiplier(0.0f);
			}
			else if ((this->GetHealth() > this->GetMaxHealth() * LowHealthPercentage || this->GetHealth() < 0) && LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
			{
				LowHealthWarningPlayer->Stop();
			}
			if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
			{
				const float MinVolume = 0.3f;
				const float VolumeMultiplier = (1.0f - (this->GetHealth() / (this->GetMaxHealth() * LowHealthPercentage)));
				LowHealthWarningPlayer->SetVolumeMultiplier(MinVolume + (1.0f - MinVolume) * VolumeMultiplier);
			}
		}

		UpdateRunSounds();
	}

	const APlayerController* PC = Cast<APlayerController>(GetController());
	const bool bLocallyControlled = (PC ? PC->IsLocalController() : false);
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
	{
	    USoundNodeLocalPlayer::GetLocallyControlledActorCache().Add(UniqueID, bLocallyControlled);
	});
	
	TArray<FVector> PointsToTest;
	BuildPauseReplicationCheckPoints(PointsToTest);

	if (NetVisualizeRelevancyTestPoints == 1)
	{
		for (FVector PointToTest : PointsToTest)
		{
			DrawDebugSphere(GetWorld(), PointToTest, 10.0f, 8, FColor::Red);
		}
	}
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
		{
			USoundNodeLocalPlayer::GetLocallyControlledActorCache().Remove(UniqueID);
		});
	}
}

void AShooterCharacter::OnStartJump()
{
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		bPressedJump = true;
	}
}

void AShooterCharacter::OnStopJump()
{
	bPressedJump = false;
	StopJumping();
}

////////////////////////////////////////////////////////////////////////// 
// Replication

void AShooterCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AShooterCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AShooterCharacter, InventoryItems, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bWantsToRun, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AShooterCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AShooterCharacter, CurrentWeapon);
	DOREPLIFETIME(AShooterCharacter, CurrentWeaponType);
	DOREPLIFETIME(AShooterCharacter, Health);
}

bool AShooterCharacter::IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer)
{
	if (NetEnablePauseRelevancy == 1)
	{
		APlayerController* PC = Cast<APlayerController>(ConnectionOwnerNetViewer.InViewer);
		check(PC);

		FVector ViewLocation;
		FRotator ViewRotation;
		PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(LineOfSight), true, PC->GetPawn());
		CollisionParams.AddIgnoredActor(this);

		TArray<FVector> PointsToTest;
		BuildPauseReplicationCheckPoints(PointsToTest);

		for (FVector PointToTest : PointsToTest)
		{
			if (!GetWorld()->LineTraceTestByChannel(PointToTest, ViewLocation, ECC_Visibility, CollisionParams))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void AShooterCharacter::OnReplicationPausedChanged(bool bIsReplicationPaused)
{
	GetMesh()->SetHiddenInGame(bIsReplicationPaused, true);
}

AShooterWeaponBase* AShooterCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

USkeletalMeshComponent* AShooterCharacter::GetPawnMesh() const
{
	return GetMesh();
	//return IsFirstPerson() ? Mesh1P : GetMesh();
}

EPawnMode AShooterCharacter::GetPawnMode()
{
	return CurrentPawnMode;
}

USkeletalMeshComponent* AShooterCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	//return WantFirstPerson == true ? Mesh1P : GetMesh();
	return GetMesh();
}

FName AShooterCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

FName AShooterCharacter::GetSwordAttachPoint() const
{
	return SwordAttachPoint;
}

FName AShooterCharacter::GetBacksideAttachPoint() const
{
	return BacksideAttachPoint;
}

float AShooterCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

bool AShooterCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float AShooterCharacter::GetRunningSpeedModifier() const
{
	return RunningSpeedModifier;
}

bool AShooterCharacter::IsFiring() const
{
	return bWantsToFire;
};

bool AShooterCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}
/*
int32 AShooterCharacter::GetMaxHealth() const
{
	return GetCurMaxHealth();
}*/
/*
bool AShooterCharacter::IsAlive() const
{
	return GetHealth() > 0;
}*/

float AShooterCharacter::GetLowHealthPercentage() const
{
	return LowHealthPercentage;
}

void AShooterCharacter::UpdateTeamColorsAllMIDs()
{
	for (int32 i = 0; i < MeshMIDs.Num(); ++i)
	{
		UpdateTeamColors(MeshMIDs[i]);
	}
}

void AShooterCharacter::BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints)
{
	FBoxSphereBounds Bounds = GetCapsuleComponent()->CalcBounds(GetCapsuleComponent()->GetComponentTransform());
	FBox BoundingBox = Bounds.GetBox();
	float XDiff = Bounds.BoxExtent.X * 2;
	float YDiff = Bounds.BoxExtent.Y * 2;

	RelevancyCheckPoints.Add(BoundingBox.Min);
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Min.X + XDiff, BoundingBox.Min.Y + YDiff, BoundingBox.Min.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(FVector(BoundingBox.Max.X - XDiff, BoundingBox.Max.Y - YDiff, BoundingBox.Max.Z));
	RelevancyCheckPoints.Add(BoundingBox.Max);
}


UAbilitySystemComponent* AShooterCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
/*
void AShooterCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("AddStartupGameplayAbilities()"));
	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UShooterGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		AddSlottedGameplayAbilities();

		bAbilitiesInitialized = true;
	}
}

void AShooterCharacter::RemoveStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && bAbilitiesInitialized)
	{
		// Remove any abilities added from a previous call
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && GameplayAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		// Do in two passes so the removal happens after we have the full list
		for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
		{
			AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		}

		// Remove all of the passive gameplay effects that were applied by this character
		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);

		RemoveSlottedGameplayAbilities(true);

		bAbilitiesInitialized = false;
	}
}*/
/*
void AShooterCharacter::OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("OnItemSlotChanged()"));
	RefreshSlottedGameplayAbilities();
}

void AShooterCharacter::RefreshSlottedGameplayAbilities()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("RefreshSlottedGameplayAbilities()"));
	if (bAbilitiesInitialized)
	{
		// Refresh any invalid abilities and adds new ones
		RemoveSlottedGameplayAbilities(false);
		AddSlottedGameplayAbilities();
	}
}

void AShooterCharacter::FillSlottedAbilitySpecs(TMap<FShooterItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{
	// First add default ones
	for (const TPair<FShooterItemSlot, TSubclassOf<UShooterGameplayAbility>>& DefaultPair : DefaultSlottedAbilities)
	{
		if (DefaultPair.Value.Get())
		{
			SlottedAbilitySpecs.Add(DefaultPair.Key, FGameplayAbilitySpec(DefaultPair.Value, GetCharacterLevel(), INDEX_NONE, this));
		}
	}

	// Now potentially override with inventory
	if (InventorySource)
	{
		const TMap<FShooterItemSlot, UShooterItem*>& SlottedItemMap = InventorySource->GetSlottedItemMap();

		for (const TPair<FShooterItemSlot, UShooterItem*>& ItemPair : SlottedItemMap)
		{
			UShooterItem* SlottedItem = ItemPair.Value;

			if (SlottedItem && SlottedItem->GrantedAbility)
			{
				// This will override anything from default
				SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, GetCharacterLevel(), INDEX_NONE, SlottedItem));
			}
		}
	}
}

void AShooterCharacter::AddSlottedGameplayAbilities()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("AddSlottedGameplayAbilities()"));
	TMap<FShooterItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	FillSlottedAbilitySpecs(SlottedAbilitySpecs);

	// Now add abilities if needed
	for (const TPair<FShooterItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpecs)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid())
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("GiveAbility()"));
		}
	}
}

void AShooterCharacter::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	TMap<FShooterItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;

	if (!bRemoveAll)
	{
		// Fill in map so we can compare
		FillSlottedAbilitySpecs(SlottedAbilitySpecs);
	}

	for (TPair<FShooterItemSlot, FGameplayAbilitySpecHandle>& ExistingPair : SlottedAbilities)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(ExistingPair.Value);
		bool bShouldRemove = bRemoveAll || !FoundSpec;

		if (!bShouldRemove)
		{
			// Need to check desired ability specs, if we got here FoundSpec is valid
			FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(ExistingPair.Key);

			if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject)
			{
				bShouldRemove = true;
			}
		}

		if (bShouldRemove)
		{
			if (FoundSpec)
			{
				// Need to remove registered ability
				AbilitySystemComponent->ClearAbility(ExistingPair.Value);
			}

			// Make sure handle is cleared even if ability wasn't found
			ExistingPair.Value = FGameplayAbilitySpecHandle();
		}
	}
}
*/
void AShooterCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::PossessedBy()"));
	Super::PossessedBy(NewController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	UpdateTeamColorsAllMIDs();

	InventorySource = NewController;
	//FOnSlottedItemChangedNative OnSlottedItemChangedNative = InventorySource->GetSlottedItemChangedDelegate();

	//InventoryUpdateHandle = OnSlottedItemChangedNative.AddUObject(this, &AShooterCharacter::OnItemSlotChanged);
	//InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &AShooterCharacter::RefreshSlottedGameplayAbilities);

	AShooterPlayerController* PC = Cast<AShooterPlayerController>(NewController);
	const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems = PC->GetSlottedItemMap();
	UE_LOG(LogTemp, Warning, TEXT("Character::SpawnPropsActors(  PC->SlottedItems：%d)"), SlottedItems.Num());
	// 道具
	SpawnPropsActors();
	//InventoryItems.Reset();
	/*
	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : PC->GetSlottedItemMap())
	{
		if (Pair.Value)
		{
			FPrimaryAssetId AssetId = Pair.Value->GetPrimaryAssetId();

			if (AssetId.PrimaryAssetType == UShooterAssetManager::WeaponItemType)
			{
				//Items.Add(Pair.Key);
				UShooterWeaponItem* WeaponItem = Cast<UShooterWeaponItem>(Pair.Value);

				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AShooterWeaponBase* NewWeapon = UShooterBlueprintLibrary::SpawnActor<AShooterWeaponBase>(GetWorld(), WeaponItem->WeaponActor, SpawnInfo);
				AddWeapon(Pair.Key, NewWeapon);
				EquipWeapon(NewWeapon);
			}
		}
	}*/
	/*
	// Initialize our abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		//this->UpdatePawnMeshes
		//AbilitySystemComponent->SetAvatarActor(this);
		AbilitySystemComponent->RefreshAbilityActorInfo();
		AddStartupGameplayAbilities();
	}*/
}

void AShooterCharacter::UnPossessed()
{
	Super::UnPossessed();
	/*
	// Unmap from inventory source
	if (InventorySource && InventoryUpdateHandle.IsValid())
	{
		InventorySource->GetSlottedItemChangedDelegate().Remove(InventoryUpdateHandle);
		InventoryUpdateHandle.Reset();

		InventorySource->GetInventoryLoadedDelegate().Remove(InventoryLoadedHandle);
		InventoryLoadedHandle.Reset();
	}

	InventorySource = nullptr;*/
}

void AShooterCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}
/*
float AShooterCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}*/

float AShooterCharacter::GetCurMaxHealth() const
{
	return GetMaxHealth();
}
/*
float AShooterCharacter::GetMana() const
{
	return AttributeSet->GetMana();
}*/
/*
float AShooterCharacter::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}*/
/*
float AShooterCharacter::GetMoveSpeed() const
{
	return AttributeSet->GetMoveSpeed();
}*/
/*
int32 AShooterCharacter::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool AShooterCharacter::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		// Our level changed so we need to refresh abilities
		RemoveStartupGameplayAbilities();
		CharacterLevel = NewLevel;
		AddStartupGameplayAbilities();
		return true;
	}
	return false;
}*/
/*
bool AShooterCharacter::ActivateAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ActivateAbilitiesWithItemSlot(SlottedAbilities.Num = %d)"), SlottedAbilities.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("ActivateAbilitiesWithItemSlot(SlottedAbilities.Num = %d)"), SlottedAbilities.Num());
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}
	return false;
}

void AShooterCharacter::GetActiveAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, TArray<UShooterGameplayAbility*>& ActiveAbilities)
{
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);

		if (FoundSpec)
		{
			TArray<UGameplayAbility*> AbilityInstances = FoundSpec->GetAbilityInstances();

			// Find all ability instances executed from this slot
			for (UGameplayAbility* ActiveAbility : AbilityInstances)
			{
				ActiveAbilities.Add(Cast<UShooterGameplayAbility>(ActiveAbility));
			}
		}
	}
}

bool AShooterCharacter::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::ActivateAbilitiesWithTags()"));
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("ActivateAbilitiesWithTags()"));
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void AShooterCharacter::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UShooterGameplayAbility*>& ActiveAbilities)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("GetActiveAbilitiesWithTags()"));
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool AShooterCharacter::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::GetCooldownRemainingForTag()"));
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}
			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}*/
/*
void AShooterCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::HandleDamage()"));
	//TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	//FDamageEvent DamageEvent(ValidDamageTypeClass);
	//OnDamaged(DamageAmount, HitInfo, DamageTags, EventInstigator, DamageCauser);
	//UGameplayStatics::ApplyDamage(DamageCauser, DamageAmount, EventInstigator, TSubclassOf<UDamageType>(UDamageType::StaticClass()));
	AShooterCharacter* hitPawn = Cast<AShooterCharacter>(HitInfo.Actor);
	if (nullptr != hitPawn) 
	{
		FPointDamageEvent damageEvent;
		damageEvent.HitInfo = HitInfo;
	}
	//TakeDamage(DamageAmount, UDamageType::StaticClass());
}*/
/*
float AShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::TakeDamage()"));
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->HasGodMode())
	{
		return 0.f;
	}

	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}
*/
/*
void AShooterCharacter::OnDamaged_Implementation(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnDamaged_Implementation()"));
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);

	if (CanUseAbility())
	{
		// int32 StartSection = FMath::RandRange(0, 2);
		// Death anim
		// float DeathAnimDuration = PlayAnimMontage(HitAnim);

		PlayHit(DamageAmount, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		
		AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(EventInstigator);
		if (MyPC && MyPC->HasGodMode())
		{
			return;
		}

		if (Health <= 0.f)
		{
			return;
		}

		// make sure we have a good damage type
		//TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		//FDamageEvent DamageEvent(ValidDamageTypeClass);

		// Modify based on game rules.
		AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
		DamageAmount = Game ? Game->ModifyDamage(DamageAmount, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

		const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		if (DamageAmount > 0.f)
		{
			Health -= DamageAmount;
			if (Health <= 0)
			{
				Die(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}
			else
			{
				PlayHit(DamageAmount, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
			}

			MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
		}
	}
	else {
		Die(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
}*/
/*
void AShooterCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::HandleHealthChanged()"));
	// We only call the BP callback if this is not the initial ability setup 
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}*/
/*
void AShooterCharacter::OnHealthChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnHealthChanged_Implementation()"));

	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);

	AShooterHUD* ShooterHUD = MyPC->GetShooterHUD();

}
*/
/*
void AShooterCharacter::HandleManaChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::HandleManaChanged()"));
	if (bAbilitiesInitialized)
	{
		OnManaChanged(DeltaValue, EventTags);
	}
}*/
/*
void AShooterCharacter::OnManaChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnManaChanged_Implementation()"));

}*/
/*
void AShooterCharacter::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	// Update the character movement's walk speed 
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();

	if (bAbilitiesInitialized)
	{
		OnMoveSpeedChanged(DeltaValue, EventTags);
	}
}
/*
void AShooterCharacter::OnMoveSpeedChanged_Implementation(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{

}*/