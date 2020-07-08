// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SShooterSessionsWidget.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "ShooterPlayerController_Login.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"


#define LOCTEXT_NAMESPACE "ShooterGame.HUD.LoginHUD"

// Sets default values
void SShooterSessionsWidget::Construct(const FArguments& args)
{
	//OwnerHUD = args._OwnerHUD;

	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");
	//GameInstance = Cast<UShooterGameInstance>(OwnerHUD->GetGameInstance());
	//PlayerOwner = OwnerHUD->GetGameInstance()->GetFirstGamePlayer();

	PlayerOwner = args._PlayerOwner;
	OwnerWidget = args._OwnerWidget;

	bSearchingForServers = false;

	bLANMatchSearch = false;
	bDedicatedServer = true;

	MapFilterName = "Any";
	StatusText = FText::GetEmpty();
	BoxWidth = 125;
	OutlineWidth = 20.0f;

	//HUDStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");
	//SShooterPawnGuideWidget::FOnSelectedContentSourceChanged::CreateSP(this, &SShooterPawnGuideWidget::SelectedContentSourceChanged)
	// Slate Attribute binding.
	//Score.Bind(this, &SShooterSessionsWidget::GetScore);

	// Fill view source.
	//RebuildViewSource();

	// Layout
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Fill)
		.AutoWidth()
		[
			SNew(SBorder)
			.BorderImage(&MenuStyle->LeftBackgroundBrush)
			.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
			.Padding(FMargin(OutlineWidth))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(10.f, 0, 10.f, 10.f)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(60)
					.WidthOverride(120)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
						.Padding(FMargin(OutlineWidth / 2))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ButtonColorAndOpacity(FSlateColor(FLinearColor::Transparent))
							.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuButtonTextStyle")
							.OnClicked(this, &SShooterSessionsWidget::OnSessionRefresh)
							.Text(LOCTEXT("Refresh", "刷新"))
						]
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(10.f, 0, 10.f, 10.f)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(60)
					.WidthOverride(120)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
						.Padding(FMargin(OutlineWidth / 2))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ButtonColorAndOpacity(FSlateColor(FLinearColor::Transparent))
							.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuButtonTextStyle")
							.OnClicked(this, &SShooterSessionsWidget::OnBeginPlay)
							.Text(LOCTEXT("Create", "创建"))
						]
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(10.f, 0, 10.f, 10.f)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(60)
					.WidthOverride(120)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
						.Padding(FMargin(OutlineWidth / 2))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ButtonColorAndOpacity(FSlateColor(FLinearColor::Transparent))
							.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuButtonTextStyle")
							.OnClicked(this, &SShooterSessionsWidget::OnBeginPlay)
							.Text(LOCTEXT("Match", "匹配"))
						]
					]
				]
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(&MenuStyle->RightBackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.Padding(FMargin(OutlineWidth))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(ServerListWidget, SListView<TSharedPtr<FServerEntry>>)
					.ItemHeight(30)
					.ListItemsSource(&ServerList)
					.SelectionMode(ESelectionMode::Single)
					.OnGenerateRow(this, &SShooterSessionsWidget::MakeListViewWidget)
					.OnSelectionChanged(this, &SShooterSessionsWidget::EntrySelectionChanged)
					.OnMouseButtonDoubleClick(this, &SShooterSessionsWidget::OnListItemDoubleClicked)
					.HeaderRow(
						SNew(SHeaderRow)
						+ SHeaderRow::Column("ServerName").FixedWidth(BoxWidth * 2).DefaultLabel(NSLOCTEXT("ServerList", "ServerNameColumn", "房间"))
						+ SHeaderRow::Column("GameType").DefaultLabel(NSLOCTEXT("ServerList", "GameTypeColumn", "模式"))
						+ SHeaderRow::Column("Map").DefaultLabel(NSLOCTEXT("ServerList", "MapNameColumn", "地图"))
						+ SHeaderRow::Column("Players").DefaultLabel(NSLOCTEXT("ServerList", "PlayersColumn", "玩家"))
						+ SHeaderRow::Column("Ping").DefaultLabel(NSLOCTEXT("ServerList", "NetworkPingColumn", "延迟")))
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SRichTextBlock)
					.Text(this, &SShooterSessionsWidget::GetBottomText)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.DemoListCheckboxTextStyle")
					.DecoratorStyleSet(&FShooterStyle::Get())
					+ SRichTextBlock::ImageDecorator()
				]
			]
		]
	];
}

