// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterInventoryItemWidget.generated.h"

/**
 * Game Player Inventory Item Widget(Player：背包Item)
 */
UCLASS()
class UShooterInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UShooterInventoryItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventoryItem", meta = (BindWidget))
	UImage* ImageIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventoryItem", meta = (BindWidget))
	UTextBlock* SlotNumLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventoryItem")
	UShooterItem* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventoryItem")
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInventoryItem")
	int32 SlotData;

public:

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void Update(UShooterItem* InItem);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetItem(UShooterItem* InItem);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetSlotIndex(int32 InSlotIndex);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetSlotData(int32 InSlotData);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrush(const FSlateBrush& InBrush);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushSize(FVector2D DesiredSize);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushTintColor(FSlateColor TintColor);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushFromAsset(USlateBrushAsset* Asset);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem")
	void SetBrushFromMaterial(UMaterialInterface* Material);


	/**
	 * Directly sets the widget text.
	 * Warning: This will wipe any binding created for the Text property!
	 * @param InText The text to assign to the widget
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventoryItem", meta = (DisplayName = "SetText (Text)"))
	void SetText(const FText& InText);


	
};
