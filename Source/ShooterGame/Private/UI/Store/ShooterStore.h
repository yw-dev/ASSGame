// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "UWidgets/ShooterItemDetailWidget.h"
#include "ShooterStoreCategory.h"
#include "ShooterStoreContent.h"
#include "Items/ShooterItem.h"
#include "Blueprint/UserWidget.h"
#include "ShooterStore.generated.h"

/**
 * Game Store View(商店主面板)
 */
UCLASS()
class UShooterStore : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UShooterStore(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store", meta = (BindWidget))
	UShooterStoreCategory* CategoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store", meta = (BindWidget))
	UShooterStoreContent* ContentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store", meta = (BindWidget))
	UShooterItemDetailWidget* DetailWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Items")
	TArray<UShooterItem*> CategoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Items")
	TArray<UShooterItem*> ContentItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Items")
	UShooterItem* CurrentCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Items")
	UShooterItem* CurrentContent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Items")
	UShooterItem* SelectedContentCategory;

	UFUNCTION()
	void DeSelectedAll();

	UFUNCTION()
	void SetCategoryItem(const TArray<class UShooterItem*>& InItems);

	UFUNCTION()
	void SetContentItem(const TArray<class UShooterItem*>& InItems);

	UFUNCTION()
	void SetCurrentCategory(UShooterItem* InItems);

	UFUNCTION()
	void SetCurrentContent(UShooterItem* InItems);

	UFUNCTION()
	void SetSelectedContentCategory(UShooterItem* InItems);

	UFUNCTION()
	void FindContentItem(UShooterItem* InItems, TArray<UShooterItem*>& OutItems);

	/**  */
	UFUNCTION()
	void OnCategoryItemSelected(UObject* Item, bool isSelected);

	/**  */
	UFUNCTION()
	void OnContentItemSelected(UObject* Item, bool isSelected);

	/**  */
	UFUNCTION()
	void OnItemPurchase();

};
