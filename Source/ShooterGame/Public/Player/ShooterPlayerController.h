// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Online.h"
#include "ShooterTypes.h"
#include "Items/ShooterTokenItem.h"
#include "Blueprint/UserWidget.h"
#include "ShooterLeaderboards.h"
#include "ShooterInventoryInterface.h"
#include "Entries/ShooterAssetEntry.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Store/ShooterStore.h"
#include "Player/ShooterPlayerView.h"
#include "Player/ShooterTargetPlayer.h"
#include "Team/ShooterTeamBar.h"
#include "UI/ViewModel/ShooterStoreViewModel.h"
#include "ShooterPlayerController.generated.h"


class AShooterHUD;
class UShooterStore;
class UShooterPlayerView;
class UhooterTargetPlayer;


UCLASS(config = Game)
class AShooterPlayerController : public APlayerController, public IShooterInventoryInterface
{
	GENERATED_UCLASS_BODY()

public:
	AShooterPlayerController();
	
	/** sets spectator location and rotation */
	UFUNCTION(reliable, client)
	void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	/** notify player about started match */
	UFUNCTION(reliable, client)
	void ClientGameStarted();

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();	

	/** notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	/** Notifies clients to send the end-of-round event */
	UFUNCTION(reliable, client)
	void ClientSendRoundEndEvent(bool bIsWinner, int32 ExpendedTimeInSeconds);

	/** Notifies clients to send the Purchase event */
	UFUNCTION(reliable, client)
	void ClientReceivePurchaseEvent(const FShooterItemSlot& NewItemSlot, const FShooterItemData& NewItemData, class UShooterItem* NewItem, bool bAdd);

	/** Notifies clients to send the Purchase Failure message */
	UFUNCTION(reliable, client)
	void ClientReceivePurchaseFailure(const FText& Message, class UShooterItem* NewItem);

	/** Notifies clients to send the Purchase Failure message */
	UFUNCTION(reliable, client)
	void ClientReceivePlayerStateChangeEvent(float InHealth, float InMaxHealth, float InRestoreHealth);

	/** used for input simulation from blueprint (for automatic perf tests) */
	UFUNCTION(BlueprintCallable, Category="Input")
	void SimulateInputKey(FKey Key, bool bPressed = true);

	/** sends cheat message */
	UFUNCTION(reliable, server, WithValidation)
	void ServerCheat(const FString& Msg);

	/* Overriden Message implementation. */
	virtual void ClientTeamMessage_Implementation( APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime ) override;

	/* Tell the HUD to toggle the chat window. */
	void ToggleChatWindow();

	/** Local function say a string */
	UFUNCTION(exec)
	virtual void Say(const FString& Msg);

	/** RPC for clients to talk to server */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerSay(const FString& Msg);	

	/** Local function run an emote */
// 	UFUNCTION(exec)
// 	virtual void Emote(const FString& Msg);

	/** notify local client about Inventory item change */
	//void OnInventoryMessage(class AShooterPlayerState* OwnerPlayerState, const UShooterItem* Item, bool bAdd);

	/** notify local client about Coins */
	void OnCoinsIncomeMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** notify local client about deaths */
	void OnDeathMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** notify local client about deaths */
	void OnConnectedMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** toggle InGameMenu handler */
	void OnToggleInGameMenu();

	/** Show the in-game menu if it's not already showing */
	void ShowInGameMenu();

	/** Hides scoreboard if currently diplayed */
	void OnConditionalCloseScoreboard();

	/** Toggles scoreboard */
	void OnToggleScoreboard();

	/** Toggles scoreboard */
	void OnToggleStoreboard();

	/** shows scoreboard */
	void OnShowScoreboard();

	/** hides scoreboard */
	void OnHideScoreboard();

	/** set infinite ammo cheat */
	void SetInfiniteAmmo(bool bEnable);

	/** set infinite clip cheat */
	void SetInfiniteClip(bool bEnable);

	/** set health regen cheat */
	void SetHealthRegen(bool bEnable);

	/** set Respawn time */
	void SetRespawnDelay(float InDelay);

	/** set god mode cheat */
	UFUNCTION(exec)
	void SetGodMode(bool bEnable);

