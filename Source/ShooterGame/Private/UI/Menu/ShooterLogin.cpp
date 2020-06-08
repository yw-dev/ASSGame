// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterLogin.h"
#include "ShooterGameLoadingScreen.h"
#include "ShooterStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "ShooterGameInstance.h"
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
#include "Player/ShooterLocalPlayer.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Login"


// Sets default values
FShooterLogin::~FShooterLogin()
{
	auto Sessions = Online::GetSessionInterface();
	CleanupOnlinePrivilegeTask();
}

void FShooterLogin::Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner)
{
	check(_GameInstance.IsValid());
	auto Sessions = Online::GetSessionInterface();

	GameInstance = _GameInstance;
	PlayerOwner = _PlayerOwner;

	LoginWidget.Reset();
	LoginWidgetContainer.Reset();

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
		SAssignNew(LoginWidget, SShooterLoginWidget)
			.Cursor(EMouseCursor::Default)
			.PlayerOwner(GetPlayerOwner())
			.IsGameMenu(false);

		SAssignNew(LoginWidgetContainer, SWeakWidget)
			.PossiblyNullContent(LoginWidget);


	}


}

// Called every frame
void FShooterLogin::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

void FShooterLogin::AddMenuToGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* const GVC = GEngine->GameViewport;
		GVC->AddViewportWidgetContent(LoginWidgetContainer.ToSharedRef());
	}
}

void FShooterLogin::RemoveMenuFromGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(LoginWidgetContainer.ToSharedRef());
	}
}

void FShooterLogin::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	if (GameInstance.IsValid())
	{
		// Lock controls for the duration of the async task
		LoginWidget->LockControls(true);
		FUniqueNetIdRepl UserId;
		if (PlayerOwner.IsValid())
		{
			UserId = PlayerOwner->GetPreferredUniqueNetId();
		}
		GameInstance->StartOnlinePrivilegeTask(Delegate, EUserPrivileges::CanPlayOnline, UserId.GetUniqueNetId());
	}
}

void FShooterLogin::CleanupOnlinePrivilegeTask()
{
	if (GameInstance.IsValid())
	{
		GameInstance->CleanupOnlinePrivilegeTask();
	}
}

TStatId FShooterLogin::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FShooterLogin, STATGROUP_Tickables);
}

UWorld* FShooterLogin::GetTickableGameObjectWorld() const
{
	ULocalPlayer* LocalPlayerOwner = GetPlayerOwner();
	return (LocalPlayerOwner ? LocalPlayerOwner->GetWorld() : nullptr);
}

ULocalPlayer* FShooterLogin::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

int32 FShooterLogin::GetPlayerOwnerControllerId() const
{
	return (PlayerOwner.IsValid()) ? PlayerOwner->GetControllerId() : -1;
}




#undef LOCTEXT_NAMESPACE
