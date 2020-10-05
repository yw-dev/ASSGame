// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	ShooterGameInstance.cpp
=============================================================================*/

#include "ShooterGame.h"
#include "Engine/DataAsset.h"
#include "ShooterGameInstance.h"
#include "ShooterMainMenu.h"
#include "ShooterWelcomeMenu.h"
#include "ShooterLogin.h"
#include "ShooterPawnGuide.h"
#include "ShooterMessageMenu.h"
#include "ShooterGameLoadingScreen.h"
#include "OnlineKeyValuePair.h"
#include "ShooterStyle.h"
#include "Engine/AssetManager.h"
#include "ShooterAssetManager.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterGameViewportClient.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "Online/ShooterPlayerState.h"
#include "Online/ShooterGameSession.h"
#include "Online/ShooterOnlineSessionClient.h"

FAutoConsoleVariable CVarShooterGameTestEncryption(TEXT("ShooterGame.TestEncryption"), 0, TEXT("If true, clients will send an encryption token with their request to join the server and attempt to encrypt the connection using a debug key. This is NOT SECURE and for demonstration purposes only."));

void SShooterWaitDialog::Construct(const FArguments& InArgs)
{
	const FShooterMenuItemStyle* ItemStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuItemStyle>("DefaultShooterMenuItemStyle");
	const FButtonStyle* ButtonStyle = &FShooterStyle::Get().GetWidgetStyle<FButtonStyle>("DefaultShooterButtonStyle");
	ChildSlot
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(20.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.Padding(50.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.BorderImage(&ItemStyle->BackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				[
					SNew(STextBlock)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
					.ColorAndOpacity(this, &SShooterWaitDialog::GetTextColor)
					.Text(InArgs._MessageText)
					.WrapTextAt(500.0f)
				]
			]
		];

	//Setup a curve
	const float StartDelay = 0.0f;
	const float SecondDelay = 0.0f;
	const float AnimDuration = 2.0f;

	WidgetAnimation = FCurveSequence();
	TextColorCurve = WidgetAnimation.AddCurve(StartDelay + SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	WidgetAnimation.Play(this->AsShared(), true);
}

FSlateColor SShooterWaitDialog::GetTextColor() const
{
	//instead of going from black -> white, go from white -> grey.
	float fAlpha = 1.0f - TextColorCurve.GetLerp();
	fAlpha = fAlpha * 0.5f + 0.5f;
	return FLinearColor(FColor(155, 164, 182, FMath::Clamp((int32)(fAlpha * 255.0f), 0, 255)));
}

namespace ShooterGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName WelcomeScreen = FName(TEXT("WelcomeScreen"));
	const FName ConnectingServer = FName(TEXT("ConnectingServer"));
	const FName PawnGuide = FName(TEXT("PawnGuide"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName MessageMenu = FName(TEXT("MessageMenu"));
	const FName Playing = FName(TEXT("Playing"));
}


UShooterGameInstance::UShooterGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnlineMode(EOnlineMode::Online) // Default to online
	, bIsLicensed(true) // Default to licensed (should have been checked by OS on boot)
	, SaveSlot(TEXT("SaveGame"))
	, SaveUserIndex(0)
{
	UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::Construct"));

	CurrentState = ShooterGameInstanceState::None;
}

void UShooterGameInstance::Init() 
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::Init"));
	Super::Init();

	IgnorePairingChangeForControllerId = -1;
	CurrentConnectionStatus = EOnlineServerConnectionStatus::Connected;

	LocalPlayerOnlineStatus.InsertDefaulted(0, MAX_LOCAL_PLAYERS);

	// game requires the ability to ID users.
	const auto OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);
	const auto IdentityInterface = OnlineSub->GetIdentityInterface();
	check(IdentityInterface.IsValid());

 	const auto SessionInterface = OnlineSub->GetSessionInterface();
 	check(SessionInterface.IsValid());

	// bind any OSS delegates we needs to handle
	for (int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		IdentityInterface->AddOnLoginStatusChangedDelegate_Handle(i, FOnLoginStatusChangedDelegate::CreateUObject(this, &UShooterGameInstance::HandleUserLoginChanged));
	}

	IdentityInterface->AddOnControllerPairingChangedDelegate_Handle(FOnControllerPairingChangedDelegate::CreateUObject(this, &UShooterGameInstance::HandleControllerPairingChanged));

	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UShooterGameInstance::HandleAppWillDeactivate);

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UShooterGameInstance::HandleAppSuspend);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UShooterGameInstance::HandleAppResume);

	FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &UShooterGameInstance::HandleSafeFrameChanged);
	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UShooterGameInstance::HandleControllerConnectionChange);
	FCoreDelegates::ApplicationLicenseChange.AddUObject(this, &UShooterGameInstance::HandleAppLicenseUpdate);

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UShooterGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UShooterGameInstance::OnPostLoadMap);

	FCoreUObjectDelegates::PostDemoPlay.AddUObject(this, &UShooterGameInstance::OnPostDemoPlay);

	bPendingEnableSplitscreen = false;

	OnlineSub->AddOnConnectionStatusChangedDelegate_Handle( FOnConnectionStatusChangedDelegate::CreateUObject( this, &UShooterGameInstance::HandleNetworkConnectionStatusChanged ) );

	if (SessionInterface.IsValid())
	{
		SessionInterface->AddOnSessionFailureDelegate_Handle( FOnSessionFailureDelegate::CreateUObject( this, &UShooterGameInstance::HandleSessionFailure ) );
	}
	
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnEndSessionComplete);

	// Register delegate for ticker callback
	TickDelegate = FTickerDelegate::CreateUObject(this, &UShooterGameInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	// Initialize the debug key with a set value for AES256. This is not secure and for example purposes only.
	DebugTestEncryptionKey.SetNum(32);

	for (int32 i = 0; i < DebugTestEncryptionKey.Num(); ++i)
	{
		DebugTestEncryptionKey[i] = uint8(i);
	}

	//if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	//{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance on DedicatedServer"));
		InitializeDataSource();
		SetSavingEnabled(true);
		if (LoadOrCreateSaveGame() == true)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Loaded Save Game."));
		}
		else
		{
			WriteSaveGame();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("New Save Game."));
		}
	//}
	/*else if (GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance on Client"));
	}
	else if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance on Standalone"));
	}*/
}

/**
*	Assets Data Initialze
*/
void UShooterGameInstance::InitializeDataSource()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::InitializeDataSource()"));
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("InitializeDataSource()")));
	//ItemSlotsPerType.Add(FPrimaryAssetType(TEXT("Token")), 1);
	ItemSlotsPerType.Add(UShooterAssetManager::CategoryItemType, 0);
	ItemSlotsPerType.Add(UShooterAssetManager::SkillItemType, 1);
	ItemSlotsPerType.Add(UShooterAssetManager::PotionItemType, 2);
	ItemSlotsPerType.Add(UShooterAssetManager::WeaponItemType, 3);
	ItemSlotsPerType.Add(UShooterAssetManager::PawnItemType, 4);
	ItemSlotsPerType.Add(UShooterAssetManager::ArmorItemType, 5);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("ItemSlotsPerType::%d"), ItemSlotsPerType.Num()));

	// TMap<FPrimaryAssetId, FShooterItemData> DataSource;
	// TMap<FPrimaryAssetType, int32> ItemSlotsPerType;
	// TArray<class UShooterItem*> AssetSourceItems;
	AssetSources.Reset();
	FShooterGameAsset GameAsset;
	TArray<UObject*> LoadedAssets;
	TArray<class UShooterItem*> AssetItems;
	TArray<FPrimaryAssetId> PrimaryAssetIdList;

	for (const TPair<FPrimaryAssetType, int32>& TypePair : ItemSlotsPerType)
	{
		AssetItems.Reset();
		LoadedAssets.Reset();
		PrimaryAssetIdList.Reset();
		//EGameKey::Type* const k = &It.Key();
		//FSimpleKeyState* const v = &It.Value();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("FPrimaryAssetType::s%"), (TypePair.Key).ToString());
		UAssetManager::Get().GetPrimaryAssetIdList(TypePair.Key, PrimaryAssetIdList);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("FPrimaryAssetType::%d"), PrimaryAssetIdList.Num()));

		GameAsset.AssetCategory = PrimaryAssetIdList[0];
		TSharedPtr<FStreamableHandle> Handle = UAssetManager::Get().LoadPrimaryAssets(PrimaryAssetIdList);

		if (Handle && Handle.IsValid())
		{
			Handle->WaitUntilComplete();
			Handle->GetLoadedAssets(LoadedAssets);
		}
		for (UObject* Obj : LoadedAssets)
		{
			if (Obj)
			{
				AssetSourceItems.AddUnique(Cast<UShooterItem>(Obj));
				AssetItems.AddUnique(Cast<UShooterItem>(Obj));
			}
		}
		GameAsset.AssetItems = AssetItems;
		AssetSources.AddUnique(GameAsset);
		//AssetSourceMap.Emplace(TypePair.Key, AssetSourceItems);
	}
	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("AssetSourceItems::%d"), AssetSourceItems.Num()));
	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("AssetSources::%d"), AssetSources.Num()));
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::InitializeDataSource() -- AssetSources = %d"), AssetSources.Num());
}

TArray<class UShooterItem*> UShooterGameInstance::GetAssetSourceItems(FPrimaryAssetType AssetType) const
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetAssetSourceItems()"));
	TArray<class UShooterItem*> FilterItem;
	//if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	//{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetAssetSourceItems() -- on DedicatedServer"));
		if (AssetSourceItems.Num() > 0)
		{
			for (class UShooterItem* Item : AssetSourceItems)
			{
				FPrimaryAssetId Asset = Item->GetPrimaryAssetId();
				if (Item->ItemType == AssetType)
				{
					FilterItem.Add(Item);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetAssetSourceItems() -- FilterItemCount = %d"), FilterItem.Num());
	//}
	return FilterItem;
}

void UShooterGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Shutdown()"));
	Super::Shutdown();

	// Unregister ticker delegate
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UShooterGameInstance::HandleNetworkConnectionStatusChanged( const FString& ServiceName, EOnlineServerConnectionStatus::Type LastConnectionStatus, EOnlineServerConnectionStatus::Type ConnectionStatus )
{
	UE_LOG( LogOnlineGame, Log, TEXT( "UShooterGameInstance::HandleNetworkConnectionStatusChanged: %s" ), EOnlineServerConnectionStatus::ToString( ConnectionStatus ) );

#if SHOOTER_CONSOLE_UI
	// If we are disconnected from server, and not currently at (or heading to) the welcome screen
	// then display a message on consoles
	if (	OnlineMode != EOnlineMode::Offline && 
			PendingState != ShooterGameInstanceState::WelcomeScreen &&
			CurrentState != ShooterGameInstanceState::WelcomeScreen && 
			ConnectionStatus != EOnlineServerConnectionStatus::Connected )
	{
		UE_LOG( LogOnlineGame, Log, TEXT( "UShooterGameInstance::HandleNetworkConnectionStatusChanged: Going to main menu" ) );

		// Display message on consoles
#if PLATFORM_XBOXONE
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost." );
#elif PLATFORM_PS4
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection to \"PSN\" has been lost." );
#else
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection has been lost." );
#endif
		const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );
		
		ShowMessageThenGotoState( ReturnReason, OKButton, FText::GetEmpty(), ShooterGameInstanceState::MainMenu);
	}

	CurrentConnectionStatus = ConnectionStatus;
#endif
}