	/** sets the produce force feedback flag. */
	void SetIsVibrationEnabled(bool bEnable);

	/** get infinite ammo cheat */
	bool HasInfiniteAmmo() const;

	/** get infinite clip cheat */
	bool HasInfiniteClip() const;

	/** get health regen cheat */
	bool HasHealthRegen() const;

	/** get gode mode cheat */
	bool HasGodMode() const;

	/** get Current Respawn time */
	float GetCurrentRespawnDelay() const;

	/** should produce force feedback? */
	bool IsVibrationEnabled() const;

	/** check if gameplay related actions (movement, weapon usage, etc) are allowed right now */
	bool IsGameInputAllowed() const;

	/** is game menu currently active? */
	bool IsGameMenuVisible() const;	

	/** is game shop menu currently active? */
	bool IsGameShopMenuVisible() const;

	/** Ends and/or destroys game session */
	void CleanupSessionOnReturnToMenu();

	/**
	 * Called when the read achievements request from the server is complete
	 *
	 * @param PlayerId The player id who is responsible for this delegate being fired
	 * @param bWasSuccessful true if the server responded successfully to the request
	 */
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful );

	UFUNCTION()
	void OnLeaderboardReadComplete(bool bWasSuccessful);
	
	// Begin APlayerController interface

	/** handle weapon visibility */
	virtual void SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning) override;

	/** Returns true if movement input is ignored. Overridden to always allow spectators to move. */
	virtual bool IsMoveInputIgnored() const override;

	/** Returns true if look input is ignored. Overridden to always allow spectators to look around. */
	virtual bool IsLookInputIgnored() const override;

	/** initialize the input system from the player settings */
	virtual void InitInputSystem() override;

	virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;

	virtual FVector GetFocalLocation() const override;

	// End APlayerController interface

	// begin AShooterPlayerController-specific

	/** 
	 * Reads achievements to precache them before first use
	 */
	void QueryAchievements();

	/**
	 * Reads backend stats to precache them before first use
	 */
	void QueryStats();

	/** 
	 * Writes a single achievement (unless another write is in progress).
	 *
	 * @param Id achievement id (string)
	 * @param Percent number 1 to 100
	 */
	void UpdateAchievementProgress( const FString& Id, float Percent );

	/** Returns a pointer to the shooter game hud. May return NULL. */
	AShooterHUD* GetShooterHUD() const;

	/** Returns the persistent user record associated with this player, or null if there is't one. */
	class UShooterPersistentUser* GetPersistentUser() const;

	/** Informs that player fragged someone */
	void OnKill();

	/** Cleans up any resources necessary to return to main menu.  Does not modify GameInstance state. */
	virtual void HandleReturnToMainMenu();

	/** Associate a new UPlayer with this PlayerController. */
	virtual void SetPlayer(UPlayer* Player);

	// end AShooterPlayerController-specific

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

protected:

	//UPROPERTY(VisibleAnywhere, Transient, ReplicatedUsing = OnRep_PurchaseItem)
	//UShooterItem* CurrentPurchaseItem;

	/** infinite ammo cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteAmmo : 1;

	/** infinite clip cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bInfiniteClip : 1;

	/** health regen cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bHealthRegen : 1;

	/** god mode cheat */
	UPROPERTY(Transient, Replicated)
	uint8 bGodMode : 1;

	/** Pawn Respawn time */
	UPROPERTY(Transient, Replicated)
	float CurrentRespawnDelay;

	/** should produce force feedback? */
	uint8 bIsVibrationEnabled : 1;

	/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
	uint8 bAllowGameActions : 1;

	/** true for the first frame after the game has ended */
	uint8 bGameEndedFrame : 1;

	/** stores pawn location at last player death, used where player scores a kill after they died **/
	FVector LastDeathLocation;

	/** shooter in-game menu */
	TSharedPtr<class FShooterIngameMenu> ShooterIngameMenu;

	/** shooter in-game shop menu */
	TSharedPtr<class FShooterShopMenu> ShooterShopMenu;

	/** Achievements write object */
	FOnlineAchievementsWritePtr WriteObject;

	/** try to find spot for death cam */
	bool FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation);

	virtual void BeginDestroy() override;

	//Begin AActor interface

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;

	/** Internal. Used to store stats from the online interface. These increment as matches are written */
	int32 StatMatchesPlayed;
	int32 StatKills;
	int32 StatDeaths;
	bool bHasFetchedPlatformData;

	/** Internal. Reads the stats from the platform backend to sync online status with local */
	FOnlineLeaderboardReadPtr ReadObject;
	FDelegateHandle LeaderboardReadCompleteDelegateHandle;
	void ClearLeaderboardDelegate();

