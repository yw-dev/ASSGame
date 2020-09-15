// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "ShooterSkillItemWidget.generated.h"

/**
 * Game Player Skill Item View Widget(Player：技能插槽)
 */
UCLASS()
class UShooterSkillItemWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UShooterSkillItemWidget(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillItem", meta = (BindWidget))
	UImage* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillItem", meta = (BindWidget))
	UBorder* ConsumeBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillItem", meta = (BindWidget))
	UTextBlock* ConsumeLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillItem")
	UShooterItem* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillItem")
	int32 SlotIndex;

public:

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void Update(UShooterItem* InItem);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetItem(UShooterItem* InItem);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetSlotIndex(int32 InSlotIndex);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrush(const FSlateBrush& InBrush);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushSize(FVector2D DesiredSize);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushTintColor(FSlateColor TintColor);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushFromAsset(USlateBrushAsset* Asset);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "SkillItem")
	void SetBrushFromMaterial(UMaterialInterface* Material);


	/**
	 * Directly sets the widget text.
	 * Warning: This will wipe any binding created for the Text property!
	 * @param InText The text to assign to the widget
	 */
	UFUNCTION(BlueprintCallable, Category = "SkillItem", meta = (DisplayName = "SetText (Text)"))
	void SetText(const FText& InText);


	
};
