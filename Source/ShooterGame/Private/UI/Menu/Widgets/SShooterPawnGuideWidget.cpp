// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"
#include "Player/ShooterCharacter.h"
#include "Player/ShooterPlayerController_Menu.h"
#include "SShooterPawnGuideWidget.h"

#define LOCTEXT_NAMESPACE "SShooterPawnGuideWidget"

class UShooterBlueprintLibrary;
class UShooterGameUserSettings;
class UAssetManager;

// Sets default values
void SShooterPawnGuideWidget::Construct(const FArguments& args)
{
	//OwnerHUD = args._OwnerHUD;

	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	PlayerOwner = args._PlayerOwner;
	OwnerWidget = args._OwnerWidget;


	GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	//PlayerOwner = OwnerHUD->GetGameInstance()->GetFirstGamePlayer();

	ViewModel = FShooterPawnSourceViewModel::CreateShared();
	ViewModel->SetOnCategoriesChanged(FShooterPawnSourceViewModel::FOnCategoriesChanged::CreateSP(
		this, &SShooterPawnGuideWidget::CategoriesChanged));
	ViewModel->SetOnContentSourcesChanged(FShooterPawnSourceViewModel::FOnContentSourcesChanged::CreateSP(
		this, &SShooterPawnGuideWidget::ContentSourcesChanged));
	ViewModel->SetOnSelectedContentSourceChanged(FShooterPawnSourceViewModel::FOnSelectedContentSourceChanged::CreateSP(
		this, &SShooterPawnGuideWidget::SelectedContentSourceChanged));

	// read user settings
#if SHOOTER_CONSOLE_UI
	bIsLanMatch = FParse::Param(FCommandLine::Get(), TEXT("forcelan"));
#else
	//UShooterGameUserSettings* const UserSettings = CastChecked<UShooterGameUserSettings>(GEngine->GetGameUserSettings());
	//bIsLanMatch = UserSettings->IsLanMatch();
	//bIsDedicatedServer = UserSettings->IsDedicatedServer();
	bIsLanMatch = false;
	OutlineWidth = 20.0f;
	bIsDedicatedServer = true;
#endif

	//HUDStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");
	//SShooterPawnGuideWidget::FOnSelectedContentSourceChanged::CreateSP(this, &SShooterPawnGuideWidget::SelectedContentSourceChanged)
	// Slate Attribute binding.
	Score.Bind(this, &SShooterPawnGuideWidget::GetScore);
	Health.Bind(this, &SShooterPawnGuideWidget::GetHealth);

	// Fill view source.
	//RebuildViewSource();

	// Layout
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.Padding(FMargin(OutlineWidth))
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
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
					//SAssignNew(CategoryTileView, SBox)
					//[
					//	CreateCategoryTabs()
					//]
					CreateCategoryTileView()
				]
			]
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(OutlineWidth))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0.f, 50.f)
			[
				SNew(SBorder)
				.BorderImage(&MenuStyle->RightBackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.Padding(FMargin(OutlineWidth))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					CreateContentTileView()
				]
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				[
					SNew(SBox)
					.HeightOverride(60)
					.WidthOverride(140)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f, 1.f))
						.Padding(FMargin(OutlineWidth/2))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ButtonColorAndOpacity(FSlateColor(FLinearColor::Transparent))
							.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuButtonTextStyle")
							.OnClicked(this, &SShooterPawnGuideWidget::OnBeginPlay)
							.Text(LOCTEXT("Play", "开始游戏"))
						]
					]
				]
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0.f, 100.f, 0.f, 100.f)
			[
				SAssignNew(ContentDetailView, SBox)
				.WidthOverride(450)
				[
					CreateContentSourceDetail(ViewModel->GetSelectedContent())
				]
			]
		]
	];
}
SShooterPawnGuideWidget::~SShooterPawnGuideWidget()
{

}

