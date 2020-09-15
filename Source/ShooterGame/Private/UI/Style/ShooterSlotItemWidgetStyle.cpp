// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterSlotItemWidgetStyle.h"


FShooterSlotItemStyle::FShooterSlotItemStyle()
{
}

FShooterSlotItemStyle::~FShooterSlotItemStyle()
{
}


const FName FShooterSlotItemStyle::TypeName(TEXT("FShooterSlotItemStyle"));

const FShooterSlotItemStyle& FShooterSlotItemStyle::GetDefault()
{
	static FShooterSlotItemStyle Default;
	return Default;
}

void FShooterSlotItemStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&TextBackgroundBrush);
	OutBrushes.Add(&ImageIconBrush);
	OutBrushes.Add(&SlotItemStyle.Normal);
	OutBrushes.Add(&SlotItemStyle.Hovered);
	OutBrushes.Add(&SlotItemStyle.Pressed);
	OutBrushes.Add(&SlotItemStyle.Disabled);
	//OutBrushes.Add(&BackgroundBrush);
	//OutBrushes.Add(&LeftArrowImage);
	//OutBrushes.Add(&RightArrowImage);
	//OutBrushes.Add(&TopArrowImage);
	//OutBrushes.Add(&BottomArrowImage);
}


UShooterSlotItemWidgetStyle::UShooterSlotItemWidgetStyle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


