// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterCharacterBase.h"
#include "ShooterBlueprintLibrary.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Items/ShooterItem.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/ShooterGameplayAbility.h" 


// Sets default values
AShooterCharacterBase::AShooterCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase()"));
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UShooterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSetBase = CreateDefaultSubobject<UShooterAttributeSet>(TEXT("AttributeSetBase"));

	bAbilitiesInitialized = false;
	IsProtectedByShield = false;
	bInvincible = false;
}

void AShooterCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AShooterCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AShooterCharacterBase::BeginDestroy()
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

void AShooterCharacterBase::Destroyed()
{
	Super::Destroyed();
}

UAbilitySystemComponent* AShooterCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AShooterCharacterBase::InitializeDefaultAttributes()
{
	UE_LOG(LogTemp, Warning, TEXT("Character::InitializeDefaultAttributes()"));
	check(AbilitySystemComponent);

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FGameplayEffectSpec effect = *NewHandle.Data.Get();
		UE_LOG(LogTemp, Warning, TEXT("AddStartupGameplayAbilities( Level = %d )"), effect.GetLevel());
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

void AShooterCharacterBase::AddStartupGameplayAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::AddStartupGameplayAbilities( DefaultEffects.num = %d)"), DefaultEffects.Num());

	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UShooterGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : DefaultEffects)
		{
			//UGameplayEffect* GameplayEffectObj = GameplayEffect->GetDefaultObject<UGameplayEffect>();
			//if (GameplayEffectObj)
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("AddStartupGameplayAbilities( Level = %d )"), GetCharacterLevel());
			//}
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				FGameplayEffectSpec effect = *NewHandle.Data.Get();
				UE_LOG(LogTemp, Warning, TEXT("AddStartupGameplayAbilities( Level = %d )"), effect.GetLevel());
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		AddSlottedGameplayAbilities();

		bAbilitiesInitialized = true;
	}
}

void AShooterCharacterBase::RemoveStartupGameplayAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::RemoveStartupGameplayAbilities()"));
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
}

void AShooterCharacterBase::OnInventoryItemChanged(UShooterItem* item, bool bAdded)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnInventoryItemChanged()"));
	/*
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
	}*/
	RefreshSlottedGameplayAbilities();
}

void AShooterCharacterBase::OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnItemSlotChanged()"));
	RefreshSlottedGameplayAbilities();
}

void AShooterCharacterBase::UpdateInventoryItems(FShooterItemSlot NewSlot, UShooterItem* NewItem)
{
	AShooterPlayerState* MyPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (MyPlayerState != NULL)
	{
		MyPlayerState->AddSlottedItems(NewSlot, NewItem);
	}
}

void AShooterCharacterBase::RefreshSlottedGameplayAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::RefreshSlottedGameplayAbilities()"));
	if (bAbilitiesInitialized)
	{
		// Refresh any invalid abilities and adds new ones
		RemoveSlottedGameplayAbilities(false);
		AddSlottedGameplayAbilities();
	}
}

void AShooterCharacterBase::FillSlottedAbilitySpecs(TMap<FShooterItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::FillSlottedAbilitySpecs()"));
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
				//UpdateInventoryItems(ItemPair.Key, ItemPair.Value);
				// This will override anything from default
				SlottedAbilitySpecs.Add(ItemPair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, GetCharacterLevel(), INDEX_NONE, SlottedItem));
			}
		}
	}
}

void AShooterCharacterBase::AddSlottedGameplayAbilities()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::AddSlottedGameplayAbilities()"));
	TMap<FShooterItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	FillSlottedAbilitySpecs(SlottedAbilitySpecs);

	// Now add abilities if needed
	for (const TPair<FShooterItemSlot, FGameplayAbilitySpec>& SpecPair : SlottedAbilitySpecs)
	{
		FGameplayAbilitySpecHandle& SpecHandle = SlottedAbilities.FindOrAdd(SpecPair.Key);

		if (!SpecHandle.IsValid())
		{
			SpecHandle = AbilitySystemComponent->GiveAbility(SpecPair.Value);
		}
	}
}

void AShooterCharacterBase::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::RemoveSlottedGameplayAbilities()"));
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

void AShooterCharacterBase::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::PossessedBy()"));
	Super::PossessedBy(NewController);

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
	//AShooterPlayerController* PC = Cast<AShooterPlayerController>(NewController);
	// Try setting the inventory source, this will fail for AI
	InventorySource = NewController;
	//AShooterPlayerController* PC = Cast<AShooterPlayerController>(NewController);
	//InventoryUpdateHandle.BindUFunction(this, STATIC_FUNCTION_FNAME(TEXT("AShooterCharacterBase::OnInventoryItemChanged")));
	InventoryUpdateHandle = InventorySource->GetInventoryItemChangedNativeDelegate().AddUObject(this, &AShooterCharacterBase::OnInventoryItemChanged);
	//InventorySource->GetInventoryItemChangedDelegate().Add(InventoryUpdateHandle);
	InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &AShooterCharacterBase::RefreshSlottedGameplayAbilities);
	
	/*
	// Initialize our abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}*/
	
	//AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(NewController);
	AShooterPlayerState* PS = Cast<AShooterPlayerState>(PlayerState);
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterBase::PossessedBy( name = %s)"), *PS->GetShortPlayerName());
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UShooterAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();
		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeDefaultAttributes();

		AddStartupGameplayAbilities();

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
	}
}