public:
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	//End AActor interface

	//Begin AController interface

	virtual void BeginInactiveState() override;

	/** transition to dead state, retries spawning later */
	virtual void FailedToSpawnPawn() override;
	
	/** update camera when pawn dies */
	virtual void PawnPendingDestroy(APawn* P) override;

	//End AController interface

	// Begin APlayerController interface

	/** respawn after dying */
	virtual void UnFreeze() override;

	//virtual float GetMinRespawnDelay() override;

	/** sets up input */
	virtual void SetupInputComponent() override;

	/**
	 * Called from game info upon end of the game, used to transition to proper state. 
	 *
	 * @param EndGameFocus Actor to set as the view target on end game
	 * @param bIsWinner true if this controller is on winning team
	 */
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) override;	

	/** Return the client to the main menu gracefully.  ONLY sets GI state. */
	void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
	virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSuicide();

	/** Updates achievements based on the PersistentUser stats at the end of a round */
	void UpdateAchievementsOnGameEnd();

	/** Updates leaderboard stats at the end of a round */
	void UpdateLeaderboardsOnGameEnd();

	/** Updates the save file at the end of a round */
	void UpdateSaveFileOnGameEnd(bool bIsWinner);

	// End APlayerController interface

	FName ServerSayString;

	// Timer used for updating friends in the player tick.
	float ShooterFriendUpdateTimer;

	// For tracking whether or not to send the end event
	bool bHasSentStartEvents;

private:

	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

