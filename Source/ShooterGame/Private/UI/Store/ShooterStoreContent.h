// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/UWidgets/ShooterTitleView.h"
#include "ShooterStoreContent.generated.h"

/**
 * Game Store Item Content View(商店：道具一览面板)
 */
UCLASS()
class UShooterStoreContent : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UShooterStoreContent(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	/** The height of each tile */
	UPROPERTY(EditAnywhere, Category = SlotTileWidget)
	float EntryHeight = 100.f;

	/** The width of each tile */
	UPROPERTY(EditAnywhere, Category = SlotTileWidget)
	float EntryWidth = 80.f;

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Content", meta = (BindWidget))
	UTextBlock* TitleWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store Content", meta = (BindWidget))
	UShooterTitleView* ContentListWidget;

	/** Gets the widget */
	UFUNCTION(BlueprintCallable, Category = "Store Content")
	UShooterTitleView* GetListWidget() const { return ContentListWidget; };

	/**
	 * Directly sets the widget text.
	 * Warning: This will wipe any binding created for the Text property!
	 * @param InText The text to assign to the widget
	 */
	UFUNCTION(BlueprintCallable, Category = "Store Content", meta = (DisplayName = "SetText (Text)"))
	void SetText(FText InText);

	/** Sets the height of every tile entry */
	UFUNCTION(BlueprintCallable, Category = "Store Content")
	void SetContentEntryHeight(float NewHeight);

	/** Sets the width if every tile entry */
	UFUNCTION(BlueprintCallable, Category = "Store Content")
	void SetContenEntryWidth(float NewWidth);

	
};
