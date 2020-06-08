// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "SShooterConnectionWidget.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"


#define LOCTEXT_NAMESPACE "ShooterGame.HUD.LoginHUD"

// Sets default values
void SShooterConnectionWidget::Construct(const FArguments& args)
{
	OwnerHUD = args._OwnerHUD;

	GameInstance = Cast<UShooterGameInstance>(OwnerHUD->GetGameInstance());
	PlayerOwner = OwnerHUD->GetGameInstance()->GetFirstGamePlayer();

	const FSlateFontInfo PlayerNameFontInfo = FShooterStyle::Get().GetWidgetStyle<FTextBlockStyle>("ShooterGame.MenuTextStyle").Font;

	//HUDStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");

	// Slate Attribute binding.
	//SIP.Bind(this, &SShooterConnectionWidget::GetSIP);
	//SPort.Bind(this, &SShooterConnectionWidget::GetSPort);
	//NickName.Bind(this, &SShooterConnectionWidget::GetNickName);

	// Fill view source.
	//RebuildViewSource();

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
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(10)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(280)
					.HeightOverride(80)
					.Padding(10)
					[
						SNew(SEditableTextBox)
						.Font(PlayerNameFontInfo)
						.HintText(FText::FromString(TEXT("127.0.0.1")))
						.Text(this, &SShooterConnectionWidget::GetSIP)
						.OnTextCommitted(this, &SShooterConnectionWidget::OnIPTextCommitted)
						.OnTextChanged(this, &SShooterConnectionWidget::OnIPTextChanged)
						.SelectAllTextWhenFocused(true)
						.SelectAllTextOnCommit(true)
						.ForegroundColor(FLinearColor(0.02f, 0.04f, 0.08f, 1.f))
						.ClearKeyboardFocusOnCommit(false)
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.MaxWidth(120)
				[
					SNew(SBox)
					.WidthOverride(120)
					.HeightOverride(80)
					.Padding(10)
					[
						SNew(SEditableTextBox)
						.Font(PlayerNameFontInfo)
						.HintText(FText::FromString(TEXT("7777")))
						.Text(this, &SShooterConnectionWidget::GetSPort)
						.OnTextCommitted(this, &SShooterConnectionWidget::OnPortTextCommitted)
						.OnTextChanged(this, &SShooterConnectionWidget::OnPortTextChanged)
						.SelectAllTextWhenFocused(true)
						.SelectAllTextOnCommit(true)
						.ForegroundColor(FLinearColor(0.02f, 0.04f, 0.08f, 1.f))
						.ClearKeyboardFocusOnCommit(false)
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.WidthOverride(400)
				.HeightOverride(80)
				.Padding(10)
				[
					SNew(SEditableTextBox)
					.Font(PlayerNameFontInfo)
					.HintText(FText::FromString(TEXT("请输入昵称.")))
					.Text(this, &SShooterConnectionWidget::GetNickName)
					.OnTextCommitted(this, &SShooterConnectionWidget::OnNickNameTextCommitted)
					.OnTextChanged(this, &SShooterConnectionWidget::OnNickNameTextChanged)
					.SelectAllTextWhenFocused(true)
					.SelectAllTextOnCommit(true)
					.ForegroundColor(FLinearColor(0.02f, 0.04f, 0.08f, 1.f))
					.ClearKeyboardFocusOnCommit(false)
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HeightOverride(80)
				.Padding(10)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuProfileNameStyle")
					.OnClicked(this, &SShooterConnectionWidget::OnConnectionServer)
					.Text(FText::FromString("Connection"))
				]
			]
		]
	];
}

SShooterConnectionWidget::~SShooterConnectionWidget()
{

}


void SShooterConnectionWidget::OnIPTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	SIP = NewText;
}

void SShooterConnectionWidget::OnIPTextChanged(const FText& NewText)
{
	SIP = NewText;
}

void SShooterConnectionWidget::OnPortTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	SPort = NewText;
}

void SShooterConnectionWidget::OnPortTextChanged(const FText& NewText)
{
	SPort = NewText;
}

void SShooterConnectionWidget::OnNickNameTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnEnter)
	{
		TArray<FAssetIdentifier> NewPaths;
		NewPaths.Add(FAssetIdentifier::FromString(NewText.ToString()));
		//SetGraphRootIdentifiers(NewPaths);
	}

	NickName = NewText;
}

void SShooterConnectionWidget::OnNickNameTextChanged(const FText& NewText)
{
	NickName = NewText;
}

FReply SShooterConnectionWidget::OnConnectionServer() const
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("OnConnectionServer( )")));

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
		UShooterBlueprintLibrary::PlaySoundAndCall(World, MenuSounds.StartGameSound, GetPlayerOwnerControllerId(), this, &SShooterConnectionWidget::ServerConnection);
	}
	return FReply::Handled();
}

void SShooterConnectionWidget::ServerConnection()
{
	APlayerController* PC = OwnerHUD->GetWorld()->GetFirstPlayerController();
	FString URL = FString::Printf(TEXT("%s:%s?Alias=%s"), *GetSIP().ToString(), *GetSPort().ToString(), *GetNickName().ToString());
	if (PC)
	{
		PC->ClientTravel(*URL, TRAVEL_Absolute);
	}
}

void SShooterConnectionWidget::ConnectionServerByMode(const FString& GameType)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ServerConnection( %s )"), &GameType));
	APlayerController* PC = OwnerHUD->GetWorld()->GetFirstPlayerController();
	FString URL = FString::Printf(TEXT("%s:%s?Alias=%s"), *GetSIP().ToString(), *GetSPort().ToString(), *GetNickName().ToString());
	if (PC)
	{
		PC->ClientTravel(*URL, TRAVEL_Absolute);
	}
}

ULocalPlayer* SShooterConnectionWidget::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

int32 SShooterConnectionWidget::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}

FText SShooterConnectionWidget::GetSIP() const
{
	return SIP;
}

FText SShooterConnectionWidget::GetSPort() const
{
	return SPort;
}

FText SShooterConnectionWidget::GetNickName() const
{
	return NickName;
}



#undef LOCTEXT_NAMESPACE