void UShooterGameInstance::HandleSessionFailure( const FUniqueNetId& NetId, ESessionFailure::Type FailureType )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleSessionFailure()"));
	UE_LOG( LogOnlineGame, Warning, TEXT( "UShooterGameInstance::HandleSessionFailure: %u" ), (uint32)FailureType );

#if SHOOTER_CONSOLE_UI
	// If we are not currently at (or heading to) the welcome screen then display a message on consoles
	if (	OnlineMode != EOnlineMode::Offline &&
			PendingState != ShooterGameInstanceState::WelcomeScreen &&
			CurrentState != ShooterGameInstanceState::WelcomeScreen )
	{
		UE_LOG( LogOnlineGame, Log, TEXT( "UShooterGameInstance::HandleSessionFailure: Going to main menu" ) );

		// Display message on consoles
#if PLATFORM_XBOXONE
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection to Xbox LIVE has been lost." );
#elif PLATFORM_PS4
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection to PSN has been lost." );
#else
		const FText ReturnReason	= NSLOCTEXT( "NetworkFailures", "ServiceUnavailable", "Connection has been lost." );
#endif
		const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );
		
		ShowMessageThenGotoState( ReturnReason, OKButton,  FText::GetEmpty(), ShooterGameInstanceState::MainMenu);
	}
#endif
}

void UShooterGameInstance::OnPreLoadMap(const FString& MapName)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPreLoadMap()"));
	UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::OnPreLoadMap"));
	if (bPendingEnableSplitscreen)
	{
		// Allow splitscreen
		UGameViewportClient* GameViewportClient = GetGameViewportClient();
		if (GameViewportClient != nullptr)
		{
			GameViewportClient->SetDisableSplitscreenOverride(false);

			bPendingEnableSplitscreen = false;
		}
	}
}

void UShooterGameInstance::OnPostLoadMap(UWorld*)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPostLoadMap()"));
	UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::OnPostLoadMap"));
	// Make sure we hide the loading screen when the level is done loading
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());
	if (ShooterViewport != nullptr)
	{
		ShooterViewport->HideLoadingScreen();
	}
}

void UShooterGameInstance::OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnUserCanPlayInvite()"));
	UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::OnUserCanPlayInvite"));
	CleanupOnlinePrivilegeTask();
	if (WelcomeMenuUI.IsValid())
	{
		WelcomeMenuUI->LockControls(false);
	}

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)	
	{
		if (UserId == *PendingInvite.UserId)
		{
			PendingInvite.bPrivilegesCheckedAndAllowed = true;
		}		
	}
	else
	{
		DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
		GotoState(ShooterGameInstanceState::WelcomeScreen);
	}
}

void UShooterGameInstance::OnUserCanPlayTogether(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnUserCanPlayTogether()"));
	CleanupOnlinePrivilegeTask();
	if (WelcomeMenuUI.IsValid())
	{
		WelcomeMenuUI->LockControls(false);
	}

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		if (WelcomeMenuUI.IsValid())
		{
			WelcomeMenuUI->SetControllerAndAdvanceToMainMenu(PlayTogetherInfo.UserIndex);
		}
	}
	else
	{
		DisplayOnlinePrivilegeFailureDialogs(UserId, Privilege, PrivilegeResults);
		GotoState(ShooterGameInstanceState::WelcomeScreen);
	}
}

void UShooterGameInstance::OnPostDemoPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPostDemoPlay()"));
	GotoState( ShooterGameInstanceState::Playing );
}

void UShooterGameInstance::HandleDemoPlaybackFailure( EDemoPlayFailure::Type FailureType, const FString& ErrorString )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleDemoPlaybackFailure()"));
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::PIE)
	{
		UE_LOG(LogEngine, Warning, TEXT("Demo failed to play back correctly, got error %s"), *ErrorString);
		return;
	}

	ShowMessageThenGotoState(FText::Format(NSLOCTEXT("UShooterGameInstance", "DemoPlaybackFailedFmt", "Demo playback failed: {0}"), FText::FromString(ErrorString)), NSLOCTEXT("DialogButtons", "OKAY", "OK"), FText::GetEmpty(), ShooterGameInstanceState::MainMenu);
}

void UShooterGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::StartGameInstance()"));
	//UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::StartGameInstance"));
#if PLATFORM_PS4 == 0
	TCHAR Parm[4096] = TEXT("");

	const TCHAR* Cmd = FCommandLine::Get();

	// Catch the case where we want to override the map name on startup (used for connecting to other MP instances)
	if (FParse::Token(Cmd, Parm, ARRAY_COUNT(Parm), 0) && Parm[0] != '-')
	{
		// if we're 'overriding' with the default map anyway, don't set a bogus 'playing' state.
		if (!MainMenuMap.Contains(Parm))
		{
			FURL DefaultURL;
			DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);

			FURL URL(&DefaultURL, Parm, TRAVEL_Partial);

			if (URL.Valid)
			{
				UEngine* const Engine = GetEngine();

				FString Error;

				UE_LOG(LogTemp, Warning, TEXT("StartGameInstance(Browse: %s DefaultURL: %s)"), *URL.ToString(), *DefaultURL.ToString());
				const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);

				if (BrowseRet == EBrowseReturnVal::Success)
				{
					UE_LOG(LogTemp, Warning, TEXT("StartGameInstance:Go to Playing. %s"), *FString::Printf(TEXT("CurrentState = %s"), *CurrentState.ToString()));
					// Success, we loaded the map, go directly to playing state
					GotoState(ShooterGameInstanceState::Playing);
					return;
				}
				else if (BrowseRet == EBrowseReturnVal::Pending)
				{
					UE_LOG(LogTemp, Warning, TEXT("StartGameInstance:Go to MainMenu."));
					// Assume network connection
					LoadFrontEndMap(MainMenuMap);
					//LoadFrontEndMap(ConnectServerMap);
					AddNetworkFailureHandlers();
					ShowLoadingScreen();
					GotoState(ShooterGameInstanceState::MainMenu);
					return;
				}/*
				else 
				{
					UE_LOG(LogTemp, Warning, TEXT("StartGameInstance:Go to MainMenuMap."));
					// Fail, 
					LoadFrontEndMap(MainMenuMap);
					//LoadFrontEndMap(ConnectServerMap);
					AddNetworkFailureHandlers();
					ShowLoadingScreen();
					GotoState(ShooterGameInstanceState::MainMenu);
					return;
				}*/
			}
		}
	}
#endif

	GotoInitialState();
}

FName UShooterGameInstance::GetInitialState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetInitialState()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::GetInitialState"));
#if SHOOTER_CONSOLE_UI	
	// Start in the welcome screen state on consoles
	return ShooterGameInstanceState::WelcomeScreen;
#else
	// On PC, go directly to the main menu
	return ShooterGameInstanceState::MainMenu;
#endif
}

void UShooterGameInstance::GotoInitialState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GotoInitialState()"));
	GotoState(GetInitialState());
}

void UShooterGameInstance::ShowMessageThenGotoState( const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting, TWeakObjectPtr< ULocalPlayer > PlayerOwner )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ShowMessageThenGotoState()"));
	UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Message: %s, NewState: %s" ), *Message.ToString(), *NewState.ToString() );

	const bool bAtWelcomeScreen = PendingState == ShooterGameInstanceState::WelcomeScreen || CurrentState == ShooterGameInstanceState::WelcomeScreen;

	// Never override the welcome screen
	if ( bAtWelcomeScreen )
	{
		UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Ignoring due to higher message priority in queue (at welcome screen)." ) );
		return;
	}

	const bool bAlreadyAtMessageMenu = PendingState == ShooterGameInstanceState::MessageMenu || CurrentState == ShooterGameInstanceState::MessageMenu;
	const bool bAlreadyAtDestState = PendingState == NewState || CurrentState == NewState;

	// If we are already going to the message menu, don't override unless asked to
	if ( bAlreadyAtMessageMenu && PendingMessage.NextState == NewState && !OverrideExisting )
	{
		UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 1)." ) );
		return;
	}

	// If we are already going to the message menu, and the next dest is welcome screen, don't override
	if ( bAlreadyAtMessageMenu && PendingMessage.NextState == ShooterGameInstanceState::WelcomeScreen )
	{
		UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 2)." ) );
		return;
	}

	// If we are already at the dest state, don't override unless asked
	if ( bAlreadyAtDestState && !OverrideExisting )
	{
		UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Ignoring due to higher message priority in queue (check 3)" ) );
		return;
	}

	PendingMessage.DisplayString		= Message;
	PendingMessage.OKButtonString		= OKButtonString;
	PendingMessage.CancelButtonString	= CancelButtonString;
	PendingMessage.NextState			= NewState;
	PendingMessage.PlayerOwner			= PlayerOwner;

	if ( CurrentState == ShooterGameInstanceState::MessageMenu )
	{
		UE_LOG( LogOnline, Log, TEXT( "ShowMessageThenGotoState: Forcing new message" ) );
		EndMessageMenuState();
		BeginMessageMenuState();
	}
	else
	{
		GotoState(ShooterGameInstanceState::MessageMenu);
	}
}

void UShooterGameInstance::ShowLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ShowLoadingScreen()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::ShowLoadingScreen"));
	// This can be confusing, so here is what is happening:
	//	For LoadMap, we use the IShooterGameLoadingScreenModule interface to show the load screen
	//  This is necessary since this is a blocking call, and our viewport loading screen won't get updated.
	//  We can't use IShooterGameLoadingScreenModule for seamless travel though
	//  In this case, we just add a widget to the viewport, and have it update on the main thread
	//  To simplify things, we just do both, and you can't tell, one will cover the other if they both show at the same time
	IShooterGameLoadingScreenModule* const LoadingScreenModule = FModuleManager::LoadModulePtr<IShooterGameLoadingScreenModule>("ShooterGameLoadingScreen");
	if (LoadingScreenModule != nullptr)
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}

	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if ( ShooterViewport != NULL )
	{
		ShooterViewport->ShowLoadingScreen();
	}
}