SShooterSessionsWidget::~SShooterSessionsWidget()
{

}

/**
 * Ticks this widget.  Override in derived classes, but always call the parent implementation.
 *
 * @param  InCurrentTime  Current absolute real time
 * @param  InDeltaTime  Real time passed since last tick
 */
void SShooterSessionsWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bSearchingForServers)
	{
		UpdateSearchStatus();
	}
}

/**
 * Get the current game session
 */
AShooterGameSession* SShooterSessionsWidget::GetGameSession() const
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::GetGameSession()"));
	UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	return GI ? GI->GetGameSession() : nullptr;
}

/** Updates current search status */
void SShooterSessionsWidget::UpdateSearchStatus()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::UpdateSearchStatus()"));
	check(bSearchingForServers); // should not be called otherwise

	bool bFinishSearch = true;
	AShooterGameSession* ShooterSession = GetGameSession();
	if (ShooterSession)
	{
		int32 CurrentSearchIdx, NumSearchResults;
		EOnlineAsyncTaskState::Type SearchState = ShooterSession->GetSearchResultStatus(CurrentSearchIdx, NumSearchResults);

		UE_LOG(LogOnlineGame, Log, TEXT("Session->GetSearchResultStatus: %s"), EOnlineAsyncTaskState::ToString(SearchState));

		switch (SearchState)
		{
		case EOnlineAsyncTaskState::InProgress:
			StatusText = LOCTEXT("Searching", "检索中...");
			bFinishSearch = false;
			break;

		case EOnlineAsyncTaskState::Done:
			// copy the results
		{
			ServerList.Empty();
			const TArray<FOnlineSessionSearchResult> & SearchResults = ShooterSession->GetSearchResults();
			check(SearchResults.Num() == NumSearchResults);
			if (NumSearchResults == 0)
			{
#if PLATFORM_PS4
				StatusText = LOCTEXT("NoServersFound", "未找到Server, 按“SQUARE”键重新检索.");
#elif PLATFORM_XBOXONE
				StatusText = LOCTEXT("NoServersFound", "未找到Server, 按“X”键重新检索.");
#elif PLATFORM_SWITCH
				StatusText = LOCTEXT("NoServersFound", "未找到Server, 按“<img src=\"ShooterGame.Switch.Left\"/>”键重新检索.");
#else
				StatusText = LOCTEXT("NoServersFound", "未找到Server, 按【刷新】或【空格】键重新检索.");
#endif
			}
			else
			{
				/*
#if PLATFORM_PS4
				StatusText = LOCTEXT("ServersRefresh", "按“SQUARE”键刷新列表.");
#elif PLATFORM_XBOXONE
				StatusText = LOCTEXT("ServersRefresh", "按“X”键刷新列表.");
#elif PLATFORM_SWITCH
				StatusText = LOCTEXT("ServersRefresh", "按“<img src=\"ShooterGame.Switch.Left\"/>”键刷新列表.");
#else
				StatusText = LOCTEXT("ServersRefresh", "");
#endif			*/
				StatusText = LOCTEXT("ServersRefresh", "");
			}

			for (int32 IdxResult = 0; IdxResult < NumSearchResults; ++IdxResult)
			{
				TSharedPtr<FServerEntry> NewServerEntry = MakeShareable(new FServerEntry());

				const FOnlineSessionSearchResult& Result = SearchResults[IdxResult];

				NewServerEntry->ServerName = Result.Session.OwningUserName;
				NewServerEntry->Ping = FString::FromInt(Result.PingInMs);
				NewServerEntry->CurrentPlayers = FString::FromInt(Result.Session.SessionSettings.NumPublicConnections
					+ Result.Session.SessionSettings.NumPrivateConnections
					- Result.Session.NumOpenPublicConnections
					- Result.Session.NumOpenPrivateConnections);
				NewServerEntry->MaxPlayers = FString::FromInt(Result.Session.SessionSettings.NumPublicConnections
					+ Result.Session.SessionSettings.NumPrivateConnections);
				NewServerEntry->SearchResultsIndex = IdxResult;

				Result.Session.SessionSettings.Get(SETTING_GAMEMODE, NewServerEntry->GameType);
				Result.Session.SessionSettings.Get(SETTING_MAPNAME, NewServerEntry->MapName);

				ServerList.Add(NewServerEntry);
			}
		}
		break;

		case EOnlineAsyncTaskState::Failed:
			// intended fall-through
		case EOnlineAsyncTaskState::NotStarted:
			StatusText = FText::GetEmpty();
			// intended fall-through
		default:
			break;
		}
	}

	if (bFinishSearch)
	{
		OnServerSearchFinished();
	}
}

