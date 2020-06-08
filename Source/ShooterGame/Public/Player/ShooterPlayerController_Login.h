// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online.h"
#include "CoreMinimal.h"
#include "ShooterMenuHUD.h"
#include "GameFramework/PlayerController.h"
#include "UI/ViewModel/ShooterPawnSourceViewModel.h"
#include "ShooterPlayerController_Login.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class SHOOTERGAME_API AShooterPlayerController_Login : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:

	/** shooter in-game menu */
	TSharedPtr<class FShooterPawnGuide> PawnGuideMenu;

	/** After game is initialized */
	virtual void PostInitializeComponents() override;

	/** Associate a new UPlayer with this PlayerController. */
	virtual void SetPlayer(UPlayer* Player);

	/** Auto slots a specific item, returns true if anything changed */
	bool FillEmptySlotWithItem(UShooterItem* NewItem);

	bool SaveInventory();

	void NotifyInventoryLoaded();

	//FShooterPawnSourceViewModel ViewSource;

	void BuildViewSourceViewModels();

private:

	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

public:

	int32 Health;

	FName ServerSayString;

	// For tracking whether or not to send the end event
	bool bHasSentStartEvents;

	void SpawnPawnActor();

	/** notify player about started match */
	UFUNCTION(reliable, client)
	void ClientGameStarted();

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
	void ClientEndOnlineGame();

	/** Returns a pointer to the shooter game hud. May return NULL. */
	AShooterMenuHUD* GetMenuHUD() const;

	/** notify local client about deaths */
	void OnConnectedMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList);

	/** Local function say a string */
	UFUNCTION(exec)
	virtual void Say(const FString& Msg);

	/** RPC for clients to talk to server */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerSay(const FString& Msg);

	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnSource)
	TMap<UShooterItem*, FShooterItemData> PawnSource;

	/** Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable, Category = PawnSource)
	FOnSlottedItemChanged OnSlottedItemChanged;

	/** Native version above, called before BP delegate */
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = PawnSource)
	FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;


	/** Map of slot, from type/num to item, initialized from ItemSlotsPerType on ShooterGameInstanceBase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnSource)
	TMap<FShooterItemSlot, UShooterItem*> SlottedItems;

	/** Return the client to the main menu gracefully */
	void ClientReturnToMainMenuWithTextReason_Implementation(const FText& ReturnReason) override;

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = PawnSource)
	bool LoadPawnSource();

	/** Returns number of instances of this item found in the inventory. This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = PawnSource)
	int32 GetInventoryItemCount(UShooterItem* Item) const;

	/** Sets slot to item, will remove from other slots if necessary. If passing null this will empty the slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SetSlottedItem(FShooterItemSlot ItemSlot, UShooterItem* Item);

	/** Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UShooterItem* GetSlottedItem(FShooterItemSlot ItemSlot) const;

	
};