bool UShooterGameInstance::LoadFrontEndMap(const FString& MapName)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadFrontEndMap()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::LoadFrontEndMap(Map:%s)"), *MapName);
	bool bSuccess = true;

	// if already loaded, do nothing
	UWorld* const World = GetWorld();
	if (World)
	{
		FString const CurrentMapName = *World->PersistentLevel->GetOutermost()->GetName();
		//if (MapName.Find(TEXT("Highrise")) != -1)
		if (CurrentMapName == MapName)
		{
			return bSuccess;
		}
	}

	FString Error;
	EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
	FURL URL(*FString::Printf(TEXT("%s"), *MapName));

	if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject)) //CastChecked<UEngine>() will fail if using Default__ShooterGameInstance, so make sure that we're not default
	{
		BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);

		// Handle failure.
		if (BrowseRet != EBrowseReturnVal::Success)
		{
			UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *MapName, *Error));
			bSuccess = false;
		}
	}
	return bSuccess;
}

AShooterGameSession* UShooterGameInstance::GetGameSession() const
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetGameSession()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::GetGameSession"));
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game)
		{
			UE_LOG(LogTemp, Log, TEXT("AuthGameMode"));
			return Cast<AShooterGameSession>(Game->GameSession);
		}
		UE_LOG(LogTemp, Log, TEXT("AuthGameMode is Invalide."));
	}

	return nullptr;
}

void UShooterGameInstance::TravelLocalSessionFailure(UWorld *World, ETravelFailure::Type FailureType, const FString& ReasonString)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLocalSessionFailure()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::TravelLocalSessionFailure"));
	AShooterPlayerController_Menu* const FirstPC = Cast<AShooterPlayerController_Menu>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (FirstPC != nullptr)
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Join Session failed.");
		if (ReasonString.IsEmpty() == false)
		{
			ReturnReason = FText::Format(NSLOCTEXT("NetworkErrors", "JoinSessionFailedReasonFmt", "Join Session failed. {0}"), FText::FromString(ReasonString));
		}

		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
	}
}

void UShooterGameInstance::ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ShowMessageThenGoMain()"));
	ShowMessageThenGotoState(Message, OKButtonString, CancelButtonString, ShooterGameInstanceState::MainMenu);
}

void UShooterGameInstance::SetPendingInvite(const FShooterPendingInvite& InPendingInvite)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SetPendingInvite()"));
	PendingInvite = InPendingInvite;
}

void UShooterGameInstance::GotoState(FName NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GotoState()"));
	UE_LOG( LogOnline, Log, TEXT( "GotoState: NewState: %s" ), *NewState.ToString() );

	PendingState = NewState;
}

void UShooterGameInstance::MaybeChangeState()
{
	//UE_LOG(LogOnline, Log, TEXT("UShooterGameInstance::MaybeChangeState"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("UShooterGameInstance::CurrentState( %s )"), *CurrentState.ToString()));

	if ( (PendingState != CurrentState) && (PendingState != ShooterGameInstanceState::None) )
	{
		FName const OldState = CurrentState;

		// end current state
		EndCurrentState(PendingState);

		// begin new state
		BeginNewState(PendingState, OldState);

		// clear pending change
		PendingState = ShooterGameInstanceState::None;
	}
}

void UShooterGameInstance::EndCurrentState(FName NextState)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("GameInstance::EndCurrentState( %s )"), *NextState.ToString()));

	// per-state custom ending code here
	if (CurrentState == ShooterGameInstanceState::PendingInvite)
	{
		EndPendingInviteState();
	}
	else if (CurrentState == ShooterGameInstanceState::WelcomeScreen)
	{
		EndWelcomeScreenState();
	}
	else if (CurrentState == ShooterGameInstanceState::ConnectingServer)
	{
		EndConnectServerState();
	}
	else if (CurrentState == ShooterGameInstanceState::PawnGuide)
	{
		EndPawnGuideState();
	}
	else if (CurrentState == ShooterGameInstanceState::MainMenu)
	{
		EndMainMenuState();
	}
	else if (CurrentState == ShooterGameInstanceState::MessageMenu)
	{
		EndMessageMenuState();
	}
	else if (CurrentState == ShooterGameInstanceState::Playing)
	{
		EndPlayingState();
	}

	CurrentState = ShooterGameInstanceState::None;
}

void UShooterGameInstance::BeginNewState(FName NewState, FName PrevState)
{
	//UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginNewState()"));
	//UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *MapName, *Error));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("GameInstance::BeginNewState( CurrentState:%s  NewState:%s )"), *CurrentState.ToString(), *NewState.ToString()));
	// per-state custom starting code here

	if (NewState == ShooterGameInstanceState::PendingInvite)
	{
		BeginPendingInviteState();
	}
	else if (NewState == ShooterGameInstanceState::WelcomeScreen)
	{
		BeginWelcomeScreenState();
	}
	else if (NewState == ShooterGameInstanceState::MainMenu)
	{
		BeginMainMenuState();
	}
	else if (NewState == ShooterGameInstanceState::ConnectingServer)
	{
		BeginConnectServerState();
	}
	else if (NewState == ShooterGameInstanceState::PawnGuide)
	{
		BeginPawnGuideState();
	}
	else if (NewState == ShooterGameInstanceState::MessageMenu)
	{
		BeginMessageMenuState();
	}
	else if (NewState == ShooterGameInstanceState::Playing)
	{
		BeginPlayingState();
	}

	CurrentState = NewState;
}

void UShooterGameInstance::BeginPendingInviteState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginPendingInviteState()"));
	if (LoadFrontEndMap(MainMenuMap))
	{				
		StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
	}
	else
	{
		GotoState(ShooterGameInstanceState::WelcomeScreen);
	}
}

void UShooterGameInstance::EndPendingInviteState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndPendingInviteState()"));
	// cleanup in case the state changed before the pending invite was handled.
	CleanupOnlinePrivilegeTask();
}

void UShooterGameInstance::BeginWelcomeScreenState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginWelcomeScreenState()"));
	//this must come before split screen player removal so that the OSS sets all players to not using online features.
	SetOnlineMode(EOnlineMode::Offline);

	// Remove any possible splitscren players
	RemoveSplitScreenPlayers();

	LoadFrontEndMap(WelcomeScreenMap);

	ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();
	LocalPlayer->SetCachedUniqueNetId(nullptr);
	check(!WelcomeMenuUI.IsValid());
	WelcomeMenuUI = MakeShareable(new FShooterWelcomeMenu);
	WelcomeMenuUI->Construct( this );
	WelcomeMenuUI->AddToGameViewport();

	// Disallow splitscreen (we will allow while in the playing state)
	GetGameViewportClient()->SetDisableSplitscreenOverride( true );
}

void UShooterGameInstance::EndWelcomeScreenState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndWelcomeScreenState()"));
	if (WelcomeMenuUI.IsValid())
	{
		WelcomeMenuUI->RemoveFromGameViewport();
		WelcomeMenuUI = nullptr;
	}
}

void UShooterGameInstance::SetPresenceForLocalPlayers(const FString& StatusStr, const FVariantData& PresenceData)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SetPresenceForLocalPlayers()"));
	const auto Presence = Online::GetPresenceInterface();
	if (Presence.IsValid())
	{
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			FUniqueNetIdRepl UserId = LocalPlayers[i]->GetPreferredUniqueNetId();

			if (UserId.IsValid())
			{
				FOnlineUserPresenceStatus PresenceStatus;
				PresenceStatus.StatusStr = StatusStr;
				PresenceStatus.Properties.Add(DefaultPresenceKey, PresenceData);

				Presence->SetPresence(*UserId, PresenceStatus);
			}
		}
	}
}

void UShooterGameInstance::BeginMainMenuState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginMainMenuState()"));
	// Make sure we're not showing the loadscreen
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if ( ShooterViewport != NULL )
	{
		ShooterViewport->HideLoadingScreen();
	}

	SetOnlineMode(EOnlineMode::Offline);

	// Disallow splitscreen
	UGameViewportClient* GameViewportClient = GetGameViewportClient();
	
	if (GameViewportClient)
	{
		GetGameViewportClient()->SetDisableSplitscreenOverride(true);
	}

	// Remove any possible splitscren players
	RemoveSplitScreenPlayers();

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FString(TEXT("In Menu")), FVariantData(FString(TEXT("OnMenu"))));

	// load startup map
	LoadFrontEndMap(MainMenuMap);

	// player 0 gets to own the UI
	ULocalPlayer* const Player = GetFirstGamePlayer();

	MainMenuUI = MakeShareable(new FShooterMainMenu());
	MainMenuUI->Construct(this, Player);
	MainMenuUI->AddMenuToGameViewport();

	// It's possible that a play together event was sent by the system while the player was in-game or didn't
	// have the application launched. The game will automatically go directly to the main menu state in those cases
	// so this will handle Play Together if that is why we transitioned here.
	if (PlayTogetherInfo.UserIndex != -1)
	{
		MainMenuUI->OnPlayTogetherEventReceived();
	}

#if !SHOOTER_CONSOLE_UI
	// The cached unique net ID is usually set on the welcome screen, but there isn't
	// one on PC/Mac, so do it here.
	if (Player != nullptr)
	{
		Player->SetControllerId(0);
		Player->SetCachedUniqueNetId(Player->GetUniqueNetIdFromCachedControllerId().GetUniqueNetId());
	}
#endif

	RemoveNetworkFailureHandlers();
}

void UShooterGameInstance::EndMainMenuState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndMainMenuState()"));
	if (MainMenuUI.IsValid())
	{
		MainMenuUI->RemoveMenuFromGameViewport();
		MainMenuUI = nullptr;
	}
}

void UShooterGameInstance::BeginConnectServerState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginConnectServerState()"));
	// Make sure we're not showing the loadscreen
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if (ShooterViewport != NULL)
	{
		ShooterViewport->HideLoadingScreen();
	}

	SetOnlineMode(EOnlineMode::Offline);

	// Disallow splitscreen
	UGameViewportClient* GameViewportClient = GetGameViewportClient();

	if (GameViewportClient)
	{
		GetGameViewportClient()->SetDisableSplitscreenOverride(true);
	}

	// Remove any possible splitscren players
	RemoveSplitScreenPlayers();

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FString(TEXT("In Menu")), FVariantData(FString(TEXT("OnMenu"))));

	// load startup map
	LoadFrontEndMap(ConnectServerMap);

	// player 0 gets to own the UI
	ULocalPlayer* const Player = GetFirstGamePlayer();

	UWorld* const World = GetWorld();

	AShooterGameState* const GameState = World != NULL ? World->GetGameState<AShooterGameState>() : NULL;

	//if (GameState)
	//{
	//	GameState->SetMatchState(MatchState::Aborted);
	//}

	//LoginUI = MakeShareable(new FShooterLogin());
	//LoginUI->Construct(this, Player);
	//LoginUI->AddMenuToGameViewport();

	// It's possible that a play together event was sent by the system while the player was in-game or didn't
	// have the application launched. The game will automatically go directly to the main menu state in those cases
	// so this will handle Play Together if that is why we transitioned here.
	//if (PlayTogetherInfo.UserIndex != -1)
	//{
		//MainMenuUI->OnPlayTogetherEventReceived();
	//}