/** Starts searching for servers */
void SShooterSessionsWidget::BeginServerSearch(bool bLANMatch, bool bIsDedicatedServer, const FString& InMapFilterName)
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::BeginServerSearch()"));
	double CurrentTime = FApp::GetCurrentTime();
	if (!bLANMatch && CurrentTime - LastSearchTime < MinTimeBetweenSearches)
	{
		OnServerSearchFinished();
	}
	else
	{
		bLANMatchSearch = bLANMatch;
		bDedicatedServer = bIsDedicatedServer;
		MapFilterName = InMapFilterName;
		bSearchingForServers = true;
		ServerList.Empty();
		LastSearchTime = CurrentTime;

		UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		if (GI)
		{
			GI->FindSessions(PlayerOwner.Get(), bIsDedicatedServer, bLANMatchSearch);
		}
	}
}

/** Called when server search is finished */
void SShooterSessionsWidget::OnServerSearchFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::OnServerSearchFinished()"));
	bSearchingForServers = false;

	UpdateServerList();
}

void SShooterSessionsWidget::UpdateServerList()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::UpdateServerList()"));
	/** Only filter maps if a specific map is specified */
	if (MapFilterName != "Any")
	{
		for (int32 i = 0; i < ServerList.Num(); ++i)
		{
			/** Only filter maps if a specific map is specified */
			if (ServerList[i]->MapName != MapFilterName)
			{
				ServerList.RemoveAt(i);
				i--;
			}
		}
	}

	int32 SelectedItemIndex = ServerList.IndexOfByKey(SelectedItem);

	ServerListWidget->RequestListRefresh();
	if (ServerList.Num() > 0)
	{
		ServerListWidget->UpdateSelectionSet();
		ServerListWidget->SetSelection(ServerList[SelectedItemIndex > -1 ? SelectedItemIndex : 0], ESelectInfo::OnNavigation);
	}
}

void SShooterSessionsWidget::ConnectToServer()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::ConnectToServer()"));
	if (bSearchingForServers)
	{
		// unsafe
		return;
	}
#if WITH_EDITOR
	if (GIsEditor == true)
	{
		return;
	}
#endif
	if (SelectedItem.IsValid())
	{
		int ServerToJoin = SelectedItem->SearchResultsIndex;

		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveAllViewportWidgets();
		}

		UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
		if (GI)
		{
			GI->JoinSession(PlayerOwner.Get(), ServerToJoin);
		}
	}
}

FText SShooterSessionsWidget::GetBottomText() const
{
	return StatusText;
}

void SShooterSessionsWidget::MoveSelection(int32 MoveBy)
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::MoveSelection()"));
	int32 SelectedItemIndex = ServerList.IndexOfByKey(SelectedItem);

	if (SelectedItemIndex + MoveBy > -1 && SelectedItemIndex + MoveBy < ServerList.Num())
	{
		ServerListWidget->SetSelection(ServerList[SelectedItemIndex + MoveBy]);
	}
}

