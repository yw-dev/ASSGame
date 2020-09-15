// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Styling/CoreStyle.h"
#include "ShooterTypes.h"

/**
 * Game Slot Item SWidget
 */
class SShooterSlotItemWidget : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SShooterSlotItemWidget)
		: _Title(TEXT("Default Value"))
		, _ItemPadding(0.f)
		, _ImageIcon()
		, _ButtonClicked()
		, _OnMouseButtonDown()
		{}

	/** Color and opacity */
	SLATE_ATTRIBUTE(FString, Title)

	/** Color and opacity */
	SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

	/** Thickness */
	SLATE_ATTRIBUTE(FText, LabelText)

	/** Thickness */
	SLATE_ATTRIBUTE(float, ItemPadding)

	/** Image resource */
	SLATE_ATTRIBUTE(const FSlateBrush*, ImageIcon)

	/** Invoked when the mouse is pressed in the widget. */
	SLATE_EVENT(FOnClicked, ButtonClicked)

	/** Invoked when the mouse is pressed in the widget. */
	SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)

	SLATE_END_ARGS()

public:
	/**
	* Construct this widget
	*
	* @param	InArgs	The declaration data for this widget
	*/
	void Construct(const FArguments& InArgs);

	//////////////////////////////////////////////////////////////////
	/** Widget Attribute Function */
	void SetItemPadding(const TAttribute<float>& InVal);

	void SetItemPadding(float InVal);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);

	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/** See the ColorAndOpacity attribute */
	void SetLabelText(const TAttribute<FText>& InLabel);

	/** See the ColorAndOpacity attribute */
	void SetLabelText(const FText& InLabel);

	/** See the ColorAndOpacity attribute */
	void SetLabelText(FText InLabel);

	/** See the Image attribute */
	void SetImage(TAttribute<const FSlateBrush*> InImage);

	/** See OnMouseButtonDown event */
	void SetButtonClicked(FOnClicked InButtonClicked);

	/** See OnMouseButtonDown event */
	void SetOnMouseButtonDown(FPointerEventHandler EventHandler);

	/**
	 * Gets the text assigned to this text block
	 *
	 * @return	This text block's text string
	 */
	const FText& GetLabelText() const
	{
		return LabelText.Get();
	}


public:

	/** The slate brush to draw for the image, or a bound delegate to a brush. */
	TAttribute<const FSlateBrush*> ImageIcon;

	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/** Color and opacity scale for this image */
	TAttribute<FText> LabelText;

	TAttribute<FString> Title;

	TAttribute<float> ItemPadding;

	/** Invoked when the mouse is pressed in the image */
	FPointerEventHandler OnMouseButtonDownHandler;

	/** Invoked when the button is pressed in the widget*/
	FOnClicked ButtonClicked;

private:

	/** style for the menu widget */
	const struct FShooterSlotItemStyle *MenuStyle;


};