#if !SHOOTER_CONSOLE_UI
	// The cached unique net ID is usually set on the welcome screen, but there isn't
	// one on PC/Mac, so do it here.
	if (Player != nullptr)
	{
		Player->SetControllerId(0);
		Player->SetCachedUniqueNetId(Player->GetUniqueNetIdFromCachedControllerId().GetUniqueNetId());
	}
#endif

	RemoveNetworkFailureHandlers();
}

void UShooterGameInstance::EndConnectServerState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndConnectServerState()"));
	if (LoginUI.IsValid())
	{
		LoginUI->RemoveMenuFromGameViewport();
		LoginUI = nullptr;
	}
}

void UShooterGameInstance::BeginPawnGuideState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginPawnGuideState()"));
	// Make sure we're not showing the loadscreen
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if (ShooterViewport != NULL)
	{
		ShooterViewport->HideLoadingScreen();
	}

	SetOnlineMode(EOnlineMode::Online);

	// Disallow splitscreen
	UGameViewportClient* GameViewportClient = GetGameViewportClient();

	if (GameViewportClient)
	{
		GetGameViewportClient()->SetDisableSplitscreenOverride(true);
	}

	// Remove any possible splitscren players
	RemoveSplitScreenPlayers();

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FString(TEXT("In Menu")), FVariantData(FString(TEXT("OnMenu"))));

	// load startup map
	LoadFrontEndMap(PawnGuideMap);

	// player 0 gets to own the UI
	ULocalPlayer* const Player = GetFirstGamePlayer();

	//PawnGuideWidgetIns = CreateWidget<UUserWidget>(Player->GetPlayerController(UWorld), PawnGuideWidgetClass);

	//check(PawnGuideWidgetIns);

	//GEngine->GameViewport->AddViewportWidgetContent(PawnGuideWidgetIns->TakeWidget(), 10);

	//PawnGuideUI = MakeShareable(new FShooterPawnGuide());
	//PawnGuideUI->Construct(this, Player);
	//PawnGuideUI->AddMenuToGameViewport();

	// It's possible that a play together event was sent by the system while the player was in-game or didn't
	// have the application launched. The game will automatically go directly to the main menu state in those cases
	// so this will handle Play Together if that is why we transitioned here.
	//if (PlayTogetherInfo.UserIndex != -1)
	//{
	//	PawnGuideUI->OnPlayTogetherEventReceived();
	//}

#if !SHOOTER_CONSOLE_UI
	// The cached unique net ID is usually set on the welcome screen, but there isn't
	// one on PC/Mac, so do it here.
	if (Player != nullptr)
	{
		Player->SetControllerId(0);
		Player->SetCachedUniqueNetId(Player->GetUniqueNetIdFromCachedControllerId().GetUniqueNetId());
	}
#endif

	RemoveNetworkFailureHandlers();
}

void UShooterGameInstance::EndPawnGuideState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndPawnGuideState()"));
	if (PawnGuideUI.IsValid())
	{
		PawnGuideUI->RemoveMenuFromGameViewport();
		PawnGuideUI = nullptr;
	}
}

void UShooterGameInstance::BeginMessageMenuState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginMessageMenuState()"));
	if (PendingMessage.DisplayString.IsEmpty())
	{
		UE_LOG(LogOnlineGame, Warning, TEXT("GameInstance::BeginMessageMenuState: Display string is empty"));
		GotoInitialState();
		return;
	}

	// Make sure we're not showing the loadscreen
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if ( ShooterViewport != NULL )
	{
		ShooterViewport->HideLoadingScreen();
	}

	check(!MessageMenuUI.IsValid());
	MessageMenuUI = MakeShareable(new FShooterMessageMenu);
	MessageMenuUI->Construct(this, PendingMessage.PlayerOwner, PendingMessage.DisplayString, PendingMessage.OKButtonString, PendingMessage.CancelButtonString, PendingMessage.NextState);

	PendingMessage.DisplayString = FText::GetEmpty();
}

void UShooterGameInstance::EndMessageMenuState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndMessageMenuState()"));
	if (MessageMenuUI.IsValid())
	{
		MessageMenuUI->RemoveFromGameViewport();
		MessageMenuUI = nullptr;
	}
}

void UShooterGameInstance::BeginPlayingState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginPlayingState()"));
	bPendingEnableSplitscreen = true;

	// Set presence for playing in a map
	SetPresenceForLocalPlayers(FString(TEXT("In Game")), FVariantData(FString(TEXT("InGame"))));

	UWorld* const World = GetWorld();
	AShooterGameState* const GameState = World != NULL ? World->GetGameState<AShooterGameState>() : NULL;

	if (GameState)
	{
		GameState->SetMatchState(MatchState::WaitingToStart);
	}
	// Make sure viewport has focus
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void UShooterGameInstance::EndPlayingState()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::EndPlayingState()"));
	// Disallow splitscreen
	GetGameViewportClient()->SetDisableSplitscreenOverride( true );

	// Clear the players' presence information
	SetPresenceForLocalPlayers(FString(TEXT("In Menu")), FVariantData(FString(TEXT("OnMenu"))));

	UWorld* const World = GetWorld();
	AShooterGameState* const GameState = World != NULL ? World->GetGameState<AShooterGameState>() : NULL;

	if (GameState)
	{
		// Send round end events for local players
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			auto ShooterPC = Cast<AShooterPlayerController>(LocalPlayers[i]->PlayerController);
			if (ShooterPC)
			{
				// Assuming you can't win if you quit early
				ShooterPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
			}
		}

		// Give the game state a chance to cleanup first
		GameState->RequestFinishAndExitToMainMenu();
	}
	else
	{
		// If there is no game state, make sure the session is in a good state
		CleanupSessionOnReturnToMenu();
	}
}

void UShooterGameInstance::OnEndSessionComplete( FName SessionName, bool bWasSuccessful )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnEndSessionComplete()"));
	UE_LOG(LogOnline, Log, TEXT("GameInstance::OnEndSessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false") );

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle  (OnStartSessionCompleteDelegateHandle);
			Sessions->ClearOnEndSessionCompleteDelegate_Handle    (OnEndSessionCompleteDelegateHandle);
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}

	// continue
	CleanupSessionOnReturnToMenu();
}

void UShooterGameInstance::CleanupSessionOnReturnToMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::CleanupSessionOnReturnToMenu()"));
	bool bPendingOnlineOp = false;

	// end online game and then destroy it
	IOnlineSubsystem * OnlineSub = IOnlineSubsystem::Get();
	IOnlineSessionPtr Sessions = ( OnlineSub != NULL ) ? OnlineSub->GetSessionInterface() : NULL;

	if ( Sessions.IsValid() )
	{
		FName GameSession(NAME_GameSession);
		EOnlineSessionState::Type SessionState = Sessions->GetSessionState(NAME_GameSession);
		UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *GameSession.ToString(), EOnlineSessionState::ToString(SessionState));

		if ( EOnlineSessionState::InProgress == SessionState )
		{
			UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *GameSession.ToString() );
			OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			Sessions->EndSession(NAME_GameSession);
			bPendingOnlineOp = true;
		}
		else if ( EOnlineSessionState::Ending == SessionState )
		{
			UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *GameSession.ToString() );
			OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			bPendingOnlineOp = true;
		}
		else if ( EOnlineSessionState::Ended == SessionState || EOnlineSessionState::Pending == SessionState )
		{
			UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *GameSession.ToString() );
			OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			Sessions->DestroySession(NAME_GameSession);
			bPendingOnlineOp = true;
		}
		else if ( EOnlineSessionState::Starting == SessionState || EOnlineSessionState::Creating == SessionState)
		{
			UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *GameSession.ToString() );
			OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			bPendingOnlineOp = true;
		}
	}

	if ( !bPendingOnlineOp )
	{
		//GEngine->HandleDisconnect( GetWorld(), GetWorld()->GetNetDriver() );
	}
}

void UShooterGameInstance::LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::LabelPlayerAsQuitter()"));
	AShooterPlayerState* const PlayerState = LocalPlayer && LocalPlayer->PlayerController ? Cast<AShooterPlayerState>(LocalPlayer->PlayerController->PlayerState) : nullptr;	
	if(PlayerState)
	{
		PlayerState->SetQuitter(true);
	}
}

void UShooterGameInstance::RemoveNetworkFailureHandlers()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::RemoveNetworkFailureHandlers()"));
	// Remove the local session/travel failure bindings if they exist
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == true)
	{
		GEngine->OnTravelFailure().Remove(TravelLocalSessionFailureDelegateHandle);
	}
}

void UShooterGameInstance::AddNetworkFailureHandlers()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::AddNetworkFailureHandlers()"));
	// Add network/travel error handlers (if they are not already there)
	if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
	{
		TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UShooterGameInstance::TravelLocalSessionFailure);
	}
}

TSubclassOf<UOnlineSession> UShooterGameInstance::GetOnlineSessionClass()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetOnlineSessionClass()"));
	return UShooterOnlineSessionClient::StaticClass();
}

bool UShooterGameInstance::HostQuickSession(ULocalPlayer& LocalPlayer, const FOnlineSessionSettings& SessionSettings)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HostQuickSession()"));
	// This function is different from BeginHostingQuickMatch in that it creates a session and then starts a quick match,
	// while BeginHostingQuickMatch assumes a session already exists

	if (AShooterGameSession* const GameSession = GetGameSession())
	{
		// Add callback delegate for completion
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UShooterGameInstance::OnCreatePresenceSessionComplete);

		TravelURL = GetQuickMatchUrl();

		FOnlineSessionSettings HostSettings = SessionSettings;

		const FString GameType = UGameplayStatics::ParseOption(TravelURL, TEXT("game"));

		// Determine the map name from the travelURL
		const FString MapNameSubStr = "/Game/Maps/";
		const FString ChoppedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString MapName = ChoppedMapName.LeftChop(ChoppedMapName.Len() - ChoppedMapName.Find("?game"));

		HostSettings.Set(SETTING_GAMEMODE, GameType, EOnlineDataAdvertisementType::ViaOnlineService);
		HostSettings.Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
		HostSettings.NumPublicConnections = 16;

		if (GameSession->HostSession(LocalPlayer.GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SessionSettings))
		{
			// If any error occurred in the above, pending state would be set
			if (PendingState == CurrentState || PendingState == ShooterGameInstanceState::None)
			{
				// Go ahead and go into loading state now
				// If we fail, the delegate will handle showing the proper messaging and move to the correct state
				ShowLoadingScreen();
				GotoState(ShooterGameInstanceState::Playing);
				return true;
			}
		}
	}

	return false;
}