FReply SShooterSessionsWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().SetUserFocus(ServerListWidget.ToSharedRef(), EFocusCause::SetDirectly).SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}

void SShooterSessionsWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
}

void SShooterSessionsWidget::EntrySelectionChanged(TSharedPtr<FServerEntry> InItem, ESelectInfo::Type SelectInfo)
{
	SelectedItem = InItem;
}

void SShooterSessionsWidget::OnListItemDoubleClicked(TSharedPtr<FServerEntry> InItem)
{
	SelectedItem = InItem;
	ConnectToServer();
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

FReply SShooterSessionsWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (bSearchingForServers) // lock input
	{
		return FReply::Handled();
	}

	FReply Result = FReply::Unhandled();
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		MoveSelection(-1);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		MoveSelection(1);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		ConnectToServer();
		Result = FReply::Handled();
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
	//hit space bar to search for servers again / refresh the list, only when not searching already
	else if (Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Left)
	{
		BeginServerSearch(bLANMatchSearch, bDedicatedServer, MapFilterName);
	}
	return Result;
}

TSharedRef<ITableRow> SShooterSessionsWidget::MakeListViewWidget(TSharedPtr<FServerEntry> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::MakeListViewWidget()"));
	class SServerEntryWidget : public SMultiColumnTableRow< TSharedPtr<FServerEntry> >
	{
	public:
		SLATE_BEGIN_ARGS(SServerEntryWidget) {}
		SLATE_END_ARGS()

			void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<FServerEntry> InItem)
		{
			Item = InItem;
			SMultiColumnTableRow< TSharedPtr<FServerEntry> >::Construct(FSuperRowType::FArguments(), InOwnerTable);
		}

		TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName)
		{
			FText ItemText = FText::GetEmpty();
			if (ColumnName == "ServerName")
			{
				ItemText = FText::FromString(Item->ServerName + "'s Server");
			}
			else if (ColumnName == "GameType")
			{
				ItemText = FText::FromString(Item->GameType);
			}
			else if (ColumnName == "Map")
			{
				ItemText = FText::FromString(Item->MapName);
			}
			else if (ColumnName == "Players")
			{
				ItemText = FText::Format(FText::FromString("{0}/{1}"), FText::FromString(Item->CurrentPlayers), FText::FromString(Item->MaxPlayers));
			}
			else if (ColumnName == "Ping")
			{
				ItemText = FText::FromString(Item->Ping);
			}
			return SNew(STextBlock)
				.Text(ItemText)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuServerListTextStyle");
		}
		TSharedPtr<FServerEntry> Item;
	};
	return SNew(SServerEntryWidget, OwnerTable, Item);
}

FReply SShooterSessionsWidget::OnBeginPlay() const
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::OnBeginPlay()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnBeginPlay( )")));

#if !SHOOTER_CONSOLE_UI
	if (GameInstance.IsValid())
	{
		GameInstance->SetOnlineMode(EOnlineMode::Online);
	}
#endif

	UWorld* const World = GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
	if (World && GetPlayerOwnerControllerId() != -1)
	{
		const FShooterMenuSoundsStyle& MenuSounds = FShooterStyle::Get().GetWidgetStyle<FShooterMenuSoundsStyle>("DefaultShooterMenuSoundsStyle");
		UShooterBlueprintLibrary::PlaySoundAndCall(World, MenuSounds.StartGameSound, GetPlayerOwnerControllerId(), this, &SShooterSessionsWidget::GameConnection);
	}
	return FReply::Handled();
}

