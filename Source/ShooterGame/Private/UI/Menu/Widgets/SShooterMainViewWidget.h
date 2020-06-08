// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Widgets/SShooterSessionsWidget.h"

class SShooterMainViewWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SShooterMainViewWidget)
		: _OwnerHUD()
		, _PlayerOwner()
	{
	}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

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
	~SShooterMainViewWidget();

	/** call to rebuild menu and start animating it */
	void BuildAndShowMenu();

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;

private:

	/** callback for when one of the N buttons is clicked */
	FReply OnStartGame() const;

	/** Hosts a game, using the passed in game type */
	void StartGame();

	void HostGame(const FString& GameType);

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

	/** right(sub) menu layout box */
	TSharedPtr<SVerticalBox> ContentBox;

	/** The tile view of the category List. */
	TSharedPtr<class SShooterSessionsWidget> SessionView;
	
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
