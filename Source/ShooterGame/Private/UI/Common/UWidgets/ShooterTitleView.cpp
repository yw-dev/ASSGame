// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTitleView.h"


/////////////////////////////////////////////////////
// UShooterTitleView

UShooterTitleView::UShooterTitleView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterTitleView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

void UShooterTitleView::OnSelectionChangedInternal(UObject* FirstSelectedItem)
{
	Super::OnSelectionChangedInternal(FirstSelectedItem);
	//auto entry = Cast<UShooterSlotItemWidget>(FirstSelectedItem);
	OnTitleItemSelectionChanged.Broadcast(FirstSelectedItem, FirstSelectedItem != nullptr);
}

void UShooterTitleView::OnItemDoubleClickedInternal(UObject* Item)
{
	Super::OnItemDoubleClickedInternal(Item);
	//auto entry = Cast<UShooterSlotItemWidget>(FirstSelectedItem);
	//OnStoreItemSelectionChanged.Broadcast(entry, entry != nullptr);
}

