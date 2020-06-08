// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SShooterLoginWidget.h"

class FShooterLogin : public TSharedFromThis<FShooterLogin>, public FTickableGameObject
{
public:

	virtual ~FShooterLogin();

	/** build menu */
	void Construct(TWeakObjectPtr<UShooterGameInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

	/** Add the menu to the gameviewport so it becomes visible */
	void AddMenuToGameViewport();

	/** Remove from the gameviewport. */
	void RemoveMenuFromGameViewport();

protected:
	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** Owning player */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	/** menu widget */
	TSharedPtr<class SShooterLoginWidget> LoginWidget;

	/* used for removing the MenuWidget */
	TSharedPtr<class SWeakWidget> LoginWidgetContainer;

	/** lan game? */
	bool bIsLanMatch;

	/** Dedicated server? */
	bool bIsDedicatedServer;

	/** Start the check for whether the owner of the menu has online privileges */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);

	/** Common cleanup code for any Privilege task delegate */
	void CleanupOnlinePrivilegeTask();

public:
	/** TickableObject Functions */
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual UWorld* GetTickableGameObjectWorld() const override;

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;

	/** Called if a play together invite is sent from the PS4 system */
	void OnPlayTogetherEventReceived();

	
	
};
