// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterPlayerAbility.h"
#include "ShooterPlayerInventory.h"
#include "ShooterPlayerPhoto.h"
#include "ShooterPlayerBuffer.h"
#include "ShooterPlayerView.generated.h"

/**
 * Game Player Photo Info View Widget(Player：PlayerDashboard玩家仪表盘)
 */
UCLASS()
class UShooterPlayerView : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UShooterPlayerView(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDashboard", meta = (BindWidget))
	UShooterPlayerAbility* AbilityWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDashboard", meta = (BindWidget))
	UShooterPlayerPhoto* PhotoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDashboard", meta = (BindWidget))
	UShooterPlayerBuffer* BufferWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDashboard", meta = (BindWidget))
	UShooterPlayerInventory* InventoryWidget;

public:

	/** Gets the Ability widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerDashboard")
	UShooterPlayerAbility* GetAbilityWidget() const { return AbilityWidget; };

	/** Gets the Photo widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerDashboard")
	UShooterPlayerPhoto* GetPhotoWidget() const { return PhotoWidget; };

	/** Gets the Buffer widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerDashboard")
	UShooterPlayerBuffer* GetBufferWidget() const { return BufferWidget; };

	/** Gets the Inventory widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerDashboard")
	UShooterPlayerInventory* GetInventoryWidget() const { return InventoryWidget; };

	UFUNCTION(BlueprintCallable, Category = "PlayerDashboard")
	void InitWidget();

	UFUNCTION()
	void UpdatePlayerView(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems, FPrimaryAssetType ItemType);

	UFUNCTION()
	void UpdateInventoryWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems);

	UFUNCTION()
	void UpdateAbilityWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems);

	UFUNCTION()
	void UpdatePhotoWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems);

	UFUNCTION()
	void UpdateBufferWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems);

	void OnSlottedItemChanged(FShooterItemSlot ItemSlot, UShooterItem* Item);

	UFUNCTION()
	void OnInventoryItemChanged(UShooterItem* item, bool bAdded);

	//UFUNCTION()
	//void NotifyUpdateInventoryItem(UShooterItem* Item, bool bAdded);
	//void NotifySlottedItemChanged(FShooterItemSlot ItemSlot, UShooterItem* Item);
};
