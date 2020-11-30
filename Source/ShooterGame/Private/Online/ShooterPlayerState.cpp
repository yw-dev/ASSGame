// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "Abilities/ShooterAttributeSet.h"
#include "Abilities/ShooterAbilitySystemComponent.h"
#include "Player/ShooterCharacterBase.h"
#include "Player/ShooterPlayerController.h"
#include "ShooterPlayerState.h"

AShooterPlayerState::AShooterPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	NumCoins = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
	SlottedItems.Reserve(PLAYER_ALL_SLOT_COUNT);
	InventorySlot.Reserve(PLAYER_INVENTORY_SLOT_COUNT);
	WeaponSlot.Reserve(PLAYER_WEAPON_SLOT_COUNT);
	SkillSlot.Reserve(PLAYER_ABILITY_SLOT_COUNT);

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UShooterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UShooterAttributeSet>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AShooterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UShooterAttributeSet * AShooterPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

void AShooterPlayerState::Reset()
{
	Super::Reset();
	
	//PlayerStates persist across seamless travel.  Keep the same teams as previous match.
	//SetTeamNum(0);
	SlottedItems.Reserve(PLAYER_ALL_SLOT_COUNT);
	InventorySlot.Reserve(PLAYER_INVENTORY_SLOT_COUNT);
	WeaponSlot.Reserve(PLAYER_WEAPON_SLOT_COUNT);
	SkillSlot.Reserve(PLAYER_ABILITY_SLOT_COUNT);
	NumKills = 0;
	NumDeaths = 0;
	NumCoins = 0;
	NumBulletsFired = 0;
	NumRocketsFired = 0;
	bQuitter = false;
}

void AShooterPlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

void AShooterPlayerState::ClientInitialize(AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void AShooterPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;

	UpdateTeamColors();
}

void AShooterPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void AShooterPlayerState::AddBulletsFired(int32 NumBullets)
{
	NumBulletsFired += NumBullets;
}

void AShooterPlayerState::FindItemData(UShooterItem* Item, FShooterItemData& ItemData) const
{
	const FShooterItemData* FoundItem = InventorySlot.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		UE_LOG(LogTemp, Warning, TEXT("PlayerState::FindItemData(ItemData.ItemCount : %d)"), ItemData.ItemCount);
		return;
	}
	ItemData = FShooterItemData(0, 0);
}

void AShooterPlayerState::FindItemSlot(UShooterItem* Item, FShooterItemSlot& ItemSlot) const
{
	const FShooterItemSlot* FoundSlot = SlottedItems.FindKey(Item);

	if (FoundSlot)
	{
		ItemSlot = *FoundSlot;
		UE_LOG(LogTemp, Warning, TEXT("PlayerState::FindItemSlot(ItemSlot.SlotNumber : %d)"), ItemSlot.SlotNumber);
		return;
	}
	ItemSlot = FShooterItemSlot();
}

void AShooterPlayerState::AddSlottedItems(FShooterItemSlot NewSlot, UShooterItem* NewItem)
{
	SlottedItems.Add(NewSlot, NewItem);
}

void AShooterPlayerState::AddInventoryItems(FShooterItemData NewData, UShooterItem* NewItem)
{
	InventorySlot.Add(NewItem, NewData);
}

void AShooterPlayerState::AddWeaponItems(FShooterItemData NewData, UShooterItem* NewItem)
{
	WeaponSlot.Add(NewItem, NewData);
}

void AShooterPlayerState::AddSkillItems(FShooterItemData NewData, UShooterItem* NewItem)
{
	SkillSlot.Add(NewItem, NewData);
}

void AShooterPlayerState::RemoveSlottedItems(FShooterItemSlot NewSlot, UShooterItem* NewItem)
{
	//FShooterItemSlot ItemSlot = FindItemSlot(NewItem, NewSlot);
	SlottedItems[NewSlot] = nullptr;
}

void AShooterPlayerState::RemoveInventoryItems(UShooterItem* NewItem)
{
	InventorySlot.Remove(NewItem);
}

void AShooterPlayerState::RemoveWeaponItems(UShooterItem* NewItem)
{
	WeaponSlot.Remove(NewItem);
}

void AShooterPlayerState::RemoveSkillItems(UShooterItem* NewItem)
{
	SkillSlot.Remove(NewItem);
}

void AShooterPlayerState::AddRocketsFired(int32 NumRockets)
{
	NumRocketsFired += NumRockets;
}

void AShooterPlayerState::SetQuitter(bool bInQuitter)
{
	bQuitter = bInQuitter;
}

void AShooterPlayerState::CopyProperties(APlayerState* PlayerState)
{	
	Super::CopyProperties(PlayerState);

	AShooterPlayerState* ShooterPlayer = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayer)
	{
		ShooterPlayer->TeamNumber = TeamNumber;
	}	
}

void AShooterPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OwnerController->GetCharacter());
		if (ShooterCharacter != NULL)
		{
			ShooterCharacter->UpdateTeamColorsAllMIDs();
		}
	}
}

void AShooterPlayerState::UpdateInventorySlotted(FShooterItemSlot NewSlot, UShooterItem* NewItem)
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OwnerController->GetCharacter());
		if (ShooterCharacter != NULL)
		{
			ShooterCharacter->UpdateInventoryItems(NewSlot, NewItem);
		}
	}
}

float AShooterPlayerState::GetHealth() const
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayerState::GetHealth() = %d"), AttributeSetBase->GetHealth());
	return AttributeSetBase->GetHealth();
}

float AShooterPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AShooterPlayerState::GetRestoreHealth() const
{
	return AttributeSetBase->GetRestoreHealth();
}

float AShooterPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AShooterPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AShooterPlayerState::GetRestoreMana() const
{
	return AttributeSetBase->GetRestoreMana();
}

float AShooterPlayerState::GetMaxEXP() const
{
	return AttributeSetBase->GetMaxEXP();
}

float AShooterPlayerState::GetProvideEXP() const
{
	return AttributeSetBase->GetProvideEXP();
}

float AShooterPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

int32 AShooterPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 AShooterPlayerState::GetKills() const
{
	return NumKills;
}

int32 AShooterPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float AShooterPlayerState::GetScore() const
{
	return Score;
}

int32 AShooterPlayerState::GetNumBulletsFired() const
{
	return NumBulletsFired;
}

int32 AShooterPlayerState::GetNumRocketsFired() const
{
	return NumRocketsFired;
}

int32 AShooterPlayerState::GetNumCoins() const
{
	return NumCoins;
}

int32 AShooterPlayerState::GetNumWeaponItems() const
{
	return WeaponSlot.Num();
}

int32 AShooterPlayerState::GetNumInventoryItems() const
{
	return InventorySlot.Num();
}

int32 AShooterPlayerState::GetNumSkillItems() const
{
	return SkillSlot.Num();
}

bool AShooterPlayerState::IsQuitter() const
{
	return bQuitter;
}

void AShooterPlayerState::ScoreKill(AShooterPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void AShooterPlayerState::ScoreDeath(AShooterPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void AShooterPlayerState::ScorePoints(int32 Points)
{
	AShooterGameState* const MyGameState = GetWorld()->GetGameState<AShooterGameState>();
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	Score += Points;
}

void AShooterPlayerState::CalculateCoins(int32 Bounty)
{
	NumCoins += Bounty;
	UE_LOG(LogTemp, Warning, TEXT("AShooterPlayerState::CalculateCoins( NumCoins = %d )"), NumCoins);
}

void AShooterPlayerState::CalculateCoins(AShooterPlayerState* KilledBy, int32 Bounty)
{
	NumCoins += Bounty;
	UE_LOG(LogTemp, Warning, TEXT("AShooterPlayerState::CalculateCoins( NumCoins = %d )"), NumCoins);
}

void AShooterPlayerState::InformAboutInventory_Implementation(class AShooterPlayerState* OwnerPlayerState, const UShooterItem* Item, bool bAdd)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			//TestPC->OnInventoryMessage(OwnerPlayerState, Item, bAdd);
		}
	}
}

void AShooterPlayerState::InformAboutCoins_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void AShooterPlayerState::InformAboutKill_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{
	//id can be null for bots
	if (KillerPlayerState->UniqueId.IsValid())
	{	
		//search for the actual killer before calling OnKill()	
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{		
			AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
			if (TestPC && TestPC->IsLocalController())
			{
				// a local player might not have an ID if it was created with CreateDebugPlayer.
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(TestPC->Player);
				FUniqueNetIdRepl LocalID = LocalPlayer->GetCachedUniqueNetId();
				if (LocalID.IsValid() &&  *LocalPlayer->GetCachedUniqueNetId() == *KillerPlayerState->UniqueId)
				{			
					TestPC->OnKill();
				}
			}
		}
	}
}

void AShooterPlayerState::BroadcastDeath_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);				
		}
	}	
}

void AShooterPlayerState::BroadcastConnected_Implementation(class AShooterPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AShooterPlayerState* KilledPlayerState)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// all local players get death messages so they can update their huds.
		AShooterPlayerController* TestPC = Cast<AShooterPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())
		{
			TestPC->OnConnectedMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void AShooterPlayerState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( AShooterPlayerState, TeamNumber );
	DOREPLIFETIME( AShooterPlayerState, NumKills );
	DOREPLIFETIME( AShooterPlayerState, NumDeaths );
	DOREPLIFETIME( AShooterPlayerState, NumCoins);
}

FString AShooterPlayerState::GetShortPlayerName() const
{
	if( GetPlayerName().Len() > MAX_PLAYER_NAME_LENGTH )
	{
		return GetPlayerName().Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}
	return GetPlayerName();
}