/**
*	Connection Server and Create Online Session
*/
bool UShooterGameInstance::ConnectionServer(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
	//UE_LOG(LogTemp, Warning, TEXT("GameInstance::ConnectionServer()"));
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ConnectionServer( %s )"), *FString::Printf(TEXT("URL：%s "), *InTravelURL));

	// start online game locally and wait for completion
	/*IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && (Sessions->GetNamedSession(NAME_GameSession) != nullptr))
		{
			FNamedOnlineSession* Session = Sessions->GetNamedSession(NAME_GameSession);

			if (Session->SessionState == EOnlineSessionState::Pending)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Pending )"));
			}
			else if (Session->SessionState == EOnlineSessionState::Ended)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Ended )"));
			}
			else if (Session->SessionState == EOnlineSessionState::NoSession)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:NoSession )"));
			}
			else if (Session->SessionState == EOnlineSessionState::Creating)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Creating )"));
			}
			else if (Session->SessionState == EOnlineSessionState::Starting)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Starting )"));
			}
			else if (Session->SessionState == EOnlineSessionState::InProgress)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:InProgress )"));
			}
			else if (Session->SessionState == EOnlineSessionState::Ending)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Ending )"));
			}
			else if (Session->SessionState == EOnlineSessionState::Destroying)
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer( SessionState:Destroying )"));
			}

			UE_LOG(LogOnlineGame, Log, TEXT("Starting session %s on server"), *FName(NAME_GameSession).ToString());
			//OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
			//Sessions->StartSession(NAME_GameSession);
		}
	}*/
	if (AShooterGameSession* const GameSession = GetGameSession())
	{
		if (GameSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameSession : Valid"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameSession : InValid"));
		}
		// add callback delegate for completion
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UShooterGameInstance::OnCreatePresenceSessionComplete);

		TravelURL = InTravelURL;
		bool const bIsLanMatch = InTravelURL.Contains(TEXT("?bIsLanMatch"));

		//determine the map name from the travelURL
		const FString& MapNameSubStr = "/Game/Maps/";
		const FString& ChoppedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString& MapName = ChoppedMapName.LeftChop(ChoppedMapName.Len() - ChoppedMapName.Find("?game"));

		if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, GameType, MapName, bIsLanMatch, true, AShooterGameSession::DEFAULT_NUM_PLAYERS))
		{
			// If any error occurred in the above, pending state would be set
			if ((PendingState == CurrentState) || (PendingState == ShooterGameInstanceState::None))
			{
				UE_LOG(LogTemp, Warning, TEXT("ConnectionServer(GotoState(ConnectingServer))"));
				// Go ahead and go into loading state now
				// If we fail, the delegate will handle showing the proper messaging and move to the correct state
				// ShowLoadingScreen();
				GotoState(ShooterGameInstanceState::ConnectingServer);
				return true;
			}
		}
	}
	return false;
}

bool UShooterGameInstance::TravelLevel(const FString& InTravelURL)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLevel()"));

	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLevel(NM_DedicatedServer)"));
	}
	else if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLevel(NM_Standalone)"));
		//SGI->ConnectionServer(SGI->GetFirstGamePlayer(), TEXT("MUIM"), StartURL);
	}
	else if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLevel(NM_ListenServer)"));
	}
	else if (GetWorld()->GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelLevel(NM_Client)"));
	}

	if (GetOnlineMode() == EOnlineMode::Online)
	{
		//
		// Offline game, just go straight to map
		//
		//ShowLoadingScreen();
		GotoState(ShooterGameInstanceState::Playing);

		// Travel to the specified match URL
		TravelURL = InTravelURL;
		GetWorld()->ServerTravel(TravelURL, false, false);
		return true;
	}

	return false;
}

bool UShooterGameInstance::HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
	UE_LOG(LogTemp, Warning, TEXT("GameIns::HostGame( %s )"), *FString::Printf(TEXT("GameType : %s  TravelURL : %s. "), *GameType, *InTravelURL));

	if (GetOnlineMode() == EOnlineMode::Online)
	{
		UE_LOG(LogTemp, Warning, TEXT("Online"));
	}
	else if (GetOnlineMode() == EOnlineMode::LAN)
	{
		UE_LOG(LogTemp, Warning, TEXT("LAN"));
	}

	if (GetOnlineMode() == EOnlineMode::Offline)
	{
		//
		// Offline game, just go straight to map
		//

		//ShowLoadingScreen();
		GotoState(ShooterGameInstanceState::Playing);

		// Travel to the specified match URL
		TravelURL = InTravelURL;
		GetWorld()->ServerTravel(TravelURL);
		return true;
	}

	//
	// Online game
	//

	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::HostGame( OnCreatePresenceSessionComplete )"));

		// add callback delegate for completion
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &UShooterGameInstance::OnCreatePresenceSessionComplete);

		TravelURL = InTravelURL;
		bool const bIsLanMatch = InTravelURL.Contains(TEXT("?bIsLanMatch"));

		//determine the map name from the travelURL
		const FString& MapNameSubStr = "/Game/Maps/";
		const FString& ChoppedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString& MapName = ChoppedMapName.LeftChop(ChoppedMapName.Len() - ChoppedMapName.Find("?game"));

		if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, GameType, MapName, bIsLanMatch, true, AShooterGameSession::DEFAULT_NUM_PLAYERS))
		{
			// If any error occurred in the above, pending state would be set
			if ( (PendingState == CurrentState) || (PendingState == ShooterGameInstanceState::None) )
			{
				UE_LOG(LogTemp, Warning, TEXT("GameInstance::HostGame( Go to Playing )"));
				// Go ahead and go into loading state now
				// If we fail, the delegate will handle showing the proper messaging and move to the correct state
				//ShowLoadingScreen();
				GotoState(ShooterGameInstanceState::Playing);
				return true;
			}
		}
	}

	return false;
}

bool UShooterGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::JoinSession()"));
	// needs to tear anything down based on current state?

	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		AddNetworkFailureHandlers();

		OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UShooterGameInstance::OnJoinSessionComplete);
		if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SessionIndexInSearchResults))
		{
			// If any error occured in the above, pending state would be set
			if ( (PendingState == CurrentState) || (PendingState == ShooterGameInstanceState::None) )
			{
				// Go ahead and go into loading state now
				// If we fail, the delegate will handle showing the proper messaging and move to the correct state
				ShowLoadingScreen();
				GotoState(ShooterGameInstanceState::Playing);
				return true;
			}
		}
	}

	return false;
}

bool UShooterGameInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::JoinSession()"));
	// needs to tear anything down based on current state?
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		AddNetworkFailureHandlers();

		OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &UShooterGameInstance::OnJoinSessionComplete);
		if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SearchResult))
		{
			// If any error occured in the above, pending state would be set
			if ( (PendingState == CurrentState) || (PendingState == ShooterGameInstanceState::None) )
			{
				// Go ahead and go into loading state now
				// If we fail, the delegate will handle showing the proper messaging and move to the correct state
				ShowLoadingScreen();
				GotoState(ShooterGameInstanceState::Playing);
				return true;
			}
		}
	}

	return false;
}

bool UShooterGameInstance::PlayDemo(ULocalPlayer* LocalPlayer, const FString& DemoName)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::PlayDemo()"));
	ShowLoadingScreen();

	// Play the demo
	PlayReplay(DemoName);
	
	return true;
}

/** Callback which is intended to be called upon finding sessions */
void UShooterGameInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnJoinSessionComplete()"));
	// unhook the delegate
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
	}

	// Add the splitscreen player if one exists
	if (Result == EOnJoinSessionCompleteResult::Success && LocalPlayers.Num() > 1)
	{
		auto Sessions = Online::GetSessionInterface();
		if (Sessions.IsValid() && LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
		{
			Sessions->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), NAME_GameSession,
				FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnRegisterJoiningLocalPlayerComplete));
		}
	}
	else
	{
		// We either failed or there is only a single local user
		FinishJoinSession(Result);
	}
}

void UShooterGameInstance::FinishJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::FinishJoinSession()"));
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		FText ReturnReason;
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::SessionIsFull:
			ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Game is full.");
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Game no longer exists.");
			break;
		default:
			ReturnReason = NSLOCTEXT("NetworkErrors", "JoinSessionFailed", "Join failed.");
			break;
		}

		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		RemoveNetworkFailureHandlers();
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
		return;
	}

	InternalTravelToSession(NAME_GameSession);
}

void UShooterGameInstance::OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnRegisterJoiningLocalPlayerComplete()"));
	FinishJoinSession(Result);
}

void UShooterGameInstance::InternalTravelToSession(const FName& SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::InternalTravelToSession()"));
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	if ( PlayerController == nullptr )
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "InvalidPlayerController", "Invalid Player Controller");
		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		RemoveNetworkFailureHandlers();
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
		return;
	}

	// travel to session
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if ( OnlineSub == nullptr )
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "OSSMissing", "OSS missing");
		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		RemoveNetworkFailureHandlers();
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
		return;
	}

	FString URL;
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

	if ( !Sessions.IsValid() || !Sessions->GetResolvedConnectString( SessionName, URL ) )
	{
		FText FailReason = NSLOCTEXT("NetworkErrors", "TravelSessionFailed", "Travel to Session failed.");
		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		ShowMessageThenGoMain(FailReason, OKButton, FText::GetEmpty());
		UE_LOG(LogOnlineGame, Warning, TEXT("Failed to travel to session upon joining it"));
		return;
	}

	// Add debug encryption token if desired.
	if (CVarShooterGameTestEncryption->GetInt() != 0)
	{
		// This is just a value for testing/debugging, the server will use the same key regardless of the token value.
		// But the token could be a user ID and/or session ID that would be used to generate a unique key per user and/or session, if desired.
		URL += TEXT("?EncryptionToken=1");
	}

	PlayerController->ClientTravel(URL, TRAVEL_Absolute);
}

/** Callback which is intended to be called upon session creation */
void UShooterGameInstance::OnCreatePresenceSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnCreatePresenceSessionComplete()"));
	AShooterGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnCreatePresenceSessionComplete().Remove(OnCreatePresenceSessionCompleteDelegateHandle);

		// Add the splitscreen player if one exists
		if (bWasSuccessful && LocalPlayers.Num() > 1)
		{
			auto Sessions = Online::GetSessionInterface();
			if (Sessions.IsValid() && LocalPlayers[1]->GetPreferredUniqueNetId().IsValid())
			{
				Sessions->RegisterLocalPlayer(*LocalPlayers[1]->GetPreferredUniqueNetId(), NAME_GameSession,
					FOnRegisterLocalPlayerCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnRegisterLocalPlayerComplete));
			}
		}
		else
		{
			// We either failed or there is only a single local user
			FinishSessionCreation(bWasSuccessful ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
		}
	}
}

/** Initiates the session searching */
bool UShooterGameInstance::FindSessions(ULocalPlayer* PlayerOwner, bool bIsDedicatedServer, bool bFindLAN)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::FindSessions()"));
	bool bResult = false;

	check(PlayerOwner != nullptr);
	if (PlayerOwner)
	{
		AShooterGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			GameSession->OnFindSessionsComplete().RemoveAll(this);
			OnSearchSessionsCompleteDelegateHandle = GameSession->OnFindSessionsComplete().AddUObject(this, &UShooterGameInstance::OnSearchSessionsComplete);

			GameSession->FindSessions(PlayerOwner->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, bFindLAN, !bIsDedicatedServer);

			bResult = true;
		}
	}

	return bResult;
}