TSharedRef<SWidget> SShooterPawnGuideWidget::CreateCategoryTileView()
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuideWidget::CreateCategoryTileView()"));
	SAssignNew(CategoryTileView, SShooterTileView)
		.ListItemsSource(ViewModel->GetCategories())
		.OnGenerateTile(this, &SShooterPawnGuideWidget::Category_OnGenerateTile)
		.OnSelectionChanged(this, &SShooterPawnGuideWidget::Category_OnSelectionChanged)
		.ItemWidth(64)
		.ItemHeight(64)
		.SelectionMode(ESelectionMode::Single);
	CategoryTileView->SetSelection(ViewModel->GetSelectedCategory(), ESelectInfo::Direct);
	return CategoryTileView.ToSharedRef();
}

TSharedRef<ITableRow> SShooterPawnGuideWidget::Category_OnGenerateTile(FShooterPawnItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(STableRow<FShooterPawnItemPtr>, OwnerTable)
			[
				SNew(STextBlock)
				.Text(FText::FromString("THIS WAS NULL SOMEHOW"))
			];
	}
	return SNew(STableRow<FShooterPawnItemPtr>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Item->GetName()))
			//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
			.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
			.ShadowColorAndOpacity(FLinearColor::Black)
			.ShadowOffset(FIntPoint(-2, 2))
		];
}

void SShooterPawnGuideWidget::Category_OnSelectionChanged(FShooterPawnItemPtr SelectedCategory, ESelectInfo::Type SelectInfo)
{
	//SetCurrentCategory(SelectedCategory);
	//UE_LOG(LogTemp, Warning, TEXT("Item Selected: %s"), *SelectedCategory->GetDisplayName());
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Category_OnSelectionChanged( %s )"), *SelectedCategory->GetName()));
	/*if (CurrentCategory->GetName() != SelectedCategory->GetName())
	{
		CurrentCategory = SelectedCategory;
		//const auto& SubCategories = SelectedCategory->GetSubDirectories();
		SetContentSource(SelectedCategory->AccessSubDirectories());
	}*/
	ViewModel->SetSelectedCategory(SelectedCategory);
}

void SShooterPawnGuideWidget::SetContentSource(TArray<FShooterPawnItemPtr> InContentSource)
{
	ContentSource = InContentSource;
}

TSharedRef<SWidget> SShooterPawnGuideWidget::CreateContentTileView()
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuideWidget::CreateContentTileView()"));
	SAssignNew(ContentTileView, SShooterTileView)
		.ListItemsSource(ViewModel->GetContentSources())
		.OnGenerateTile(this, &SShooterPawnGuideWidget::Content_OnGenerateTile)
		.OnSelectionChanged(this, &SShooterPawnGuideWidget::Content_OnSelectionChanged)
		.ItemWidth(64)
		.ItemHeight(64)
		.SelectionMode(ESelectionMode::Single);
	ContentTileView->SetSelection(ViewModel->GetSelectedContent(), ESelectInfo::Direct);
	//ContentTileView->RequestListRefresh();
	return ContentTileView.ToSharedRef();
}

TSharedRef<ITableRow> SShooterPawnGuideWidget::Content_OnGenerateTile(FShooterPawnItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(STableRow<FShooterPawnItemPtr>, OwnerTable)
			[
				SNew(STextBlock)
				.Text(FText::FromString("THIS WAS NULL SOMEHOW"))
			];
	}
	return SNew(STableRow<FShooterPawnItemPtr>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Item->GetName()))
			//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
			.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
			.ShadowColorAndOpacity(FLinearColor::Black)
			.ShadowOffset(FIntPoint(-2, 2))
		];
}

