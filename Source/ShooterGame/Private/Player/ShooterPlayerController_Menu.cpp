// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterTypes.h"
#include "ShooterGameInstance.h"
#include "ShooterAssetManager.h"
#include "Items/ShooterItem.h"
#include "Player/ShooterLocalPlayer.h"
#include "Online/ShooterPlayerState.h"
#include "UI/ShooterHUD.h"
#include "Online.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineEventsInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSessionInterface.h"
#include "Player/ShooterPlayerController_Menu.h"


AShooterPlayerController_Menu::AShooterPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Health = 100;
	bHasSentStartEvents = false;
	ServerSayString = TEXT("Say");

	//ViewSource.SetOnViewSourceChanged(FShooterPawnSourceViewModel::FOnViewSourceChanged::CreateUObject(this, &AShooterPlayerController_Menu::BuildViewSourceViewModels));

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
}

void AShooterPlayerController_Menu::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	FShooterStyle::Initialize();
	LoadPawnSource();
}

void AShooterPlayerController_Menu::BuildViewSourceViewModels()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::BuildViewSourceViewModels()"));
}

void AShooterPlayerController_Menu::SpawnPawnActor()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::SpawnPawnActor()"));
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UObject* loadObj = StaticLoadObject(UBlueprint::StaticClass(), NULL, TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA'"));
	if (loadObj != nullptr)
	{
		FVector Loc = FVector(-1346.0, 386.0, 126.0);
		FRotator Rot = FRotator(0,0,180);
		UBlueprint* ubp = Cast<UBlueprint>(loadObj);
		AShooterCharacter* spawnActor = GetWorld()->SpawnActor<AShooterCharacter>(ubp->GetClass(), Loc, Rot, SpawnInfo);
		UE_LOG(LogClass, Log, TEXT("Success"));
	}
	//AShooterCharacter* AIC = GetWorld()->SpawnActor<AShooterCharacter>(SpawnInfo);
	//if (AIC->PlayerState)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Game_Menu::PostLogin()"));
		//FString BotName = FString::Printf(TEXT("Bot %d"), BotNum);
		//AIC->PlayerState->SetPlayerName(BotName);
	//}
}

void AShooterPlayerController_Menu::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

	if (!ensure(SGI != NULL))
	{
		return;
	}

	if (ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		//Build menu only after game is initialized
		PawnGuideMenu = MakeShareable(new FShooterPawnGuide());
		PawnGuideMenu->Construct(SGI, Cast<ULocalPlayer>(Player));

		//FInputModeGameOnly InputMode;
		//SetInputMode(InputMode);
	}
}

void AShooterPlayerController_Menu::GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList)
{

}

void AShooterPlayerController_Menu::ClientGameStarted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::ClientGameStarted_Implementation()"));
	//bAllowGameActions = true;

	// Enable controls mode now the game has started
	//SetIgnoreMoveInput(false);

	//AShooterHUD* ShooterHUD = GetShooterHUD();
	//if (ShooterHUD)
	//{
	//	ShooterHUD->SetMatchState(EShooterMatchState::Playing);
	//	ShooterHUD->ShowScoreboard(false);
	//}
	//bGameEndedFrame = false;

	//QueryAchievements();

	//QueryStats();

	// Send round start event
	const auto Events = Online::GetEventsInterface();
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if (LocalPlayer != nullptr && Events.IsValid())
	{
		auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();

		if (UniqueId.IsValid())
		{
			// Generate a new session id
			Events->SetPlayerSessionId(*UniqueId, FGuid::NewGuid());

			FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());

			// Fire session start event for all cases
			FOnlineEventParms Params;
			Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
			Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused
			Params.Add(TEXT("MapName"), FVariantData(MapName));

			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionStart"), Params);

			// Online matches require the MultiplayerRoundStart event as well
			UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

			if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online))
			{
				FOnlineEventParms MultiplayerParams;

				// @todo: fill in with real values
				MultiplayerParams.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
				MultiplayerParams.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
				MultiplayerParams.Add(TEXT("MatchTypeId"), FVariantData((int32)1)); // @todo abstract the specific meaning of this value across platforms
				MultiplayerParams.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

				Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundStart"), MultiplayerParams);
			}

			bHasSentStartEvents = true;
		}
	}
}

/** Starts the online game using the session name in the PlayerState */
void AShooterPlayerController_Menu::ClientStartOnlineGame_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::ClientStartOnlineGame_Implementation()"));
	if (!IsPrimaryPlayer())
		return;

	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && (Sessions->GetNamedSession(ShooterPlayerState->SessionName) != nullptr))
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *ShooterPlayerState->SessionName.ToString());
				Sessions->StartSession(ShooterPlayerState->SessionName);
			}
		}
	}
	else
	{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AShooterPlayerController_Menu::ClientStartOnlineGame_Implementation, 0.2f, false);
	}
}

