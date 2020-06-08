// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterGameInstance.h"

/**
* 角色选择
*/
class FShooterPawnGuide : public TSharedFromThis<FShooterPawnGuide>, public FTickableGameObject
{

public:

	virtual ~FShooterPawnGuide();

	/** build menu */
	void Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

	/** Add the menu to the gameviewport so it becomes visible */
	void AddMenuToGameViewport();

	/** Remove from the gameviewport. */
	void RemoveMenuFromGameViewport();

	/** Hosts a game, using the passed in game type */
	void HostGame(const FString& GameType);

	/** Hosts team deathmatch game */
	void HostTeamDeathMatch();

	void ShowMenuView();

	void LoadPawnDataSource();

	/** TickableObject Functions */
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual UWorld* GetTickableGameObjectWorld() const override;

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Gets the view models for the current set of content source categories. */
	//const TArray<FShooterRaceEntry>* GetCategories();

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;


protected:

	//TMap<UShooterItem*, FShooterItemData> PawnSource;

	//TMap<FShooterItemSlot, UShooterItem*> SlottedItems;

	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** Owning player */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	/** menu widget */
	//TSharedPtr<class SShooterPawnGuideWidget> PawnGuideWidget;

	/* used for removing the MenuWidget */
	TSharedPtr<class SWeakWidget> PawnGuideWidgetContainer;

	/** The view models for the available categories. */
	//TArray<FShooterRaceEntry> RaceCategories;

	//TSharedPtr<FShooterPawnGuideViewModel> ViewModel;

	//TMap<EShooterRaceType, TArray<TSharedPtr<FShooterRaceEntry>>> CategoryToSelectedContentSourceMap;

	/** lan game? */
	bool bIsLanMatch;

	/** Recording demos? */
	bool bIsRecordingDemo;

	/** Dedicated server? */
	bool bIsDedicatedServer;

	/** Start the check for whether the owner of the menu has online privileges */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);

	/** Common cleanup code for any Privilege task delegate */
	void CleanupOnlinePrivilegeTask();

	/** Handles the available categories changing on the view model. */
	void OnViewSourcesComplete();

	
};