void SShooterPawnGuideWidget::Content_OnSelectionChanged(FShooterPawnItemPtr SelectedCategory, ESelectInfo::Type SelectInfo)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Category_OnSelectionChanged( %s )"), *SelectedCategory->GetName()));
	/*if (CurrentContent->GetName() != SelectedCategory->GetName())
	{
		CurrentContent = SelectedCategory;
		//const auto& SubCategories = SelectedCategory->GetSubDirectories();
		//SetContentSource(SelectedCategory->AccessSubDirectories());
	}*/
	ViewModel->SetSelectedContent(SelectedCategory);
}

TSharedRef<SWidget> SShooterPawnGuideWidget::CreateContentSourceDetail(FShooterPawnItemPtr ContentItem)
{
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);
	if (ContentItem.IsValid())
	{
		VerticalBox->AddSlot()
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
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					.HAlign(EHorizontalAlignment::HAlign_Left)
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]

					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						//.TextStyle(FAddContentDialogStyle::Get(), "AddContentDialog.HeadingText")
						.Text(FText::FromString(ContentItem->GetName()))
						.AutoWrapText(true)
					]

					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						.AutoWrapText(true)
					]

					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]

					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
					+ SScrollBox::Slot()
					.Padding(FMargin(0, 0, 0, 5))
					[
						SNew(STextBlock)
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 10, 0, 0)
				.HAlign(HAlign_Right)
				[
					SNew(SButton)
					//.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
					.OnClicked(this, &SShooterPawnGuideWidget::AddButtonClicked)
					.ContentPadding(FMargin(5, 5, 5, 5))
					.HAlign(EHorizontalAlignment::HAlign_Center)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.AutoWidth()
						.Padding(0, 0, 2, 0)
						[
							SNew(STextBlock)
							.Text(FText::FromString(ContentItem->GetName()))
							//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
							.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
							.ShadowColorAndOpacity(FLinearColor::Black)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(ContentItem->GetName()))
							//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
							.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
							.ShadowColorAndOpacity(FLinearColor::Black)
						]
					]
				]
			]
		];
	}
	return VerticalBox;
}

FReply SShooterPawnGuideWidget::AddButtonClicked()
{
	/*if (ViewModel->GetSelectedContent().IsValid())
	{
		ViewModel->GetSelectedContent()->GetContentSource()->InstallToProject("/Game");
	}*/
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ButtonClicked( %s )"), *ViewModel->GetSelectedContent()->GetName()));
	return FReply::Handled();
}

void SShooterPawnGuideWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// Call parent implementation
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	//can do things here every tick
}

void SShooterPawnGuideWidget::CategoriesChanged()
{
	CategoryTileView->RequestListRefresh();
}

void SShooterPawnGuideWidget::ContentSourcesChanged()
{
	ContentTileView->RequestListRefresh();
}

void SShooterPawnGuideWidget::SelectedContentSourceChanged()
{
	//ContentTileView->SetSelection(ViewModel->GetSelectedContent(), ESelectInfo::Direct);
	FShooterPawnItemPtr Item = ViewModel->GetSelectedContent();
	ContentDetailView->SetContent(CreateContentSourceDetail(ViewModel->GetSelectedContent()));

	//AShooterPlayerController_Menu* PCM = Cast<AShooterPlayerController_Menu>(OwnerHUD->GetOwningPlayerController());
	// PCM 调用 Spawn  character 方法
	if (Item)
	{
		//SpawnPawnActor(Item);
	}
}

FReply SShooterPawnGuideWidget::OnBeginPlay() const
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnBeginPlay( %s )"), *ViewModel->GetSelectedContent()->GetName()));
/*#if WITH_EDITOR
	if (GIsEditor == true)
	{
		return;
	}
#endif
	if (!IsMapReady())
	{
		return;
	}
	*/
#if !SHOOTER_CONSOLE_UI
	if (GameInstance.IsValid())
	{
		GameInstance->SetOnlineMode(bIsLanMatch ? EOnlineMode::LAN : EOnlineMode::Online);
	}
