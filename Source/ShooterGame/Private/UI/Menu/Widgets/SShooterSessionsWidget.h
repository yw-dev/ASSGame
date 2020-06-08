// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "SShooterServerList.h"
#include "ShooterLoginHUD.h"
#include "ShooterGameInstance.h"

struct FServerSessionEntry
{
	FString ServerName;
	FString CurrentPlayers;
	FString MaxPlayers;
	FString GameType;
	FString MapName;
	FString Ping;
	int32 SearchResultsIndex;
};


class SShooterSessionsWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SShooterSessionsWidget)
	{
	}
	//SLATE_ARGUMENT(TWeakObjectPtr<class UShooterGameInstance>, _GameInstance);
	//SLATE_ARGUMENT(TWeakObjectPtr<class AShooterLoginHUD>, OwnerHUD);
	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

	SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)


	SLATE_END_ARGS()

public:
	/**
	 * Constructs and lays out the Tutorial HUD UI Widget.
	 *
	 * \args Arguments structure that contains widget-specific setup information.
	 **/
	void Construct(const FArguments& args);

	/** Destructor */
	~SShooterSessionsWidget();

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;

	/** if we want to receive focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** focus received handler - keep the ActionBindingsList focused */
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	/** focus lost handler - keep the ActionBindingsList focused */
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	/** key down handler */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/**
	 * Ticks this widget.  Override in derived classes, but always call the parent implementation.
	 *
	 * @param  AllottedGeometry The space allotted for this widget
	 * @param  InCurrentTime  Current absolute real time
	 * @param  InDeltaTime  Real time passed since last tick
	 */
	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);

	/** SListView item double clicked */
	void OnListItemDoubleClicked(TSharedPtr<FServerEntry> InItem);

	/** creates single item widget, called for every list item */
	TSharedRef<ITableRow> MakeListViewWidget(TSharedPtr<FServerEntry> Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** selection changed handler */
	void EntrySelectionChanged(TSharedPtr<FServerEntry> InItem, ESelectInfo::Type SelectInfo);

	/**
	 * Get the current game session
	 *
	 * @return The current game session
	 */
	AShooterGameSession* GetGameSession() const;

	/** Updates current search status */
	void UpdateSearchStatus();

	/** Starts searching for servers */
	void BeginServerSearch(bool bLANMatch, bool bIsDedicatedServer, const FString& InMapFilterName);

	/** Called when server search is finished */
	void OnServerSearchFinished();

	/** fill/update server list, should be called before showing this control */
	void UpdateServerList();

	/** connect to chosen server */
	void ConnectToServer();

	/** selects item at current + MoveBy index */
	void MoveSelection(int32 MoveBy);

private:

	/** callback for when one of the N buttons is clicked */
	FReply OnBeginPlay() const;

	/** callback for when one of the N buttons is clicked */
	FReply OnSessionRefresh();

	/** Hosts a game, using the passed in game type */
	void GameConnection();

	/** gets Button Background color */
	FSlateColor GetButtonBGColor() const;

	/** Cache the UserIndex from the owning PlayerController */
	int32 GetOwnerUserIndex();

private:

	/** Whether last searched for LAN (so spacebar works) */
	bool bLANMatchSearch;

	/** Whether last searched for Dedicated Server (so spacebar works) */
	bool bDedicatedServer;

	/** Whether we're searching for servers */
	bool bSearchingForServers;

	/** Time the last search began */
	double LastSearchTime;

	/** Minimum time between searches (platform dependent) */
	double MinTimeBetweenSearches;
	
	/** action bindings array */
	TArray< TSharedPtr<FServerEntry> > ServerList;

	/** action bindings list slate widget */
	TSharedPtr< SListView< TSharedPtr<FServerEntry> > > ServerListWidget;

	/** currently selected list item */
	TSharedPtr<FServerEntry> SelectedItem;

	/** get current status text */
	FText GetBottomText() const;

	/** current status text */
	FText StatusText;

	/** Map filter name to use during server searches */
	FString MapFilterName;

	/** size of standard column in pixels */
	int32 BoxWidth;

	/** space between menu item and border */
	float OutlineWidth;

	/**
	 * Stores a weak reference to the HUD owning this widget.
	 **/
	TWeakObjectPtr<class AShooterLoginHUD> OwnerHUD;

	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** pointer to our owner PC */
	TWeakObjectPtr<class ULocalPlayer> PlayerOwner;

	/** pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;

	/**
	 * A reference to the Slate Style used for this HUD's widgets.
	 **/
	 //const struct FGlobalStyle* HUDStyle;
	 /**
	  * Attribute storing the binding for the player's score.
	  **/
	TAttribute<FText> Score;

	/**
	 * Our Score will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetScore() const;


	
};
