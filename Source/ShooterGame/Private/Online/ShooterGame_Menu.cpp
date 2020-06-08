// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterGame_Menu.h"
#include "ShooterMainMenu.h"
#include "ShooterWelcomeMenu.h"
#include "ShooterMessageMenu.h"
#include "ShooterMenuHUD.h"
#include "ShooterPlayerController_Menu.h"
#include "Player/ShooterSpectatorPawn.h"
#include "Player/ShooterDemoSpectator.h"
#include "Online/ShooterPlayerState.h"
#include "Online/ShooterGameSession.h"

AShooterGame_Menu::AShooterGame_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	//DefaultPawnClass = PlayerPawnOb.Class;

	//PlayerControllerClass = AShooterPlayerController_Menu::StaticClass();
	//PlayerControllerClass = AShooterPlayerController::StaticClass();
	//HUDClass = AShooterMenuHUD::StaticClass();
	//GameStateClass = AShooterGameState::StaticClass();
	//CurrentScore = 100;
	//PlayerStateClass = AShooterPlayerState::StaticClass();
	//SpectatorClass = AShooterSpectatorPawn::StaticClass();
	bUseSeamlessTravel = true;
}

void AShooterGame_Menu::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode::InitGame()"));
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AShooterGame_Menu::PreInitializeComponents()
{
	UE_LOG(LogTemp, Warning, TEXT("Game_Menu::PreInitializeComponents()"));
	Super::PreInitializeComponents();
}

void AShooterGame_Menu::RestartPlayer(class AController* NewPlayer)
{
	// don't restart
}

void AShooterGame_Menu::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

}

/** Returns game session class to use */
TSubclassOf<AGameSession> AShooterGame_Menu::GetGameSessionClass() const
{
	return AShooterGameSession::StaticClass();
}

void AShooterGame_Menu::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("Game_Menu::PreLogin()"));
	// GameSession can be NULL if the match is over
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void AShooterGame_Menu::PostLogin(APlayerController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Game_Menu::PostLogin()"));
	Super::PostLogin(NewPlayer);
	/*
	// update spectator location for client
	AShooterPlayerController_Menu* NewPC = Cast<AShooterPlayerController_Menu>(NewPlayer);
	if (NewPC && NewPC->GetPawn() == NULL)
	{
		//NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
	}

	// notify new player if match is already in progress
	if (NewPC)
	{
		NewPC->ClientGameStarted();
		NewPC->ClientStartOnlineGame();
	}*/
}

bool AShooterGame_Menu::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

UClass* AShooterGame_Menu::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->IsA<AShooterPlayerController_Menu>())
	{
		return DefaultPawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* AShooterGame_Menu::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* TestSpawn = *It;
		if (TestSpawn->PlayerStartTag.Compare(FName(TEXT("PawnPreview"))))
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

void AShooterGame_Menu::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	//CreateBotControllers();
}

void AShooterGame_Menu::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// checking number of existing human player.
	UWorld* World = GetWorld();
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		AShooterPlayerController_Menu* AIC = Cast<AShooterPlayerController_Menu>(*It);
		if (AIC)
		{
			RestartPlayer(AIC);
			AIC->ClientGameStarted();
		}
	}
}

void AShooterGame_Menu::SpawnPawnActor()
{
	UWorld* World = GetWorld();
	AShooterPlayerController_Menu* ACM = Cast<AShooterPlayerController_Menu>(World->GetFirstPlayerController());

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = nullptr;

	AShooterCharacter* AIC = World->SpawnActor<AShooterCharacter>(SpawnInfo);
	if (AIC->PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game_Menu::PostLogin()"));
		//FString BotName = FString::Printf(TEXT("Bot %d"), BotNum);
		//AIC->PlayerState->SetPlayerName(BotName);
	}
}

int32 AShooterGame_Menu::GetScore()
{
	return CurrentScore;
}

void AShooterGame_Menu::AddPoints(int32 value)
{
	if (value > 0) {
		CurrentScore += value;
	}
}

void AShooterGame_Menu::DeductPoints(int32 value)
{
	if (value > 0) {
		CurrentScore = FMath::Max(CurrentScore - value, 0);
	}
}