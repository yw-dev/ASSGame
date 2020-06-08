// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterLoginHUD.h"




AShooterLoginHUD::AShooterLoginHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AShooterLoginHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	/*
	GetOwningPlayerController()->bShowMouseCursor = true;
	//PlayerOwner->bEnableMouseOverEvents = true;
	GetOwningPlayerController()->bEnableClickEvents = true;

	if (GEngine && GEngine->GameViewport)
	{
		SAssignNew(ConnectionWidgetContainer, SShooterConnectionWidget).OwnerHUD(TWeakObjectPtr<AShooterLoginHUD>(this));

		//SAssignNew(SessionWidgetContainer, SShooterSessionsWidget).OwnerHUD(TWeakObjectPtr<AShooterLoginHUD>(this));

		SAssignNew(MainViewContainer, SShooterMainViewWidget).OwnerHUD(TWeakObjectPtr<AShooterLoginHUD>(this));
	}

	if (GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(NM_DedicatedServer)"));
	}
	else if (GetNetMode() == NM_Standalone)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(NM_Standalone)"));
	}
	else if (GetNetMode() == NM_ListenServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(NM_ListenServer)"));
	}
	else if (GetNetMode() == NM_Client)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(NM_Client)"));
	}

	UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

	if (SGI->GetOnlineMode() == EOnlineMode::Online)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(Online)"));
	}
	else if (SGI->GetOnlineMode() == EOnlineMode::LAN)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(LAN)"));
	}
	else if (SGI->GetOnlineMode() == EOnlineMode::Offline)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInit(Offline)"));
	}
	
	// net mode
	if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online) && GetNetMode() > NM_Standalone)
	{
		FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");

		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInitialize( %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));

		IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem)
		{
			IOnlineSessionPtr SessionSubsystem = OnlineSubsystem->GetSessionInterface();
			if (SessionSubsystem.IsValid())
			{
				FNamedOnlineSession * Session = SessionSubsystem->GetNamedSession(NAME_GameSession);
				if (Session)
				{
					GEngine->GameViewport->RemoveViewportWidgetContent(ConnectionWidgetContainer.ToSharedRef());
					NetModeDesc += TEXT("\nSession: ");
					NetModeDesc += Session->SessionInfo->GetSessionId().ToString();
					UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInitialize( Session has created. - %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));
					GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MainViewContainer.ToSharedRef()));
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("LoginHUD::PostInitialize( %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));
	}
	else 
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(ConnectionWidgetContainer.ToSharedRef()));
	}
	GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MainViewContainer.ToSharedRef()));*/

}


void AShooterLoginHUD::UpdateViewContainer()
{
	GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(ConnectionWidgetContainer.ToSharedRef()));
}



