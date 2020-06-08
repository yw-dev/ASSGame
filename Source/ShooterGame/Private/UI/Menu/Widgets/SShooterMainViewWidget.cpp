// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SShooterMainViewWidget.h"
#include "SShooterSessionsWidget.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.LoginHUD"


// Sets default values
void SShooterMainViewWidget::Construct(const FArguments& args)
{
	OwnerHUD = args._OwnerHUD;
	PlayerOwner = args._PlayerOwner;

	GameInstance = Cast<UShooterGameInstance>(OwnerHUD->GetGameInstance());
	PlayerOwner = OwnerHUD->GetGameInstance()->GetFirstGamePlayer();

	const FSlateFontInfo PlayerNameFontInfo = FShooterStyle::Get().GetWidgetStyle<FTextBlockStyle>("ShooterGame.MenuTextStyle").Font;

	// Layout
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FLinearColor(0.02f, 0.04f, 0.08f, 0.25f))
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
					.HeightOverride(120)
					[
						SNew(SImage)
						.ColorAndOpacity(FLinearColor(0.02f, 0.04f, 0.08f, 0.35f))
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(1, -1, 1, 1)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				.AutoWidth()
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(20)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.HeightOverride(100)
							.WidthOverride(100)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("Play"))
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.HeightOverride(100)
							.WidthOverride(100)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("Join"))
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.HeightOverride(100)
							.WidthOverride(100)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("Option"))
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.HeightOverride(100)
							.WidthOverride(100)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("Exit"))
							]
						]

					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(-1, -1, 1, 1)
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(-2, -2, -2, 1)
						[
							SNew(SBorder)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("Session View"))
								//SAssignNew(SessionView, SShooterSessionsWidget)
								//.OwnerHUD(OwnerHUD)
								//SAssignNew(ContentBox, SVerticalBox)
								//.Clipping(EWidgetClipping::ClipToBounds)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						.Padding(20)
						.AutoHeight()
						[
							SNew(SBox)
							.HeightOverride(80)
							.WidthOverride(200)
							.Padding(10)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
								.OnClicked(this, &SShooterMainViewWidget::OnStartGame)
								.Text(FText::FromString("StartPlay"))
							]
						]
					]
				]
			]
		]
	];
}

SShooterMainViewWidget::~SShooterMainViewWidget()
{

}

void SShooterMainViewWidget::BuildAndShowMenu()
{
	ContentBox->ClearChildren();

	TSharedPtr<SWidget> ContentView;
	//Custom menu items are not supported in the right panel
	ContentView = SAssignNew(ContentView, SShooterSessionsWidget)
		.PlayerOwner(OwnerHUD->GetGameInstance()->GetFirstGamePlayer());
	if (ContentView.IsValid())
	{
		ContentBox->AddSlot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				ContentView.ToSharedRef()
			];
	}
}

void SShooterMainViewWidget::OnIPTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	SIP = NewText;
}

void SShooterMainViewWidget::OnIPTextChanged(const FText& NewText)
{
	SIP = NewText;
}

void SShooterMainViewWidget::OnPortTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	SPort = NewText;
}

void SShooterMainViewWidget::OnPortTextChanged(const FText& NewText)
{
	SPort = NewText;
}

void SShooterMainViewWidget::OnNickNameTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	NickName = NewText;
}

void SShooterMainViewWidget::OnNickNameTextChanged(const FText& NewText)
{
	NickName = NewText;
}

FReply SShooterMainViewWidget::OnStartGame() const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnStartGame( )")));

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
		UShooterBlueprintLibrary::PlaySoundAndCall(World, MenuSounds.StartGameSound, GetPlayerOwnerControllerId(), this, &SShooterMainViewWidget::StartGame);
	}
	return FReply::Handled();
}

void SShooterMainViewWidget::StartGame()
{
	APlayerController* PC = OwnerHUD->GetWorld()->GetFirstPlayerController();
	//FString URL = FString::Printf(TEXT("%s:%s?Alias=%s"), *GetSIP().ToString(), *GetSPort().ToString(), *GetNickName().ToString());
	FString StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"), TEXT("Highrise"), TEXT("FFA"));
	if (PC)
	{
		PC->ClientTravel(*StartURL, TRAVEL_Relative, true);
	}
	HostGame(LOCTEXT("FFA", "FFA").ToString());
}

void SShooterMainViewWidget::HostGame(const FString& GameType)
{
	if (ensure(GameInstance.IsValid()) && GetPlayerOwner() != NULL)
	{
		FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"),
			TEXT("Highrise"),
			*GameType);

		// Game instance will handle success, failure and dialogs
		//GameInstance->HostGame(GetPlayerOwner(), GameType, StartURL);
		//GameInstance->TravelLevel(StartURL);		
	}
}

ULocalPlayer* SShooterMainViewWidget::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

int32 SShooterMainViewWidget::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}

FText SShooterMainViewWidget::GetSIP() const
{
	return SIP;
}

FText SShooterMainViewWidget::GetSPort() const
{
	return SPort;
}

FText SShooterMainViewWidget::GetNickName() const
{
	return NickName;
}



#undef LOCTEXT_NAMESPACE