#endif

	//MenuWidget->LockControls(true);
	//MenuWidget->HideMenu();

	UWorld* const World = GameInstance.IsValid() ? GameInstance->GetWorld() : nullptr;
	if (World && GetPlayerOwnerControllerId() != -1)
	{
		const FShooterMenuSoundsStyle& MenuSounds = FShooterStyle::Get().GetWidgetStyle<FShooterMenuSoundsStyle>("DefaultShooterMenuSoundsStyle");
		UShooterBlueprintLibrary::PlaySoundAndCall(World, MenuSounds.StartGameSound, GetPlayerOwnerControllerId(), this, &SShooterPawnGuideWidget::HostTeamDeathMatch);
	}

	return FReply::Handled();
}

void SShooterPawnGuideWidget::HostTeamDeathMatch()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("HostTeamDeathMatch( %s )"), *ViewModel->GetSelectedContent()->GetName()));
	//HostGame(LOCTEXT("TDM", "TDM").ToString());
	APlayerController* PC = PlayerOwner->GetWorld()->GetFirstPlayerController();
	//FString URL = FString::Printf(TEXT("%s:%s?Alias=%s"), *GetSIP().ToString(), *GetSPort().ToString(), *GetNickName().ToString());
	FString StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"), TEXT("Highrise"), TEXT("TDM"));
	if (PC)
	{
		PC->ClientTravel(*StartURL, TRAVEL_Relative, true);
	}
}

void SShooterPawnGuideWidget::HostGame(const FString& GameType)
{				
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("PawnGuideWidget::HostGame( %s )"), &GameType));
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
			FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"), TEXT("Highrise"), *GameType);
			/*
			FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s%s%s"),
				TEXT("Highrise"),
				*GameType,
				GameInstance->GetOnlineMode() != EOnlineMode::Offline ? TEXT("?listen") : TEXT(""),
				GameInstance->GetOnlineMode() == EOnlineMode::LAN ? TEXT("?bIsLanMatch") : TEXT("")
			);*/

			// Game instance will handle success, failure and dialogs
			GameInstance->HostGame(GetPlayerOwner(), GameType, StartURL);
			//GameInstance->TravelLevel(StartURL);
		//}
	}
}

bool SShooterPawnGuideWidget::IsMapReady() const
{
	bool bReady = true;
	/*IPlatformChunkInstall* ChunkInstaller = FPlatformMisc::GetPlatformChunkInstall();
	if (ChunkInstaller)
	{
		EMap SelectedMap = EMap::EHighRise;
		// should use the AssetRegistry as soon as maps are added to the AssetRegistry
		int32 MapChunk = ChunkMapping[(int)SelectedMap];
		EChunkLocation::Type ChunkLocation = ChunkInstaller->GetChunkLocation(MapChunk);
		if (ChunkLocation == EChunkLocation::NotAvailable)
		{
			bReady = false;
		}
	}*/
	return bReady;
}

void SShooterPawnGuideWidget::SpawnPawnActor(FShooterPawnItemPtr Item)
{
	UE_LOG(LogTemp, Warning, TEXT("PawnGuideWidget::SpawnPawnActor()"));
	UWorld* World = PlayerOwner->GetWorld();

	if (PreviewActor.IsSet())
	{
		if (!PreviewActor.Get()->IsPendingKillPending())
		{
			PreviewActor.Get()->Destroy();
		}
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//UAssetManager::GetStreamableManager().RequestAsyncLoad(TEXT("/Game/BluePrints/Pawns/HeroA"), FStreamableDelegate::CreateUObject(this, &SShooterPawnGuideWidget::LoadActorResCallBack), 0, true);
	FString path = Item->GetPawn();
	//FName text = FName(*Item->GetPawn());
	//TCHAR path = *str;
	UClass* PawnClass = LoadClass<AActor>(NULL, *path);
	//UObject* loadObj = StaticLoadObject(UBlueprint::StaticClass(), NULL, TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA'"));
	if (PawnClass != nullptr)
	{
		const FVector& Loc = FVector(-1346.0, 386.0, 126.0);
		const FRotator& Rot = FRotator(0, 180, 0);

		//AShooterCharacter* spawnActor = World->SpawnActor<AShooterCharacter>(ubp->GetClass(), Loc, Rot, SpawnInfo);
		AShooterCharacter* spawnActor = UShooterBlueprintLibrary::SpawnActor<AShooterCharacter>(PlayerOwner->GetWorld(), PawnClass, Loc, Rot);
		
		if (spawnActor != nullptr)
		{
			PreviewActor = spawnActor;
			AShooterCharacter* SC = spawnActor;
			SC->GetVelocity();
		}
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

void SShooterPawnGuideWidget::LoadActorResCallBack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("ActorRes Load Succeed!"));
}

ULocalPlayer* SShooterPawnGuideWidget::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

int32 SShooterPawnGuideWidget::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}

