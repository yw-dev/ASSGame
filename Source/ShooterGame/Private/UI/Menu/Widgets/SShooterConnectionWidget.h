// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "ShooterLoginHUD.h"
#include "ShooterGameInstance.h"



class SShooterConnectionWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SShooterConnectionWidget)
		: _OwnerHUD()
	{
	}

	SLATE_ARGUMENT(TWeakObjectPtr<class AShooterLoginHUD>, OwnerHUD);

	SLATE_END_ARGS()

public:
	/**
	 * Constructs and lays out the Tutorial HUD UI Widget.
	 *
	 * \args Arguments structure that contains widget-specific setup information.
	 **/
	void Construct(const FArguments& args);

	/** Destructor */
	~SShooterConnectionWidget();

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;

private:

	/** callback for when one of the N buttons is clicked */
	FReply OnConnectionServer() const;

	/** Hosts a game, using the passed in game type */
	void ServerConnection();

	/** Hosts a game, using the passed in game type */
	void ConnectionServerByMode(const FString& GameType);

	void OnIPTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);

	void OnIPTextChanged(const FText& NewText);

	void OnPortTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);

	void OnPortTextChanged(const FText& NewText);

	void OnNickNameTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);

	void OnNickNameTextChanged(const FText& NewText);

private:

	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** Owning player */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;


	/**
	 * Stores a weak reference to the HUD owning this widget.
	 **/
	TWeakObjectPtr<class AShooterLoginHUD> OwnerHUD;

	/**
	 * A reference to the Slate Style used for this HUD's widgets.
	 **/
	 //const struct FGlobalStyle* HUDStyle;
	 /**
	  * Attribute storing the binding for the player's input server IP address.
	  **/
	FText SIP;

	/**
	 * A reference to the Slate Style used for this HUD's widgets.
	 **/
	 //const struct FGlobalStyle* HUDStyle;
	 /**
	  * Attribute storing the binding for the player's input server IP address.
	  **/
	FText SPort;

	/**
	 * A reference to the Slate Style used for this HUD's widgets.
	 **/
	 //const struct FGlobalStyle* HUDStyle;
	 /**
	  * Attribute storing the binding for the player's input server IP address.
	  **/
	FText NickName;

	/**
	 * Server address will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetSIP() const;

	/**
	 * Server address will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetSPort() const;

	/**
	 * Server address will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetNickName() const;
	   
	
	
};
