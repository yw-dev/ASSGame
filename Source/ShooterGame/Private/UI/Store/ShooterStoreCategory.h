// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/UWidgets/ShooterTitleView.h"
#include "ShooterStoreCategory.generated.h"

/**
 * Game Store Item Category View (商店：道具类别菜单面板)
 */
UCLASS()
class UShooterStoreCategory : public UUserWidget
{
	GENERATED_BODY()
	


public:

	UShooterStoreCategory(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	/** The height of each tile */
	UPROPERTY(EditAnywhere, Category = SlotTileWidget)
	float EntryHeight = 100;

	/** The width of each tile */
	UPROPERTY(EditAnywhere, Category = SlotTileWidget)
	float EntryWidth = 80.f;

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Category", meta = (BindWidget))
	UShooterTitleView* CategoryWidget;

	/** Sets the width if every tile entry */
	UFUNCTION(BlueprintCallable, Category = "Store Category")
	UShooterTitleView* GetListWidget() const { return CategoryWidget; };

};