FText SShooterPawnGuideWidget::GetScore() const 
{
	// NOTE: THIS IS A TERRIBLE WAY TO DO THIS. DO NOT DO IT. IT ONLY WORKS ON SERVERS. USE GAME STATES INSTEAD!
	UWorld* World = PlayerOwner->GetWorld();
	//const AShooterGame_Menu* gameMode = World->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	//PlayerOwner->GetWorld()->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	AShooterGame_Menu* gameMode = Cast<AShooterGame_Menu>(World->GetAuthGameMode());

	if (gameMode == nullptr) {
		return FText::FromString(TEXT("SCORE: --"));
	}
	//gameMode->SpawnPawnActor();

	FString score = TEXT("SCORE: ");
	score.AppendInt(gameMode->GetScore());
	//UE_LOG(LogTemp, Warning, TEXT("*********************%s"), *score);
	return FText::FromString(score);
}

FText SShooterPawnGuideWidget::GetHealth() const 
{
	//APlayerController* AC = PlayerOwner->GetOwningPlayerController();
	AShooterPlayerController_Menu* ACM = Cast<AShooterPlayerController_Menu>(PlayerOwner->GetPlayerController(PlayerOwner->GetWorld()));
	//FShooterPawnItemPtr ItemPtr = ViewModel->GetSelectedContent();
	//AShooterCharacter* Character = Cast<AShooterCharacter>(ACM->GetPawn());
	//AShooterCharacter* MyPawn = (OwnerHUD->GetOwningPlayerController()) ? Cast<AShooterCharacter>(OwnerHUD->GetOwningPlayerController()->GetCharacter()) : NULL;

	if (ACM == nullptr) {
		return FText::FromString(TEXT("HEALTH: --"));
	}
	FString health = TEXT("HEALTH: ");
	health.AppendInt(ACM->Health);
	//health.Append(ItemPtr->GetName());

	return FText::FromString(health);
}

/*
AActor* SShooterPawnGuideWidget::GetPreviewPoint() const
{
	// NOTE: THIS IS A TERRIBLE WAY TO DO THIS. DO NOT DO IT. IT ONLY WORKS ON SERVERS. USE GAME STATES INSTEAD!
	UWorld* World = OwnerHUD->GetWorld();
	//const AShooterGame_Menu* gameMode = World->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	//OwnerHUD->GetWorld()->GetGameState()->GetDefaultGameMode<AShooterGame_Menu>();
	AShooterGame_Menu* gameMode = Cast<AShooterGame_Menu>(World->GetAuthGameMode());

	//APlayerStart* BestStart = NULL;
	if (gameMode == nullptr) {
		return NULL;
	}

	//APlayerStart score = TEXT("SCORE: ");
	//BestStart = gameMode->ChoosePlayerStart(OwnerHUD->GetOwningPlayerController());
	//UE_LOG(LogTemp, Warning, TEXT("*********************%s"), *score);
	return gameMode->ChoosePlayerStart(OwnerHUD->GetOwningPlayerController());
}
*/

#undef LOCTEXT_NAMESPACE