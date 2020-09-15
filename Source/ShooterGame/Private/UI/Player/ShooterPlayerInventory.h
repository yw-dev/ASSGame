// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/UWidgets/ShooterInventoryItemWidget.h"
#include "ShooterPlayerInventory.generated.h"

/**
 * Game Player Photo Info View Widget(Player：背包面板)
 */
UCLASS()
class UShooterPlayerInventory : public UUserWidget
{
	GENERATED_BODY()

public:

	UShooterPlayerInventory(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;
	virtual void BeginDestroy() override;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	uint8 bFirstWeapon : 1;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* WeaponSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* WeaponSlot2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UShooterInventoryItemWidget* OtherSlot6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventory", meta = (BindWidget))
	UTextBlock* CoinsLabel;

public:

	UFUNCTION()
	void UpdateInventory(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems);

	/**
	 * Directly sets the widget text.
	 * Warning: This will wipe any binding created for the Text property!
	 * @param InText The text to assign to the widget
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory | Coins")
	void SetCoins(const FText& InText);

	/** Gets the Inventory Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UTextBlock* GetCoinsLabel() const { return CoinsLabel; };

	/** Gets the Inventory Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetWeaponSlot1() const { return WeaponSlot1; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetWeaponSlot2() const { return WeaponSlot2; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot1() const { return OtherSlot1; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot2() const { return OtherSlot2; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot3() const { return OtherSlot3; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot4() const { return OtherSlot4; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot5() const { return OtherSlot5; };

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UShooterInventoryItemWidget* GetOtherSlot6() const { return OtherSlot6; };


};
