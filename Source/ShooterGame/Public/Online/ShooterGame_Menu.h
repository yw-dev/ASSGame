// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterGame_Menu.generated.h"

UCLASS()
class AShooterGame_Menu : public AGameMode
{
	GENERATED_UCLASS_BODY()

public:

	/**
	 * Retrieves the current Score from the game mode.
	 **/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Score")
	int32 GetScore();

	/**
	 * Adds to the game score.
	 **/
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddPoints(int32 value);

	/**
	 * Removes from the game score.
	 **/
	UFUNCTION(BlueprintCallable, Category = "Score")
	void DeductPoints(int32 value);

	// Begin AGameModeBase interface
	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** Returns game session class to use */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
	// End AGameModeBase interface

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	void SpawnPawnActor();

protected:

	/**
	 * Stores the current score.
	 **/
	int32 CurrentScore;

	/** Perform some final tasks before hosting/joining a session. Remove menus, set king state etc */
	void BeginSession();
	
	/** Display a loading screen */
	void ShowLoadingScreen();

	virtual void PreInitializeComponents() override;

	/** Initialize the game. This is called before actors' PreInitializeComponents. */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;

};