void AShooterCharacterBase::UnPossessed()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::UnPossessed()"));
	// Unmap from inventory source
	if (InventorySource && InventoryLoadedHandle.IsValid())
	{
		InventorySource->GetInventoryItemChangedNativeDelegate().Remove(InventoryUpdateHandle);
		InventoryUpdateHandle.Reset();

		InventorySource->GetInventoryLoadedDelegate().Remove(InventoryLoadedHandle);
		InventoryLoadedHandle.Reset();
		UE_LOG(LogTemp, Warning, TEXT("CharacterBase::UnPossessed( Inventory Delegate Removed in Character.)"));
	}

	InventorySource = nullptr;
}

void AShooterCharacterBase::ReceivePossessed_Implementation(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::ReceivePossessed_Implementation()"));
	Super::ReceivePossessed(NewController);
	//AShooterPlayerController* PC = Cast<AShooterPlayerController>(NewController);
	// Try setting the inventory source, this will fail for AI
	//InventorySource = NewController;
	//AShooterPlayerController* PC = Cast<AShooterPlayerController>(NewController);

	//InventoryUpdateHandle = InventorySource->GetInventoryItemChangedDelegate().AddDynamic(this, &AShooterCharacterBase::OnInventoryItemChanged);
	//InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &AShooterCharacterBase::RefreshSlottedGameplayAbilities);
	//if (InventorySource)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::PossessedBy( CharacterBase->InventorySource UpdateDelegates bound.)"));
	//	InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &AShooterCharacterBase::OnItemSlotChanged);
	//	InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &AShooterCharacterBase::RefreshSlottedGameplayAbilities);
	//}

	// Initialize our abilities
	//if (AbilitySystemComponent)
	//{
	//	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	//	AddStartupGameplayAbilities();
	//}
}

void AShooterCharacterBase::OnRep_Controller()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnRep_Controller()"));
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}
/*
void AShooterCharacterBase::OnRep_CurrentWeapon(AShooterWeaponBase* LastWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Character::OnRep_CurrentWeapon()"));
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AShooterCharacterBase::OnRep_CurrentWeaponType(EShooterWeaponType weaponType)
{
	SetCurrentWeaponType(weaponType);
}

void AShooterCharacterBase::SetCurrentWeaponType(EShooterWeaponType activeType)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::SetCurrentWeaponType()"));
	CurrentWeaponType = activeType;
}

void AShooterCharacterBase::SetCurrentWeapon(AShooterWeaponBase* NewWeapon, AShooterWeaponBase* LastWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::SetCurrentWeapon()"));

}*/

void AShooterCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacterBase, CharacterLevel);
}

void AShooterCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AShooterPlayerState* SPS = Cast<AShooterPlayerState>(PlayerState);
	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local 
	if (SPS != NULL)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UShooterAbilitySystemComponent>(SPS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(SPS, this);

		// Set the AttributeSet for convenience attribute functions
		AttributeSetBase = SPS->GetAttributeSetBase();


	}
}

bool AShooterCharacterBase::IsAlive() const
{
	return GetHealth() > 0;
}

bool AShooterCharacterBase::IsUsingMelee()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::IsUsingMelee()"));
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

bool AShooterCharacterBase::IsUsingSkill()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::IsUsingSkill()"));
	TArray<UShooterGameplayAbility*> ActiveAbilities;

	FGameplayTag AbilityTag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Skill"));
	GetActiveAbilitiesWithTags(FGameplayTagContainer(AbilityTag), ActiveAbilities);
	if (ActiveAbilities.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::Using Skill.--------"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character::Not Using Skill.--------"));
	}
	return ActiveAbilities.Num() > 0;
}

bool AShooterCharacterBase::CanUseAnyAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::CanUseAnyAbility()"));
	return IsAlive() && (IsUsingSkill() == false);
}

bool AShooterCharacterBase::IsPlayHighPriorityMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::IsPlayHighPriorityMontage()"));
	//UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	return GetMesh()->GetAnimInstance()->Montage_IsPlaying(HighPriorityMontage);
}

void AShooterCharacterBase::PlayHighPriorityMontage(UAnimMontage* Montage, FName SectionName)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::PlayAnimMontage( StartSectionName = %s )"), *SectionName.ToString());
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();

	if (Montage && animInstance && animInstance->Montage_IsPlaying(Montage))
	{
		HighPriorityMontage = Montage;
		PlayAnimMontage(HighPriorityMontage, 1.f, SectionName);
		//GetMesh()->GetAnimInstance();
		//return UseMesh->AnimScriptInstance->Montage_Play(Montage, InPlayRate);
	}
}

