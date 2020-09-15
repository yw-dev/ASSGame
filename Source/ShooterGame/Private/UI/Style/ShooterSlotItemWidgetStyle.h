// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Styling/ButtonWidgetStyle.h"
#include "SlateWidgetStyleContainerBase.h"
#include "ShooterSlotItemWidgetStyle.generated.h"

/**
 * Represents the appearance of an SShooterSlotItemWidget
 */
USTRUCT()
struct FShooterSlotItemStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()
	

	FShooterSlotItemStyle();
	virtual ~FShooterSlotItemStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FShooterSlotItemStyle& GetDefault();

	UPROPERTY(EditAnywhere, Category = Appearance)
	FButtonStyle SlotItemStyle;
	FShooterSlotItemStyle& SetSlotItemBrush(const FButtonStyle& InSlotItemStyle) { SlotItemStyle = InSlotItemStyle; return *this; }

	// Style that defines the text on all of our menu buttons. 
	UPROPERTY(EditAnywhere, Category = Appearance)
	FTextBlockStyle SlotItemTextStyle;
	FShooterSlotItemStyle& SetSlotItemBrush(const FTextBlockStyle& InSlotItemTextStyle) { SlotItemTextStyle = InSlotItemTextStyle; return *this; }

	/**
	 * The brush used for the header background
	 */
	UPROPERTY(EditAnywhere, Category = Appearance)
	FSlateBrush TextBackgroundBrush;
	FShooterSlotItemStyle& SetTextBackgroundBrush(const FSlateBrush& InTextBackgroundBrush) { TextBackgroundBrush = InTextBackgroundBrush; return *this; }

	/**
	 * The brush used for the header background
	 */
	UPROPERTY(EditAnywhere, Category = Appearance)
	FSlateBrush ImageIconBrush;
	FShooterSlotItemStyle& SetImageIconBrush(const FSlateBrush& InImageIconBrush) { ImageIconBrush = InImageIconBrush; return *this; }

};


/**
 */
UCLASS(hidecategories = Object, MinimalAPI)
class UShooterSlotItemWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu's appearance. */
	UPROPERTY(Category = Appearance, EditAnywhere, meta = (ShowOnlyInnerProperties))
	FShooterSlotItemStyle SlotItemStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast<const struct FSlateWidgetStyle*>(&SlotItemStyle);
	}
};