public:

	virtual void BeginPlay() override;
	virtual void Possess(APawn* NewPawn) override;
	virtual void UnPossess() override;


	/******************** PlayerDashboard View Event Delegate *********************/
	/** Show the Player Dashboard if it's not already showing */
	//UFUNCTION(BlueprintCallable, Category = Inventory)
	//void ShowPlayerDashboard();

	UFUNCTION()
	void NotifyHPChanged(float InHealth, float InMaxHealth, float InRestoreHealth);

	UFUNCTION()
	void NotifyMPChanged(float InMana, float InMaxMana, float InRestoreMana);

	/******************** PlayerTarget View Event Delegate *********************/



	void InitializPlayerSlots();

	// 显示商店面板
	void ShowStoreView();

	// 隐藏商店面板
	void HideStoreView();

	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	void CloseStoreBoard();

	/* 卖出道具 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	void SellItem(UShooterItem* Item);

	/* 购买道具 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	void PurchaseItem(UShooterItem* Item);

	/* 满足购买条件吗 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	bool CanPurchaseItem(UShooterItem* Item);

	/** Returns number of instances of this item found in the inventory. This uses count from GetItemData */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasItem(UShooterItem* Item);

	/* 金币足够吗 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	bool IsCoinsEnough(UShooterItem* Item);

	/* 背包空间足够吗 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	bool IsSlotSpaceEnough(UShooterItem* Item);

	/* 是可叠加的物品吗 */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	bool IsPluralItems(UShooterItem* Item);

	/* 更新Souls数量-Item的price（卖出道具后） */
	UFUNCTION(BlueprintCallable, Category = "StoreWidget")
	void AddSouls(int32 Price);

	void AddItemToSlottedItems(FShooterItemData NewData, UShooterItem* NewItem, int32 ItemCount);

	void RemoveItemFromSlottedItems(FShooterItemData NewData, UShooterItem* NewItem);

	/** server equip weapon */
	UFUNCTION(reliable, server, WithValidation)
	void ServerPurchase(UShooterItem* Item);

	UFUNCTION(BlueprintCallable)
	TArray<UShooterItem*> GetInventoryItemList(FPrimaryAssetType ItemType) const;

	UFUNCTION(BlueprintCallable)
	int32 GetInventoryItemListCount() const;

	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Store)
	TArray<UShooterItem*> AssetCategoryList;

	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Store)
	TArray<UShooterItem*> StoreItems;

	/** Map of Inventory items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<UShooterItem*, FShooterItemData> InventorySlot;

	/** Map of Weapon items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<UShooterItem*, FShooterItemData> WeaponSlot;

	/** Map of Ability items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	TMap<UShooterItem*, FShooterItemData> AbilitySlot;

	/** Map of slot, from type/num to item, initialized from ItemSlotsPerType on ShooterGameInstanceBase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FShooterItemSlot, UShooterItem*> SlottedItems;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	UShooterTokenItem* SoulsItem;

	/** Handle to various registered delegates */
	//FDelegateHandle InventoryItemChangedHandle;

	//FDelegateHandle SlottedItemChangedHandle;

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryItemChanged OnInventoryItemChanged;

	/** Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = Store)
	FOnStoreCategoryChanged OnStoreCategoryChanged;

	/** Native version above, called before BP delegate */
	FOnStoreCategoryChangedNative OnStoreCategoryChangedNative;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = Store)
	FOnStoreContentChanged OnStoreContentChanged;

	/** Native version above, called before BP delegate */
	FOnStoreContentChangedNative OnStoreContentChangedNative;


	/** Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnSlottedItemChanged OnSlottedItemChanged;

	/** Native version above, called before BP delegate */
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	/** Delegate called when an inventory souls has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventorySoulsChanged OnInventorySoulsChanged;

	//DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySoulsChangedNative, int32 /*ItemCount*/);
	FOnInventorySoulsChangedNative OnInventorySoulsChangedNative;

	void OnInventoryItemChange(UShooterItem* item, bool bAdded);
	//virtual void OnInventoryItemChange_Implementation(UShooterItem* item, bool bAdded);

	UFUNCTION(BlueprintNativeEvent, Category = Inventory)
	void OnItemSlotChanged(FShooterItemSlot ItemSlot, UShooterItem* item);
	//virtual void OnItemSlotChanged_Implementation(FShooterItemSlot ItemSlot, UShooterItem* item);

	/** Auto slots a specific item, returns true if anything changed */
	bool FillEmptySlotWithItem(UShooterItem* NewItem);

	/** Calls the inventory update callbacks */
	void NotifyInventoryItemChanged(UShooterItem* Item, bool bAdded);
	void NotifySlottedItemChanged(FShooterItemSlot ItemSlot, UShooterItem* Item);
	void NotifyInventoryLoaded();

	/** Called right after we have possessed a pawn */
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "BeginPossessPawn"))
	void ReceivePossess(APawn* NewPawn);

	/** Called right before unpossessing a pawn */
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "EndPossessPawn"))
	void ReceiveUnPossess(APawn* PreviousPawn);

	/** Update inventory: Adds or Remove inventory item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UpdateInventoryItem(UShooterItem* NewItem, bool bAdd = true);

	/** Adds a new inventory item, will add it to an empty slot if possible. If the item supports count you can add more than one count. It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(UShooterItem* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1, bool bAutoSlot = true);

	/** Remove an inventory item, will also remove from slots. A remove count of <= 0 means to remove all copies */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveInventoryItem(UShooterItem* RemovedItem, int32 RemoveCount = 1);

	/** Returns all inventory items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void GetInventoryItems(TArray<UShooterItem*>& Items, FPrimaryAssetType ItemType);

	/** Returns number of instances of this item found in the inventory. This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = Inventory)
	int32 GetInventoryItemCount(UShooterItem* Item) const;

	/** Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure, Category = Inventory)
	bool GetInventoryItemData(UShooterItem* Item, FShooterItemData& ItemData) const;

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool GetInventoryItemSlot(UShooterItem* Item, FShooterItemSlot& ItemSlot) const;

	/** Sets slot to item, will remove from other slots if necessary. If passing null this will empty the slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SetSlottedItem(FShooterItemSlot ItemSlot, UShooterItem* Item);

	/** Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UShooterItem* GetSlottedItem(FShooterItemSlot ItemSlot) const;

	/** Returns PlayerDashboard widget. */
	//UFUNCTION(BlueprintPure, Category = "HUD|PlayerDashboard")
	//UShooterPlayerView* GetPlayerDashboard() const;

	/** Gets the Buffer widget */
	//UFUNCTION(BlueprintCallable, Category = "HUD|PlayerTarget")
	//UShooterTargetPlayer* GetPlayerTarget() const;

	/** Gets the Buffer widget */
	//UFUNCTION(BlueprintCallable, Category = "HUD|TeamBar")
	//UShooterTeamBar* GetTeamBar() const;

	/** Fills in any empty slots with items in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void FillEmptySlots();

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveInventory();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool LoadInventory();

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SaveStore();

	/** Loads inventory from save game on game instance, this will replace arrays */
	//UFUNCTION(BlueprintCallable, Category = Inventory)
	//bool LoadStoreItemSource();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Store)
	bool FindStoreAssetsByID(FPrimaryAssetId InAssetId, TArray<UShooterItem*>& OutItems);

	TArray<TSharedPtr<FShooterAssetEntry>> Categories;

	//const TArray<TSharedPtr<FShooterAssetEntry>>* GetStoreCategory();

	// Implement IShooterInventoryInterface
	virtual const TMap<UShooterItem*, FShooterItemData>& GetInventorySlotMap() const override
	{
		return InventorySlot;
	}
	virtual const TMap<UShooterItem*, FShooterItemData>& GetWeaponItemMap() const override
	{
		return WeaponSlot;
	}
	virtual const TMap<UShooterItem*, FShooterItemData>& GetAbilityItemMap() const override
	{
		return AbilitySlot;
	}
	virtual const TMap<FShooterItemSlot, UShooterItem*>& GetSlottedItemMap() const override
	{
		return SlottedItems;
	}
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedNativeDelegate() override
	{
		return OnInventoryItemChangedNative;
	}
	virtual FOnInventoryItemChanged& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChanged;
	}
	virtual FOnStoreCategoryChangedNative& GetStoreCategoryChangedDelegate() override
	{
		return OnStoreCategoryChangedNative;
	}
	virtual FOnStoreContentChangedNative& GetStoreContentChangedDelegate() override
	{
		return OnStoreContentChangedNative;
	}
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() override
	{
		return OnSlottedItemChangedNative;
	}
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return OnInventoryLoadedNative;
	}