/** Callback which is intended to be called upon finding sessions */
void UShooterGameInstance::OnSearchSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnSearchSessionsComplete()"));
	AShooterGameSession* const Session = GetGameSession();
	if (Session)
	{
		Session->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
	}
}

bool UShooterGameInstance::Tick(float DeltaSeconds)
{
	// Dedicated server doesn't need to worry about game state
	if (IsRunningDedicatedServer() == true)
	{
		return true;
	}

	MaybeChangeState();

	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if (CurrentState != ShooterGameInstanceState::WelcomeScreen)
	{
		// If at any point we aren't licensed (but we are after welcome screen) bounce them back to the welcome screen
		if (!bIsLicensed && CurrentState != ShooterGameInstanceState::None && !ShooterViewport->IsShowingDialog())
		{
			const FText ReturnReason	= NSLOCTEXT( "ProfileMessages", "NeedLicense", "The signed in users do not have a license for this game. Please purchase ShooterGame from the Xbox Marketplace or sign in a user with a valid license." );
			const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

			ShowMessageThenGotoState( ReturnReason, OKButton, FText::GetEmpty(), ShooterGameInstanceState::WelcomeScreen );
		}

		// Show controller disconnected dialog if any local players have an invalid controller
		if(ShooterViewport != NULL &&
			!ShooterViewport->IsShowingDialog())
		{
			for (int i = 0; i < LocalPlayers.Num(); ++i)
			{
				if (LocalPlayers[i] && LocalPlayers[i]->GetControllerId() == -1)
				{
					ShooterViewport->ShowDialog( 
						LocalPlayers[i],
						EShooterDialogType::ControllerDisconnected,
						FText::Format(NSLOCTEXT("ProfileMessages", "PlayerReconnectControllerFmt", "Player {0}, please reconnect your controller."), FText::AsNumber(i + 1)),
#if PLATFORM_PS4
						NSLOCTEXT("DialogButtons", "PS4_CrossButtonContinue", "Cross Button - Continue"),
#else
						NSLOCTEXT("DialogButtons", "AButtonContinue", "A - Continue"),
#endif
						FText::GetEmpty(),
						FOnClicked::CreateUObject(this, &UShooterGameInstance::OnControllerReconnectConfirm),
						FOnClicked()
					);
				}
			}
		}
	}

	// If we have a pending invite, and we are at the welcome screen, and the session is properly shut down, accept it
	if (PendingInvite.UserId.IsValid() && PendingInvite.bPrivilegesCheckedAndAllowed && CurrentState == ShooterGameInstanceState::PendingInvite)
	{
		IOnlineSubsystem * OnlineSub = IOnlineSubsystem::Get();
		IOnlineSessionPtr Sessions = (OnlineSub != NULL) ? OnlineSub->GetSessionInterface() : NULL;

		if (Sessions.IsValid())
		{
			EOnlineSessionState::Type SessionState = Sessions->GetSessionState(NAME_GameSession);

			if (SessionState == EOnlineSessionState::NoSession)
			{
				ULocalPlayer * NewPlayerOwner = GetFirstGamePlayer();

				if (NewPlayerOwner != nullptr)
				{
					NewPlayerOwner->SetControllerId(PendingInvite.ControllerId);
					NewPlayerOwner->SetCachedUniqueNetId(PendingInvite.UserId);
					SetOnlineMode(EOnlineMode::Online);

					const bool bIsLocalPlayerHost = PendingInvite.UserId.IsValid() && PendingInvite.InviteResult.Session.OwningUserId.IsValid() && *PendingInvite.UserId == *PendingInvite.InviteResult.Session.OwningUserId;
					if (bIsLocalPlayerHost)
					{
						HostQuickSession(*NewPlayerOwner, PendingInvite.InviteResult.Session.SessionSettings);
					}
					else
					{
						JoinSession(NewPlayerOwner, PendingInvite.InviteResult);
					}
				}

				PendingInvite.UserId.Reset();
			}
		}
	}

	return true;
}

bool UShooterGameInstance::HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleOpenCommand()"));
	bool const bOpenSuccessful = Super::HandleOpenCommand(Cmd, Ar, InWorld);
	if (bOpenSuccessful)
	{
		GotoState(ShooterGameInstanceState::MainMenu);
	}

	return bOpenSuccessful;
}

void UShooterGameInstance::HandleSignInChangeMessaging()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleSignInChangeMessaging()"));
	// Master user signed out, go to initial state (if we aren't there already)
	if ( CurrentState != GetInitialState() )
	{
#if SHOOTER_CONSOLE_UI
		// Display message on consoles
		const FText ReturnReason	= NSLOCTEXT( "ProfileMessages", "SignInChange", "Sign in status change occurred." );
		const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

		ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), GetInitialState());
#else								
		GotoInitialState();
#endif
	}
}

void UShooterGameInstance::HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleUserLoginChanged()"));
	// On Switch, accounts can play in LAN games whether they are signed in online or not. 
#if PLATFORM_SWITCH
	const bool bDowngraded = LoginStatus == ELoginStatus::NotLoggedIn || (GetOnlineMode() == EOnlineMode::Online && LoginStatus == ELoginStatus::UsingLocalProfile);
#else
	const bool bDowngraded = (LoginStatus == ELoginStatus::NotLoggedIn && GetOnlineMode() == EOnlineMode::Offline) || (LoginStatus != ELoginStatus::LoggedIn && GetOnlineMode() != EOnlineMode::Offline);
#endif

	UE_LOG( LogOnline, Log, TEXT( "HandleUserLoginChanged: bDownGraded: %i" ), (int)bDowngraded );

	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();

	// Find the local player associated with this unique net id
	ULocalPlayer * LocalPlayer = FindLocalPlayerFromUniqueNetId( UserId );

	LocalPlayerOnlineStatus[GameUserIndex] = LoginStatus;

	// If this user is signed out, but was previously signed in, punt to welcome (or remove splitscreen if that makes sense)
	if ( LocalPlayer != NULL )
	{
		if (bDowngraded)
		{
			UE_LOG( LogOnline, Log, TEXT( "HandleUserLoginChanged: Player logged out: %s" ), *UserId.ToString() );

			LabelPlayerAsQuitter(LocalPlayer);

			// Check to see if this was the master, or if this was a split-screen player on the client
			if ( LocalPlayer == GetFirstGamePlayer() || GetOnlineMode() != EOnlineMode::Offline )
			{
				HandleSignInChangeMessaging();
			}
			else
			{
				// Remove local split-screen players from the list
				RemoveExistingLocalPlayer( LocalPlayer );
			}
		}
	}
}

void UShooterGameInstance::HandleAppWillDeactivate()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleAppWillDeactivate()"));
	if (CurrentState == ShooterGameInstanceState::Playing)
	{
		// Just have the first player controller pause the game.
		UWorld* const GameWorld = GetWorld();
		if (GameWorld)
		{
			// protect against a second pause menu loading on top of an existing one if someone presses the Jewel / PS buttons.
			bool bNeedsPause = true;
			for (FConstControllerIterator It = GameWorld->GetControllerIterator(); It; ++It)
			{
				AShooterPlayerController* Controller = Cast<AShooterPlayerController>(*It);
				if (Controller && (Controller->IsPaused() || Controller->IsGameMenuVisible()))
				{
					bNeedsPause = false;
					break;
				}
			}

			if (bNeedsPause)
			{
				AShooterPlayerController* const Controller = Cast<AShooterPlayerController>(GameWorld->GetFirstPlayerController());
				if (Controller)
				{
					Controller->ShowInGameMenu();
				}
			}
		}
	}
}

void UShooterGameInstance::HandleAppSuspend()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleAppSuspend()"));
	// Players will lose connection on resume. However it is possible the game will exit before we get a resume, so we must kick off round end events here.
	UE_LOG( LogOnline, Warning, TEXT( "UShooterGameInstance::HandleAppSuspend" ) );
	UWorld* const World = GetWorld(); 
	AShooterGameState* const GameState = World != NULL ? World->GetGameState<AShooterGameState>() : NULL;

	if ( CurrentState != ShooterGameInstanceState::None && CurrentState != GetInitialState() )
	{
		UE_LOG( LogOnline, Warning, TEXT( "UShooterGameInstance::HandleAppSuspend: Sending round end event for players" ) );

		// Send round end events for local players
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			auto ShooterPC = Cast<AShooterPlayerController>(LocalPlayers[i]->PlayerController);
			if (ShooterPC && GameState)
			{
				// Assuming you can't win if you quit early
				ShooterPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
			}
		}
	}
}

void UShooterGameInstance::HandleAppResume()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleAppResume()"));
	UE_LOG( LogOnline, Log, TEXT( "UShooterGameInstance::HandleAppResume" ) );

	if ( CurrentState != ShooterGameInstanceState::None && CurrentState != GetInitialState() )
	{
		UE_LOG( LogOnline, Warning, TEXT( "UShooterGameInstance::HandleAppResume: Attempting to sign out players" ) );

		for ( int32 i = 0; i < LocalPlayers.Num(); ++i )
		{
			if ( LocalPlayers[i]->GetCachedUniqueNetId().IsValid() && LocalPlayerOnlineStatus[i] == ELoginStatus::LoggedIn && !IsLocalPlayerOnline( LocalPlayers[i] ) )
			{
				UE_LOG( LogOnline, Log, TEXT( "UShooterGameInstance::HandleAppResume: Signed out during resume." ) );
				HandleSignInChangeMessaging();
				break;
			}
		}
	}
}

void UShooterGameInstance::HandleAppLicenseUpdate()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleAppLicenseUpdate()"));
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();
}

void UShooterGameInstance::HandleSafeFrameChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleSafeFrameChanged()"));
	UCanvas::UpdateAllCanvasSafeZoneData();
}

void UShooterGameInstance::RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::RemoveExistingLocalPlayer()"));
	check(ExistingPlayer);
	if (ExistingPlayer->PlayerController != NULL)
	{
		// Kill the player
		AShooterCharacter* MyPawn = Cast<AShooterCharacter>(ExistingPlayer->PlayerController->GetPawn());
		if ( MyPawn )
		{
			MyPawn->KilledBy(NULL);
		}
	}

	// Remove local split-screen players from the list
	RemoveLocalPlayer( ExistingPlayer );
}

void UShooterGameInstance::RemoveSplitScreenPlayers()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::RemoveSplitScreenPlayers()"));
	// if we had been split screen, toss the extra players now
	// remove every player, back to front, except the first one
	while (LocalPlayers.Num() > 1)
	{
		ULocalPlayer* const PlayerToRemove = LocalPlayers.Last();
		RemoveExistingLocalPlayer(PlayerToRemove);
	}
}

FReply UShooterGameInstance::OnPairingUsePreviousProfile()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPairingUsePreviousProfile()"));
	// Do nothing (except hide the message) if they want to continue using previous profile
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if ( ShooterViewport != nullptr )
	{
		ShooterViewport->HideDialog();
	}

	return FReply::Handled();
}

