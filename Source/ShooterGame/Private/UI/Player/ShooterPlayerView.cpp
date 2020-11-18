// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "ShooterPlayerView.h"



#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Photo Info View Widget(Player：主面板)
 */
UShooterPlayerView::UShooterPlayerView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterPlayerView::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::NativeConstruct()"));
	Super::NativeConstruct();

	AShooterPlayerController* Controller = Cast<AShooterPlayerController>(GetOwningPlayer());
	Controller->GetInventoryItemChangedDelegate().AddDynamic(this, &UShooterPlayerView::OnInventoryItemChanged);
	Controller->GetSlottedItemChangedDelegate().AddUObject(this, &UShooterPlayerView::OnSlottedItemChanged);
}

void UShooterPlayerView::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UShooterPlayerView::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterPlayerView::InitWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::InitWidget()"));
	if (AbilityWidget)
	{
		AbilityWidget->InitWidget();
	}
}

void UShooterPlayerView::UpdatePlayerView(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems, FPrimaryAssetType ItemType)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());
	if (ItemType == UShooterAssetManager::SkillItemType)
	{
		UpdateAbilityWidget(SlottedItems);
	}
	else
	{
		UpdateInventoryWidget(SlottedItems);
	}
}

void UShooterPlayerView::UpdateInventoryWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());
	InventoryWidget->UpdateInventory(SlottedItems);
}

void UShooterPlayerView::UpdateAbilityWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());
	AbilityWidget->UpdateAbility(SlottedItems);
}

void UShooterPlayerView::UpdatePhotoWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());
	//PhotoWidget->UpdatePhoto(SlottedItems);
}

void UShooterPlayerView::UpdateBufferWidget(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());
	//BufferWidget->UpdateBuff(SlottedItems);
}

void UShooterPlayerView::OnSlottedItemChanged(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::OnSlottedItemChanged(--------------------------)"));

}

void UShooterPlayerView::OnInventoryItemChanged(UShooterItem* item, bool bAdded)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("OnInventoryItemChanged(--------------------------)"));
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerView::OnInventoryItemChanged(--------------------------)"));

}


/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE





