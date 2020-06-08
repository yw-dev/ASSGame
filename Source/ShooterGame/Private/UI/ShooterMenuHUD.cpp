// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterMenuHUD.h"


AShooterMenuHUD::AShooterMenuHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AShooterMenuHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetOwningPlayerController()->bShowMouseCursor = true;
	//PlayerOwner->bEnableMouseOverEvents = true;
	GetOwningPlayerController()->bEnableClickEvents = true;

	// net mode
	if (GetNetMode() != NM_Standalone)
	{
		FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");

		UE_LOG(LogTemp, Warning, TEXT("MenuHUD::PostInitialize( %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));

		IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem)
		{
			IOnlineSessionPtr SessionSubsystem = OnlineSubsystem->GetSessionInterface();
			if (SessionSubsystem.IsValid())
			{
				FNamedOnlineSession * Session = SessionSubsystem->GetNamedSession(NAME_GameSession);
				if (Session)
				{
					NetModeDesc += TEXT("\nSession: ");
					NetModeDesc += Session->SessionInfo->GetSessionId().ToString();
					UE_LOG(LogTemp, Warning, TEXT("MenuHUD::PostInitialize( Session has created. - %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("MenuHUD::PostInitialize( %s )"), *FString::Printf(TEXT("NetMode : %s "), *NetModeDesc));
	}

	if (GEngine && GEngine->GameViewport)
	{
		//UGameViewportClient* Viewport = GEngine->GameViewport;

		//SAssignNew(MenuWidgetContainer, SShooterPawnGuideWidget).PlayerOwner(PlayerOwner);
		
		//GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MenuWidgetContainer.ToSharedRef()));
	}
}


void AShooterMenuHUD::SetMatchState(EShooterMatchState::Type NewState)
{
	MatchState = NewState;
}

EShooterMatchState::Type AShooterMenuHUD::GetMatchState() const
{
	return MatchState;
}


