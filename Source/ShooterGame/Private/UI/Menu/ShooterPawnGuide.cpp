// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPawnGuide.h"
#include "ShooterGameLoadingScreen.h"
#include "ShooterStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "SShooterConfirmationDialog.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterGameUserSettings.h"
#include "ShooterGameViewportClient.h"
#include "ShooterPersistentUser.h"
#include "ShooterAssetManager.h"
#include "Player/ShooterLocalPlayer.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.PawnGuide"


// Sets default values
FShooterPawnGuide::~FShooterPawnGuide()
{
	auto Sessions = Online::GetSessionInterface();
	CleanupOnlinePrivilegeTask();
}

void FShooterPawnGuide::Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner)
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuide::Construct()"));
	check(_GameInstance.IsValid());
	auto Sessions = Online::GetSessionInterface();

	GameInstance = _GameInstance;
	PlayerOwner = _PlayerOwner;

	//PawnGuideWidget.Reset();
	PawnGuideWidgetContainer.Reset();

	//ViewModel->SetOnViewSourcesComplete(FShooterPawnGuideViewModel::FOnViewSourcesComplete::CreateSP(this, &FShooterPawnGuide::OnViewSourcesComplete));

	bIsRecordingDemo = false;

	// read user settings
#if SHOOTER_CONSOLE_UI
	bIsLanMatch = FParse::Param(FCommandLine::Get(), TEXT("forcelan"));
#else
	UShooterGameUserSettings* const UserSettings = CastChecked<UShooterGameUserSettings>(GEngine->GetGameUserSettings());
	bIsLanMatch = UserSettings->IsLanMatch();
	bIsDedicatedServer = UserSettings->IsDedicatedServer();
#endif

	TArray<FString> Keys;
	GConfig->GetSingleLineArray(TEXT("/Script/SwitchRuntimeSettings.SwitchRuntimeSettings"), TEXT("LeaderboardMap"), Keys, GEngineIni);

	if (GEngine && GEngine->GameViewport)
	{
		/*
		PawnGuideWidget = SNew(SShooterPawnGuideWidget)
			.Cursor(EMouseCursor::Default)
			.PlayerOwner(GetPlayerOwner())
			.ViewModel(ViewModel)
			.IsGameMenu(false);

		SAssignNew(PawnGuideWidgetContainer, SWeakWidget)
			.PossiblyNullContent(PawnGuideWidget);*/
	}


}

// Called every frame
void FShooterPawnGuide::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

void FShooterPawnGuide::OnViewSourcesComplete()
{
	LoadPawnDataSource();
}

void FShooterPawnGuide::AddMenuToGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* const GVC = GEngine->GameViewport;
		GVC->AddViewportWidgetContent(PawnGuideWidgetContainer.ToSharedRef());
	}
}

void FShooterPawnGuide::RemoveMenuFromGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(PawnGuideWidgetContainer.ToSharedRef());
	}
}

void FShooterPawnGuide::HostGame(const FString& GameType)
{
	if (ensure(GameInstance.IsValid()) && GetPlayerOwner() != NULL)
	{
		FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s%s%s?%s"), 
												 TEXT("Highrise"), 
												 *GameType, 
												 GameInstance->GetOnlineMode() != EOnlineMode::Offline ? TEXT("?listen") : TEXT(""), 
												 GameInstance->GetOnlineMode() == EOnlineMode::LAN ? TEXT("?bIsLanMatch") : TEXT(""), 
												 bIsRecordingDemo ? TEXT("?DemoRec") : TEXT("")
												);

		// Game instance will handle success, failure and dialogs
		GameInstance->HostGame(GetPlayerOwner(), GameType, StartURL);
	}
}

void FShooterPawnGuide::HostTeamDeathMatch()
{
	HostGame(LOCTEXT("TDM", "TDM").ToString());
}

void FShooterPawnGuide::ShowMenuView()
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuide::ShowMenuView()"));
	/*
	if (!PawnGuideWidget.IsValid())
	{
		return;
	}*/
	if (GetPlayerOwnerControllerId() == -1)
	{
		UE_LOG(LogShooter, Log, TEXT("Trying to toggle in-game menu for invalid userid"));
		return;
	}

	//AShooterPlayerController* const PCOwner = PlayerOwner ? Cast<AShooterPlayerController>(PlayerOwner->PlayerController) : nullptr;

	//GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(PawnGuideWidgetContainer, SWeakWidget).PossiblyNullContent(PawnGuideWidget.ToSharedRef()));

	//if (PCOwner)
	//{
		// Disable controls while paused
	//	PCOwner->SetCinematicMode(true, false, false, true, true);

	//}
}

