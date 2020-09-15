// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "ShooterItemDetailWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class URichTextBlock;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class USlateBrushAsset;
class UTexture2D;

/**
 * 
 * Game Store Slot Item Detail Info View(商店：道具详情面板)
 */
UCLASS()
class UShooterItemDetailWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UShooterItemDetailWidget(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemDetailWidget", meta = (BindWidget))
	UButton* ButtonWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemDetailWidget", meta = (BindWidget))
	UImage* PreviewWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemDetailWidget", meta = (BindWidget))
	UTextBlock* TitleWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemDetailWidget", meta = (BindWidget))
	URichTextBlock* DescripWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemDetailWidget")
	UShooterItem* CurrentItem;

	UFUNCTION()
	void SetCurrentItem(UShooterItem* InItems);

	UFUNCTION(BlueprintCallable, Category = "ItemDetailWidget")
	void SetPreview(const FSlateBrush& InBrush);

	UFUNCTION(BlueprintCallable, Category = "ItemDetailWidget")
	void SetTitle(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "ItemDetailWidget")
	void SetDescrip(const FText& InText);

	/** Gets the Inventory Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerInventory")
	UButton* GetPurchaseButton() const { return ButtonWidget; };

};
