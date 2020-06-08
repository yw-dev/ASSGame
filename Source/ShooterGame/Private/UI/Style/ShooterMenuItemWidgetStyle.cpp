// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterMenuItemWidgetStyle.h"

FShooterMenuItemStyle::FShooterMenuItemStyle()
{
}

FShooterMenuItemStyle::~FShooterMenuItemStyle()
{
}

const FName FShooterMenuItemStyle::TypeName(TEXT("FShooterMenuItemStyle"));

const FShooterMenuItemStyle& FShooterMenuItemStyle::GetDefault()
{
	static FShooterMenuItemStyle Default;
	return Default;
}

void FShooterMenuItemStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
	OutBrushes.Add(&LeftArrowImage);
	OutBrushes.Add(&RightArrowImage);
	OutBrushes.Add(&TopArrowImage);
	OutBrushes.Add(&BottomArrowImage);
}


UShooterMenuItemWidgetStyle::UShooterMenuItemWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
