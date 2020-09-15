// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterSlotItemWidget.h"
#include "Components/TileView.h"
#include "ShooterTitleView.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTitleItemSelectionChanged, UObject*, bool);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnListItemSelectionChangedDynamic, UObject*, Item, bool, bIsSelected);
/**
 * 
 */
UCLASS(meta = (EntryInterface = UserObjectListEntry))
class UShooterTitleView : public UTileView
{
	GENERATED_BODY()

public:
	UShooterTitleView(const FObjectInitializer& ObjectInitializer);

	FOnTitleItemSelectionChanged OnTitleItemSelectionChanged;

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:

	virtual void OnItemDoubleClickedInternal(UObject* Item) override;
	virtual void OnSelectionChangedInternal(UObject* FirstSelectedItem) override;
};