void AShooterCharacterBase::DoMeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::DoMeleeAttack()"));
	
	if (IsUsingMelee() && CanUseAnyAbility())
	{
		ActivateAbilitiesWithItemSlot(CurrentWeaponSlot, true);
	}
}

void AShooterCharacterBase::DoSkillAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::DoSkillAttack()"));
	if (CanUseAnyAbility())
	{
		FGameplayTag Tag = UShooterBlueprintLibrary::GetGameplayTag(TEXT("Ability.Skill"));
		//FGameplayTagContainer
		ActivateAbilitiesWithTags(FGameplayTagContainer(Tag), true);
	}
}

float AShooterCharacterBase::GetHealth() const
{
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_SimulatedProxy"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Character::ROLE_None"));
		break;
	}
	AShooterPlayerState* PS = Cast<AShooterPlayerState>(PlayerState);
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetHealth( NikeName = %s)"), *PS->GetShortPlayerName());
		UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetHealth(PS.Health = %f"), PS->GetHealth());
	}
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetHealth() = %f"), AttributeSetBase->GetHealth());
	return AttributeSetBase->GetHealth();
}

float AShooterCharacterBase::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AShooterCharacterBase::GetRestoreHealth() const
{
	return AttributeSetBase->GetRestoreHealth();
}

float AShooterCharacterBase::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AShooterCharacterBase::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AShooterCharacterBase::GetRestoreMana() const
{
	return AttributeSetBase->GetRestoreMana();
}

float AShooterCharacterBase::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

void AShooterCharacterBase::SetHealth(float Health)
{
	if (Health < 0 )
	{
		Health = 0.f;
	}
	AttributeSetBase->SetHealth(Health);
}

void AShooterCharacterBase::SetMana(float Mana)
{
	if (Mana < 0)
	{
		Mana = 0.f;
	}
	AttributeSetBase->SetMana(Mana);
}

int32 AShooterCharacterBase::GetCharacterLevel() const
{
	return 1;
}

bool AShooterCharacterBase::SetCharacterLevel(int32 NewLevel)
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
}

bool AShooterCharacterBase::ActivateAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, bool bAllowRemoteActivation)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::ActivateAbilitiesWithItemSlot()"));
	/*
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
	}*/
	FGameplayAbilitySpecHandle* FoundHandle = SlottedAbilities.Find(ItemSlot);

	if (FoundHandle && AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}

	return false;
}

void AShooterCharacterBase::GetActiveAbilitiesWithItemSlot(FShooterItemSlot ItemSlot, TArray<UShooterGameplayAbility*>& ActiveAbilities)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetActiveAbilitiesWithItemSlot()"));
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

bool AShooterCharacterBase::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::ActivateAbilitiesWithTags()"));
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void AShooterCharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UShooterGameplayAbility*>& ActiveAbilities)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetActiveAbilitiesWithTags()"));
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool AShooterCharacterBase::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::GetCooldownRemainingForTag()"));
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
}

void AShooterCharacterBase::HandleDamage(float DamageAmount, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::HandleDamage()"));
	OnDamaged(DamageAmount, DamagedActor, DamageEvent, EventInstigator, DamageCauser);
}

void AShooterCharacterBase::OnDamaged_Implementation(float DamageAmount, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnDamaged_Implementation( CurrentHealth = %d)"), GetHealth());
}

void AShooterCharacterBase::HandleHealthChanged(float DeltaValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::HandleHealthChanged()"));
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue);
	}
}

void AShooterCharacterBase::OnHealthChanged_Implementation(float DeltaValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnHealthChanged_Implementation( CurrentHealth = %d)"), GetHealth());

	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	//MyPC->NotifyHPChanged(GetHealth(), GetMaxHealth(), GetRestoreHealth());
	//AShooterHUD* ShooterHUD = MyPC->GetShooterHUD();
	if (MyPC != nullptr && bAbilitiesInitialized)
	{
		MyPC->ClientReceivePlayerStateChangeEvent(GetHealth(), GetMaxHealth(), GetRestoreHealth());
	}
}

void AShooterCharacterBase::HandleManaChanged(float DeltaValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::HandleManaChanged()"));
	if (bAbilitiesInitialized)
	{
		OnManaChanged(DeltaValue);
	}
}

void AShooterCharacterBase::OnManaChanged_Implementation(float DeltaValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::OnManaChanged_Implementation()"));
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	MyPC->NotifyMPChanged(GetMana(), GetMaxMana(), GetRestoreMana());
}

void AShooterCharacterBase::HandleMoveSpeedChanged(float DeltaValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterBase::HandleMoveSpeedChanged()"));
	// Update the character movement's walk speed
	GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();

	if (bAbilitiesInitialized)
	{
		OnMoveSpeedChanged(DeltaValue);
	}
}

void AShooterCharacterBase::OnMoveSpeedChanged_Implementation(float DeltaValue)
{

}