FReply UShooterGameInstance::OnPairingUseNewProfile()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPairingUseNewProfile()"));
	HandleSignInChangeMessaging();
	return FReply::Handled();
}

void UShooterGameInstance::HandleControllerPairingChanged( int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleControllerPairingChanged()"));
	UE_LOG(LogOnlineGame, Log, TEXT("UShooterGameInstance::HandleControllerPairingChanged GameUserIndex %d PreviousUser '%s' NewUser '%s'"),
		GameUserIndex, *PreviousUser.ToString(), *NewUser.ToString());
	
	if ( CurrentState == ShooterGameInstanceState::WelcomeScreen )
	{
		// Don't care about pairing changes at welcome screen
		return;
	}

#if SHOOTER_CONSOLE_UI && PLATFORM_XBOXONE
	if ( IgnorePairingChangeForControllerId != -1 && GameUserIndex == IgnorePairingChangeForControllerId )
	{
		// We were told to ignore
		IgnorePairingChangeForControllerId = -1;	// Reset now so there there is no chance this remains in a bad state
		return;
	}

	if ( PreviousUser.IsValid() && !NewUser.IsValid() )
	{
		// Treat this as a disconnect or signout, which is handled somewhere else
		return;
	}

	if ( !PreviousUser.IsValid() && NewUser.IsValid() )
	{
		// Treat this as a signin
		ULocalPlayer * ControlledLocalPlayer = FindLocalPlayerFromControllerId( GameUserIndex );

		if ( ControlledLocalPlayer != NULL && !ControlledLocalPlayer->GetCachedUniqueNetId().IsValid() )
		{
			// If a player that previously selected "continue without saving" signs into this controller, move them back to welcome screen
			HandleSignInChangeMessaging();
		}
		
		return;
	}

	// Find the local player currently being controlled by this controller
	ULocalPlayer * ControlledLocalPlayer	= FindLocalPlayerFromControllerId( GameUserIndex );

	// See if the newly assigned profile is in our local player list
	ULocalPlayer * NewLocalPlayer			= FindLocalPlayerFromUniqueNetId( NewUser );

	// If the local player being controlled is not the target of the pairing change, then give them a chance 
	// to continue controlling the old player with this controller
	if ( ControlledLocalPlayer != nullptr && ControlledLocalPlayer != NewLocalPlayer )
	{
		UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

		if ( ShooterViewport != nullptr )
		{
			ShooterViewport->ShowDialog( 
				nullptr,
				EShooterDialogType::Generic,
				NSLOCTEXT("ProfileMessages", "PairingChanged", "Your controller has been paired to another profile, would you like to switch to this new profile now? Selecting YES will sign out of the previous profile."),
				NSLOCTEXT("DialogButtons", "YES", "A - YES"),
				NSLOCTEXT("DialogButtons", "NO", "B - NO"),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnPairingUseNewProfile),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnPairingUsePreviousProfile)
			);
		}
	}
#endif
}

void UShooterGameInstance::HandleControllerConnectionChange( bool bIsConnection, int32 Unused, int32 GameUserIndex )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::HandleControllerConnectionChange()"));
	UE_LOG(LogOnlineGame, Log, TEXT("UShooterGameInstance::HandleControllerConnectionChange bIsConnection %d GameUserIndex %d"),
		bIsConnection, GameUserIndex);

	if(!bIsConnection)
	{
		// Controller was disconnected

		// Find the local player associated with this user index
		ULocalPlayer * LocalPlayer = FindLocalPlayerFromControllerId( GameUserIndex );

		if ( LocalPlayer == NULL )
		{
			return;		// We don't care about players we aren't tracking
		}

		// Invalidate this local player's controller id.
		LocalPlayer->SetControllerId(-1);
	}
}

FReply UShooterGameInstance::OnControllerReconnectConfirm()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnControllerReconnectConfirm()"));
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());
	if(ShooterViewport)
	{
		ShooterViewport->HideDialog();
	}

	return FReply::Handled();
}

TSharedPtr< const FUniqueNetId > UShooterGameInstance::GetUniqueNetIdFromControllerId( const int ControllerId )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::GetUniqueNetIdFromControllerId()"));
	IOnlineIdentityPtr OnlineIdentityInt = Online::GetIdentityInterface();

	if ( OnlineIdentityInt.IsValid() )
	{
		TSharedPtr<const FUniqueNetId> UniqueId = OnlineIdentityInt->GetUniquePlayerId( ControllerId );

		if ( UniqueId.IsValid() )
		{
			return UniqueId;
		}
	}

	return nullptr;
}

void UShooterGameInstance::SetOnlineMode(EOnlineMode InOnlineMode)
{
	if (InOnlineMode == EOnlineMode::Online)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineMode : Online"));
	}
	else if (InOnlineMode == EOnlineMode::LAN)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineMode : LAN"));
	}
	else if (InOnlineMode == EOnlineMode::Offline)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnlineMode : Offline"));
	}
	OnlineMode = InOnlineMode;
	UpdateUsingMultiplayerFeatures(InOnlineMode == EOnlineMode::Online);
}

void UShooterGameInstance::UpdateUsingMultiplayerFeatures(bool bIsUsingMultiplayerFeatures)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::UpdateUsingMultiplayerFeatures()"));
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		for (int32 i = 0; i < LocalPlayers.Num(); ++i)
		{
			ULocalPlayer* LocalPlayer = LocalPlayers[i];

			FUniqueNetIdRepl PlayerId = LocalPlayer->GetPreferredUniqueNetId();
			if (PlayerId.IsValid())
			{
				OnlineSub->SetUsingMultiplayerFeatures(*PlayerId, bIsUsingMultiplayerFeatures);
			}
		}
	}
}

void UShooterGameInstance::TravelToSession(const FName& SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::TravelToSession()"));
	// Added to handle failures when joining using quickmatch (handles issue of joining a game that just ended, i.e. during game ending timer)
	AddNetworkFailureHandlers();
	ShowLoadingScreen();
	GotoState(ShooterGameInstanceState::Playing);
	InternalTravelToSession(SessionName);
}

void UShooterGameInstance::SetIgnorePairingChangeForControllerId( const int32 ControllerId )
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SetIgnorePairingChangeForControllerId()"));
	IgnorePairingChangeForControllerId = ControllerId;
}

bool UShooterGameInstance::IsLocalPlayerOnline(ULocalPlayer* LocalPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::IsLocalPlayerOnline()"));
	if (LocalPlayer == NULL)
	{
		return false;
	}
	const auto OnlineSub = IOnlineSubsystem::Get();
	if(OnlineSub)
	{
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		if(IdentityInterface.IsValid())
		{
			auto UniqueId = LocalPlayer->GetCachedUniqueNetId();
			if (UniqueId.IsValid())
			{
				const auto LoginStatus = IdentityInterface->GetLoginStatus(*UniqueId);
				if(LoginStatus == ELoginStatus::LoggedIn)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UShooterGameInstance::IsLocalPlayerSignedIn(ULocalPlayer* LocalPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::IsLocalPlayerSignedIn()"));
	if (LocalPlayer == NULL)
	{
		return false;
	}

	const auto OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			auto UniqueId = LocalPlayer->GetCachedUniqueNetId();
			if (UniqueId.IsValid())
			{
				return true;
			}
		}
	}

	return false;
}

bool UShooterGameInstance::ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ValidatePlayerForOnlinePlay()"));
	// Get the viewport
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

#if PLATFORM_XBOXONE
	if (CurrentConnectionStatus != EOnlineServerConnectionStatus::Connected)
	{
		// Don't let them play online if they aren't connected to Xbox LIVE
		if (ShooterViewport != NULL)
		{
			const FText Msg				= NSLOCTEXT("NetworkFailures", "ServiceDisconnected", "You must be connected to the Xbox LIVE service to play online.");
			const FText OKButtonString	= NSLOCTEXT("DialogButtons", "OKAY", "OK");

			ShooterViewport->ShowDialog( 
				NULL,
				EShooterDialogType::Generic,
				Msg,
				OKButtonString,
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
			);
		}

		return false;
	}
#endif

	if (!IsLocalPlayerOnline(LocalPlayer))
	{
		// Don't let them play online if they aren't online
		if (ShooterViewport != NULL)
		{
			const FText Msg				= NSLOCTEXT("NetworkFailures", "MustBeSignedIn", "You must be signed in to play online");
			const FText OKButtonString	= NSLOCTEXT("DialogButtons", "OKAY", "OK");

			ShooterViewport->ShowDialog( 
				NULL,
				EShooterDialogType::Generic,
				Msg,
				OKButtonString,
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
			);
		}

		return false;
	}

	return true;
}

bool UShooterGameInstance::ValidatePlayerIsSignedIn(ULocalPlayer* LocalPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ValidatePlayerIsSignedIn()"));
	// Get the viewport
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());

	if (!IsLocalPlayerSignedIn(LocalPlayer))
	{
		// Don't let them play online if they aren't online
		if (ShooterViewport != NULL)
		{
			const FText Msg = NSLOCTEXT("NetworkFailures", "MustBeSignedIn", "You must be signed in to play online");
			const FText OKButtonString = NSLOCTEXT("DialogButtons", "OKAY", "OK");

			ShooterViewport->ShowDialog(
				NULL,
				EShooterDialogType::Generic,
				Msg,
				OKButtonString,
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
			);
		}

		return false;
	}

	return true;
}


FReply UShooterGameInstance::OnConfirmGeneric()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnConfirmGeneric()"));
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());
	if(ShooterViewport)
	{
		ShooterViewport->HideDialog();
	}

	return FReply::Handled();
}

void UShooterGameInstance::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr< const FUniqueNetId > UserId)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::StartOnlinePrivilegeTask()"));
	WaitMessageWidget = SNew(SShooterWaitDialog)
		.MessageText(NSLOCTEXT("NetworkStatus", "CheckingPrivilegesWithServer", "Checking privileges with server.  Please wait..."));

	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* const GVC = GEngine->GameViewport;
		GVC->AddViewportWidgetContent(WaitMessageWidget.ToSharedRef());
	}

	auto Identity = Online::GetIdentityInterface();
	if (Identity.IsValid() && UserId.IsValid())
	{		
		Identity->GetUserPrivilege(*UserId, Privilege, Delegate);
	}
	else
	{
		// Can only get away with faking the UniqueNetId here because the delegates don't use it
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
	}
}

void UShooterGameInstance::CleanupOnlinePrivilegeTask()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::CleanupOnlinePrivilegeTask()"));
	if (GEngine && GEngine->GameViewport && WaitMessageWidget.IsValid())
	{
		UGameViewportClient* const GVC = GEngine->GameViewport;
		GVC->RemoveViewportWidgetContent(WaitMessageWidget.ToSharedRef());
	}
}