FReply SShooterSessionsWidget::OnSessionRefresh()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::OnSessionRefresh()"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnBeginPlay( )")));
	if (GetPlayerOwnerControllerId() != -1)
	{
		BeginServerSearch(false, true, MapFilterName);
		//const FShooterMenuSoundsStyle& MenuSounds = FShooterStyle::Get().GetWidgetStyle<FShooterMenuSoundsStyle>("DefaultShooterMenuSoundsStyle");
		//UShooterBlueprintLibrary::PlaySoundAndCall(World, MenuSounds.StartGameSound, GetPlayerOwnerControllerId(), this, &SShooterSessionsWidget::GameConnection);
	}
	FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound, GetOwnerUserIndex());
	return FReply::Handled();
}

void SShooterSessionsWidget::GameConnection()
{
	UE_LOG(LogTemp, Warning, TEXT("SessionsWidget::GameConnection()"));
	if (ensure(GameInstance.IsValid()) && GetPlayerOwner() != NULL)
	{
		//AShooterGameSession* const GameSession = GameInstance->GetGameSession();
		//if (GameSession) 
		//{
		if (GameInstance->GetOnlineMode() == EOnlineMode::Offline)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnlineMode( Offline )"));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnlineMode( Offline )")));
		}
		else if (GameInstance->GetOnlineMode() == EOnlineMode::Online)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnlineMode( Online )"));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnlineMode( Online )")));
		}
		else if (GameInstance->GetOnlineMode() == EOnlineMode::LAN)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnlineMode( LAN )"));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnlineMode( LAN )")));
		}
		FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"), TEXT("Highrise"));
		/*
		FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s%s%s"),
			TEXT("Highrise"),
			*GameType,
			GameInstance->GetOnlineMode() != EOnlineMode::Offline ? TEXT("?listen") : TEXT(""),
			GameInstance->GetOnlineMode() == EOnlineMode::LAN ? TEXT("?bIsLanMatch") : TEXT("")
		);*/

		// Game instance will handle success, failure and dialogs
		//GameInstance->HostGame(GetPlayerOwner(), GameType, StartURL);
		//GameInstance->TravelLevel(StartURL);
		//APlayerController* PC = OwnerHUD->GetWorld()->GetFirstPlayerController();
		//FString URL = FString::Printf(TEXT("%s:%s?Alias=%s"), *GetSIP().ToString(), *GetSPort().ToString(), *GetNickName().ToString());

		AShooterPlayerController_Login* NewPC = Cast<AShooterPlayerController_Login>(OwnerHUD->GetWorld()->GetFirstPlayerController());
		if (NewPC)
		{
			NewPC->ClientGameStarted();
			NewPC->ClientStartOnlineGame();
		}
		//if (PC)
		//{
			//PC->ClientTravel(*URL, TRAVEL_Absolute);
		//}
	//}
	}
}

ULocalPlayer* SShooterSessionsWidget::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

int32 SShooterSessionsWidget::GetOwnerUserIndex()
{
	return PlayerOwner.IsValid() ? PlayerOwner->GetControllerId() : 0;
}

int32 SShooterSessionsWidget::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}

FSlateColor SShooterSessionsWidget::GetButtonBGColor() const
{
	return FLinearColor(0,0,0,0.5f);
}

FText SShooterSessionsWidget::GetScore() const
{
	// NOTE: THIS IS A TERRIBLE WAY TO DO THIS. DO NOT DO IT. IT ONLY WORKS ON SERVERS. USE GAME STATES INSTEAD!
	UWorld* World = OwnerHUD->GetWorld();
	//const AShooterGame_Menu* gameMode = World->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	//OwnerHUD->GetWorld()->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	AShooterGame_Menu* gameMode = Cast<AShooterGame_Menu>(World->GetAuthGameMode());

	if (gameMode == nullptr) {
		return FText::FromString(TEXT("SCORE: --"));
	}
	//gameMode->SpawnPawnActor();

	FString score = TEXT("SCORE: ");
	score.AppendInt(gameMode->GetScore());
	UE_LOG(LogTemp, Warning, TEXT("*********************%s"), *score);
	return FText::FromString(score);
}


#undef LOCTEXT_NAMESPACE