/** Ends the online game using the session name in the PlayerState */
void AShooterPlayerController_Menu::ClientEndOnlineGame_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::ClientEndOnlineGame_Implementation()"));
	if (!IsPrimaryPlayer())
		return;

	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && (Sessions->GetNamedSession(ShooterPlayerState->SessionName) != nullptr))
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *ShooterPlayerState->SessionName.ToString());
				Sessions->EndSession(ShooterPlayerState->SessionName);
			}
		}
	}
}

void AShooterPlayerController_Menu::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::ClientGameEnded_Implementation()"));
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Disable controls now the game has ended
	SetIgnoreMoveInput(true);

	//bAllowGameActions = false;

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

	//AShooterHUD* ShooterHUD = GetShooterHUD();
	//if (ShooterHUD)
	//{
	//	ShooterHUD->SetMatchState(bIsWinner ? EShooterMatchState::Won : EShooterMatchState::Lost);
	//}

	//UpdateSaveFileOnGameEnd(bIsWinner);
	//UpdateAchievementsOnGameEnd();
	//UpdateLeaderboardsOnGameEnd();

	// Flag that the game has just ended (if it's ended due to host loss we want to wait for ClientReturnToMainMenu_Implementation first, incase we don't want to process)
	//bGameEndedFrame = true;
}

void AShooterPlayerController_Menu::Say(const FString& Msg)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::Say()"));
	ServerSay(Msg.Left(128));
}

bool AShooterPlayerController_Menu::ServerSay_Validate(const FString& Msg)
{
	return true;
}

void AShooterPlayerController_Menu::ServerSay_Implementation(const FString& Msg)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller_Menu::ServerSay()"));
	GetWorld()->GetAuthGameMode<AShooterGameMode>()->Broadcast(this, Msg, ServerSayString);
}

void AShooterPlayerController_Menu::ClientReturnToMainMenuWithTextReason_Implementation(const FText& InReturnReason)
{
	UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

	if (!ensure(SGI != NULL))
	{
		return;
	}

	if (GetNetMode() == NM_Client)
	{
		const FText ReturnReason = NSLOCTEXT("NetworkErrors", "HostQuit", "The host has quit the match.");
		const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

		SGI->ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), ShooterGameInstanceState::PawnGuide);
	}
	else
	{
		SGI->GotoState(ShooterGameInstanceState::PawnGuide);
	}
}

bool AShooterPlayerController_Menu::LoadPawnSource()
{
	PawnSource.Reset();
	SlottedItems.Reset();

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FShooterItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UShooterAssetManager& AssetManager = UShooterAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FShooterItemData>& ItemPair : CurrentSaveGame->AssetDataSource)
		{
			UShooterItem* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

			if (LoadedItem != nullptr)
			{
				PawnSource.Add(LoadedItem, ItemPair.Value);
			}
		}

		for (const TPair<FShooterItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UShooterItem* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}

		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			//FillEmptySlots();
		}

		NotifyInventoryLoaded();

		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	NotifyInventoryLoaded();

	return false;
}

bool AShooterPlayerController_Menu::SaveInventory()
{
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		CurrentSaveGame->AssetDataSource.Reset();
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<UShooterItem*, FShooterItemData>& ItemPair : PawnSource)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->AssetDataSource.Add(AssetId, ItemPair.Value);
			}
		}

		for (const TPair<FShooterItemSlot, UShooterItem*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool AShooterPlayerController_Menu::SetSlottedItem(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	// Iterate entire inventory because we need to remove from old slot
	bool bFound = false;
	for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			// Add to new slot
			bFound = true;
			Pair.Value = Item;
			//NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			// If this item was found in another slot, remove it
			Pair.Value = nullptr;
			//NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		SaveInventory();
		return true;
	}

	return false;
}

UShooterItem* AShooterPlayerController_Menu::GetSlottedItem(FShooterItemSlot ItemSlot) const
{
	UShooterItem* const* FoundItem = SlottedItems.Find(ItemSlot);

	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

int32 AShooterPlayerController_Menu::GetInventoryItemCount(UShooterItem* Item) const
{
	const FShooterItemData* FoundItem = PawnSource.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool AShooterPlayerController_Menu::FillEmptySlotWithItem(UShooterItem* NewItem)
{
	// Look for an empty item slot to fill with this item
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FShooterItemSlot EmptySlot;
	for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		//NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}

	return false;
}

void AShooterPlayerController_Menu::NotifyInventoryLoaded()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("PawnSource Loaded !"));
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}