protected:

	/**************************** PlayerController All Data RPC *******************************/
	//UFUNCTION()
	//void OnRep_PurchaseItem(UShooterItem* NewPurchaseItem);

	//UFUNCTION()
	//void SetCurrentPurchaseItem(UShooterItem* NewPurchaseItem);
	   	 
	/************************* HUD UI widgets *****************************/
	TSharedPtr<SWidget> StoreboardContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StoreWidget")
	TSubclassOf<UShooterStore> StoreWidgetClass;

	UPROPERTY()
	UShooterStore* StoreBoard;

	uint8 bStoreVisible : 1;

	/** PlayerDashboard widget overlay. */
	//TSharedPtr<class SOverlay> PlayerboardOverlay;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerDashboard")
	//TSubclassOf<UShooterPlayerView> PlayerBoardWidgetClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDashboard")
	//UShooterPlayerView* PlayerDashboard;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerTarget")
	//TSubclassOf<UShooterTargetPlayer> PlayerTargetWidgetClass;

	/** PlayerDashboard widget overlay. */
	//TSharedPtr<class SOverlay> PlayerTargetOverlay;

	//UPROPERTY()
	//UShooterTargetPlayer* PlayerTarget;

	//TSharedPtr<SWidget> TeamBarContainer;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerTeamBar")
	//TSubclassOf<UShooterTeamBar> TeamBarWidgetClass;

	/** PlayerDashboard widget overlay. */
	//TSharedPtr<class SOverlay> TeamBarOverlay;

	//UPROPERTY()
	//UShooterTeamBar* TeamBar;

	/** Show the Player Target if it's not already showing */
	//void ShowPlayerTarget();

	/** Show the Player Target if it's not already showing */
	//void ShowTeamBar();

	//获取需要操作数据的widget对象
	void FindImageComponents();

	//动态加载icon所需Texture2D
	void LoadAssetsDynamic();


};

