// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IUserObjectListEntry.h"
#include "Styling/SlateBrush.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "ShooterSlotItemWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class USlateBrushAsset;
class UTexture2D;

/**
 * Game Store Slot Item View
 */
UCLASS()
class UShooterSlotItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()


public:

	UShooterSlotItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

	virtual bool Initialize() override;

protected:

	virtual void NativeConstruct() override;

public:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotItemWidget", meta = (BindWidget))
	//UButton* ButtonWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotItemWidget", meta = (BindWidget))
	UImage* ImageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlotItemWidget", meta = (BindWidget))
	UTextBlock* TextWidget;

public:
	/**  */
	UFUNCTION()
	void OnButtonClicked();

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrush(const FSlateBrush& InBrush);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushSize(FVector2D DesiredSize);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushTintColor(FSlateColor TintColor);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushFromAsset(USlateBrushAsset* Asset);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget")
	void SetBrushFromMaterial(UMaterialInterface* Material);

	/**
	 * Gets the widget text
	 * @return The widget text
	 */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget", meta = (DisplayName = "GetText (Text)"))
	FText GetText() const;

	/**
	 * Directly sets the widget text.
	 * Warning: This will wipe any binding created for the Text property!
	 * @param InText The text to assign to the widget
	 */
	UFUNCTION(BlueprintCallable, Category = "SlotItemWidget", meta = (DisplayName = "SetText (Text)"))
	void SetText(const FText& InText);

	
	
	
};