void FShooterPawnGuide::LoadPawnDataSource()
{/*
	UE_LOG(LogTemp, Warning, TEXT("PawnGuide::LoadPawnDataSource()"));
	PawnSource.Reset();
	SlottedItems.Reset();
	TArray<TSharedPtr<FShooterPawnSouceViewModel>> SourceVM;

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		if (Pair.Key == UShooterAssetManager::PawnItemType)
		{
			for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
			{
				SlottedItems.Add(FShooterItemSlot(Pair.Key, SlotNumber), nullptr);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("PawnGuide::ItemSlotsPerType num:%d "), GameInstance->ItemSlotsPerType.Num());
	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UShooterAssetManager& AssetManager = UShooterAssetManager::Get();

	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FShooterItemData>& ItemPair : CurrentSaveGame->AssetDataSource)
		{
			if (ItemPair.Key.PrimaryAssetType == UShooterAssetManager::PawnItemType)
			{
				UShooterItem* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

				if (LoadedItem != nullptr)
				{
					PawnSource.Add(LoadedItem, ItemPair.Value);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("PawnGuide::AssetDataSource num:%d "), CurrentSaveGame->AssetDataSource.Num());
		UE_LOG(LogTemp, Warning, TEXT("PawnGuide::ViewSource num:%d "), PawnSource.Num());


		for (const TPair<FShooterItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid() && SlotPair.Value.PrimaryAssetType == UShooterAssetManager::PawnItemType)
			{
				UShooterItem* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("PawnGuide::SlottedItems num:%d "), CurrentSaveGame->SlottedItems.Num());
		UE_LOG(LogTemp, Warning, TEXT("PawnGuide::ViewSource num:%d "), SlottedItems.Num());

		TArray<class UShooterItem*> AssetSource = GameInstance->GetAssetSourceItems(UShooterAssetManager::PawnItemType);
		*/
		/*
		for (const class UShooterItem* Item : AssetSource)
		{
			//FShooterItemData* ItemData = PawnSource.Find(SlotPair.Value);
			//UShooterPawnItem* Item = Cast<UShooterPawnItem>(AssetSource[index]);
			const FShooterItemSlot& ItemSLot = FShooterItemSlot(UShooterAssetManager::PawnItemType, index);
			ViewSource.Add(FShooterPawnViewModel(ItemSLot, Item));
			index++;
		}*/
	/*
		if (AssetSource.Num() > 0)
		{
			//int32 index = 0;
			for (UShooterItem* Item: AssetSource)
			{
				//FShooterItemData* ItemData = PawnSource.Find(SlotPair.Value);
					UShooterPawnItem* PawnItem = Cast<UShooterPawnItem>(Item);
					TSharedPtr<FShooterPawnSource> Pawn = MakeShareable(new FShooterPawnSource(GetPlayerOwner(), PawnItem));
					SourceVM.Add(MakeShareable(new FShooterPawnSouceViewModel(Pawn)));
				//index++;
			}
			UE_LOG(LogTemp, Warning, TEXT("PawnGuide::ViewSource num:%d "), SourceVM.Num());
			ViewModel = MakeShareable(new FShooterPawnGuideViewModel(SourceVM));
		}
	}*/
}

void FShooterPawnGuide::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuide::StartOnlinePrivilegeTask()"));
	if (GameInstance.IsValid())
	{
		// Lock controls for the duration of the async task
		//PawnGuideWidget->LockControls(true);
		FUniqueNetIdRepl UserId;
		if (PlayerOwner.IsValid())
		{
			UserId = PlayerOwner->GetPreferredUniqueNetId();
		}
		GameInstance->StartOnlinePrivilegeTask(Delegate, EUserPrivileges::CanPlayOnline, UserId.GetUniqueNetId());
	}
}

void FShooterPawnGuide::CleanupOnlinePrivilegeTask()
{
	if (GameInstance.IsValid())
	{
		GameInstance->CleanupOnlinePrivilegeTask();
	}
}

TStatId FShooterPawnGuide::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FShooterPawnGuide, STATGROUP_Tickables);
}

UWorld* FShooterPawnGuide::GetTickableGameObjectWorld() const
{
	//UE_LOG(LogTemp, Warning, TEXT("PawnGuide::GetTickableGameObjectWorld()"));
	ULocalPlayer* LocalPlayerOwner = GetPlayerOwner();
	return (LocalPlayerOwner ? LocalPlayerOwner->GetWorld() : nullptr);
}

ULocalPlayer* FShooterPawnGuide::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}
/*
const TArray<FShooterRaceEntry>* FShooterPawnGuide::GetCategories()
{
	return &RaceCategories;
}*/

int32 FShooterPawnGuide::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}





#undef LOCTEXT_NAMESPACE