void UShooterGameInstance::DisplayOnlinePrivilegeFailureDialogs(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::DisplayOnlinePrivilegeFailureDialogs()"));
	// Show warning that the user cannot play due to age restrictions
	UShooterGameViewportClient * ShooterViewport = Cast<UShooterGameViewportClient>(GetGameViewportClient());
	TWeakObjectPtr<ULocalPlayer> OwningPlayer;
	if (GEngine)
	{
		for (auto It = GEngine->GetLocalPlayerIterator(GetWorld()); It; ++It)
		{
			FUniqueNetIdRepl OtherId = (*It)->GetPreferredUniqueNetId();
			if (OtherId.IsValid())
			{
				if (UserId == (*OtherId))
				{
					OwningPlayer = *It;
				}
			}
		}
	}
	
	if (ShooterViewport != NULL && OwningPlayer.IsValid())
	{
		if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AccountTypeFailure) != 0)
		{
			IOnlineExternalUIPtr ExternalUI = Online::GetExternalUIInterface();
			if (ExternalUI.IsValid())
			{
				ExternalUI->ShowAccountUpgradeUI(UserId);
			}
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredSystemUpdate) != 0)
		{
			ShooterViewport->ShowDialog(
				OwningPlayer.Get(),
				EShooterDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "RequiredSystemUpdate", "A required system update is available.  Please upgrade to access online features."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
				);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::RequiredPatchAvailable) != 0)
		{
			ShooterViewport->ShowDialog(
				OwningPlayer.Get(),
				EShooterDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "RequiredPatchAvailable", "A required game patch is available.  Please upgrade to access online features."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
				);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::AgeRestrictionFailure) != 0)
		{
			ShooterViewport->ShowDialog(
				OwningPlayer.Get(),
				EShooterDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "AgeRestrictionFailure", "Cannot play due to age restrictions!"),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
				);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::UserNotFound) != 0)
		{
			ShooterViewport->ShowDialog(
				OwningPlayer.Get(),
				EShooterDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "UserNotFound", "Cannot play due invalid user!"),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
				);
		}
		else if ((PrivilegeResults & (uint32)IOnlineIdentity::EPrivilegeResults::GenericFailure) != 0)
		{
			ShooterViewport->ShowDialog(
				OwningPlayer.Get(),
				EShooterDialogType::Generic,
				NSLOCTEXT("OnlinePrivilegeResult", "GenericFailure", "Cannot play online.  Check your network connection."),
				NSLOCTEXT("DialogButtons", "OKAY", "OK"),
				FText::GetEmpty(),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric),
				FOnClicked::CreateUObject(this, &UShooterGameInstance::OnConfirmGeneric)
				);
		}
	}
}

void UShooterGameInstance::OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnRegisterLocalPlayerComplete()"));
	FinishSessionCreation(Result);
}

void UShooterGameInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::FinishSessionCreation( %s )"), *FString::Printf(TEXT("TravelURL = %s"), *TravelURL));
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		// This will send any Play Together invites if necessary, or do nothing.
		SendPlayTogetherInvites();

		// Travel to the specified match URL
		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		FText ReturnReason = NSLOCTEXT("NetworkErrors", "CreateSessionFailed", "Failed to create session.");
		FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");
		ShowMessageThenGoMain(ReturnReason, OKButton, FText::GetEmpty());
	}
}

FString UShooterGameInstance::GetQuickMatchUrl()
{
	static const FString QuickMatchUrl(TEXT("/Game/Maps/Highrise?game=TDM?listen"));
	return QuickMatchUrl;
}

void UShooterGameInstance::BeginHostingQuickMatch()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::BeginHostingQuickMatch()"));
	ShowLoadingScreen();
	GotoState(ShooterGameInstanceState::Playing);

	// Travel to the specified match URL
	GetWorld()->ServerTravel(GetQuickMatchUrl());	
}

void UShooterGameInstance::OnPlayTogetherEventReceived(const int32 UserIndex, const TArray<TSharedPtr<const FUniqueNetId>>& UserIdList)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnPlayTogetherEventReceived()"));
	PlayTogetherInfo = FShooterPlayTogetherInfo(UserIndex, UserIdList);

	const IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);

	const IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
	check(SessionInterface.IsValid());

	// If we have available slots to accomedate the whole party in our current sessions, we should send invites to the existing one
	// instead of a new one according to Sony's best practices.
	const FNamedOnlineSession* const Session = SessionInterface->GetNamedSession(NAME_GameSession);
	if (Session != nullptr && Session->NumOpenPrivateConnections + Session->NumOpenPublicConnections >= UserIdList.Num())
	{
		SendPlayTogetherInvites();
	}
	// Always handle Play Together in the main menu since the player has session customization options.
	else if (CurrentState == ShooterGameInstanceState::MainMenu)
	{
		MainMenuUI->OnPlayTogetherEventReceived();
	}
	else if (CurrentState == ShooterGameInstanceState::WelcomeScreen)
	{
		StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &UShooterGameInstance::OnUserCanPlayTogether), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
	}
	else
	{
		GotoState(ShooterGameInstanceState::MainMenu);
	}
}

void UShooterGameInstance::SendPlayTogetherInvites()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SendPlayTogetherInvites()"));
	const IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);

	const IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
	check(SessionInterface.IsValid());

	if (PlayTogetherInfo.UserIndex != -1)
	{
		for (const ULocalPlayer* LocalPlayer : LocalPlayers)
		{
			if (LocalPlayer->GetControllerId() == PlayTogetherInfo.UserIndex)
			{
				FUniqueNetIdRepl PlayerId = LocalPlayer->GetPreferredUniqueNetId();
				if (PlayerId.IsValid())
				{
					// Automatically send invites to friends in the player's PS4 party to conform with Play Together requirements
					for (const TSharedPtr<const FUniqueNetId>& FriendId : PlayTogetherInfo.UserIdList)
					{
						SessionInterface->SendSessionInviteToFriend(*PlayerId, NAME_GameSession, *FriendId.ToSharedRef());
					}
				}

			}
		}

		PlayTogetherInfo = FShooterPlayTogetherInfo();
	}
}

void UShooterGameInstance::ReceivedNetworkEncryptionToken(const FString& EncryptionToken, const FOnEncryptionKeyResponse& Delegate)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ReceivedNetworkEncryptionToken()"));
	// This is a simple implementation to demonstrate using encryption for game traffic using a hardcoded key.
	// For a complete implementation, you would likely want to retrieve the encryption key from a secure source,
	// such as from a web service over HTTPS. This could be done in this function, even asynchronously - just
	// call the response delegate passed in once the key is known. The contents of the EncryptionToken is up to the user,
	// but it will generally contain information used to generate a unique encryption key, such as a user and/or session ID.

	FEncryptionKeyResponse Response(EEncryptionResponse::Failure, TEXT("Unknown encryption failure"));

	if (EncryptionToken.IsEmpty())
	{
		Response.Response = EEncryptionResponse::InvalidToken;
		Response.ErrorMsg = TEXT("Encryption token is empty.");
	}
	else
	{
		Response.Response = EEncryptionResponse::Success;
		Response.EncryptionKey = DebugTestEncryptionKey;
	}

	Delegate.ExecuteIfBound(Response);

}

void UShooterGameInstance::ReceivedNetworkEncryptionAck(const FOnEncryptionKeyResponse& Delegate)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ReceivedNetworkEncryptionAck()"));
	// This is a simple implementation to demonstrate using encryption for game traffic using a hardcoded key.
	// For a complete implementation, you would likely want to retrieve the encryption key from a secure source,
	// such as from a web service over HTTPS. This could be done in this function, even asynchronously - just
	// call the response delegate passed in once the key is known.

	FEncryptionKeyResponse Response;

	TArray<uint8> FakeKey;
	
	Response.Response = EEncryptionResponse::Success;
	Response.EncryptionKey = DebugTestEncryptionKey;

	Delegate.ExecuteIfBound(Response);
}

void UShooterGameInstance::AddPawnSource(UShooterSaveGame* SaveGame, bool bRemoveExtra)
{
	/*
	if (Role < ROLE_Authority)
	{
		return;
	}*/
	// If we want to remove extra, clear out the existing inventory
	if (bRemoveExtra)
	{
		SaveGame->PawnAssetSource.Reset();
	}

	// Now add the default inventory, this only adds if not already in hte inventory
	for (const TPair<FPrimaryAssetId, FShooterItemData>& Pair : DataSource)
	{
		if (!SaveGame->PawnAssetSource.Contains(Pair.Key))
		{
			SaveGame->PawnAssetSource.Add(Pair.Key, Pair.Value);
		}
	}
}

void UShooterGameInstance::AddDataSource(UShooterSaveGame* SaveGame, bool bRemoveExtra)
{
	/*
	if (Role < ROLE_Authority)
	{
		return;
	}*/
	// If we want to remove extra, clear out the existing inventory
	if (bRemoveExtra)
	{
		SaveGame->AssetDataSource.Reset();
		SaveGame->AssetSources.Reset();
	}

	// Now add the default inventory, this only adds if not already in hte inventory
	for (const TPair<FPrimaryAssetId, FShooterItemData>& Pair : DataSource)
	{
		if (!SaveGame->AssetDataSource.Contains(Pair.Key))
		{
			SaveGame->AssetDataSource.Add(Pair.Key, Pair.Value);
		}
	}
	for (auto Temp : AssetSources)
	{
		SaveGame->AssetSources.AddUnique(Temp);
	}
}

bool UShooterGameInstance::IsValidItemSlot(FShooterItemSlot ItemSlot) const
{
	if (ItemSlot.IsValid())
	{
		const int32* FoundCount = ItemSlotsPerType.Find(ItemSlot.ItemType);

		if (FoundCount)
		{
			return ItemSlot.SlotNumber < *FoundCount;
		}
	}
	return false;
}

UShooterSaveGame* UShooterGameInstance::GetCurrentSaveGame()
{
	return CurrentSaveGame;
}

void UShooterGameInstance::SetSavingEnabled(bool bEnabled)
{
	bSavingEnabled = bEnabled;
}

bool UShooterGameInstance::LoadOrCreateSaveGame()
{
	// Drop reference to old save game, this will GC out
	CurrentSaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled)
	{
		CurrentSaveGame = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, SaveUserIndex));
	}

	if (CurrentSaveGame)
	{
		// Make sure it has any newly added default inventory
		AddDataSource(CurrentSaveGame, false);

		return true;
	}
	else
	{
		// This creates it on demand
		CurrentSaveGame = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));

		AddDataSource(CurrentSaveGame, true);

		return false;
	}
}

bool UShooterGameInstance::WriteSaveGame()
{
	if (bSavingEnabled)
	{
		return UGameplayStatics::SaveGameToSlot(GetCurrentSaveGame(), SaveSlot, SaveUserIndex);
	}
	return false;
}

void UShooterGameInstance::ResetSaveGame()
{
	bool bWasSavingEnabled = bSavingEnabled;
	bSavingEnabled = false;
	LoadOrCreateSaveGame();
	bSavingEnabled = bWasSavingEnabled;
}

