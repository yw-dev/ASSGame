// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"
#include "ShooterPlayerInventory.h"



#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Photo Info View Widget(Player：背包面板)
 */
UShooterPlayerInventory::UShooterPlayerInventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterPlayerInventory::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerInventory::NativeConstruct()"));
	Super::NativeConstruct();

	if (WeaponSlot1)
	{
		WeaponSlot1->SetSlotIndex(1);
	}
	if (WeaponSlot2)
	{
		WeaponSlot2->SetSlotIndex(2);
	}
	if (OtherSlot1)
	{
		OtherSlot1->SetSlotIndex(1);
		OtherSlot1->SetText(FText::FromString(TEXT("1")));
	}
	if (OtherSlot2)
	{
		OtherSlot2->SetSlotIndex(2);
		OtherSlot2->SetText(FText::FromString(TEXT("2")));
	}
	if (OtherSlot3)
	{
		OtherSlot3->SetSlotIndex(3);
		OtherSlot3->SetText(FText::FromString(TEXT("3")));
	}
	if (OtherSlot4)
	{
		OtherSlot4->SetSlotIndex(4);
		OtherSlot4->SetText(FText::FromString(TEXT("4")));
	}
	if (OtherSlot5)
	{
		OtherSlot5->SetSlotIndex(5);
		OtherSlot5->SetText(FText::FromString(TEXT("5")));
	}
	if (OtherSlot6)
	{
		OtherSlot6->SetSlotIndex(6);
		OtherSlot6->SetText(FText::FromString(TEXT("6")));
	}
	if (CoinsLabel)
	{
		CoinsLabel->SetText(FText::FromString(TEXT("350")));
	}
}

void UShooterPlayerInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UShooterPlayerInventory::BeginDestroy()
{
	Super::BeginDestroy();
}

void UShooterPlayerInventory::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerInventory::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterPlayerInventory::UpdateInventory(const TMap<FShooterItemSlot, UShooterItem*>& SlottedItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerInventory::UpdateInventory(SlottedItems.Num = %d)"), SlottedItems.Num());

	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == UShooterAssetManager::WeaponItemType)
		{
			if (WeaponSlot2 && Pair.Key.SlotNumber == WeaponSlot2->SlotIndex-1)
			{
				WeaponSlot2->Update(Pair.Value);
			}
			else if (WeaponSlot1 && Pair.Key.SlotNumber == WeaponSlot1->SlotIndex-1)
			{
				WeaponSlot1->Update(Pair.Value);
			}
		}
		else 
		{
			if (Pair.Key.ItemType != UShooterAssetManager::SkillItemType)
			{
				if (OtherSlot1 && Pair.Key.SlotNumber == OtherSlot1->SlotIndex - 1)
				{
					OtherSlot1->Update(Pair.Value);
				}
				else if (OtherSlot2 && Pair.Key.SlotNumber == OtherSlot2->SlotIndex - 1)
				{
					OtherSlot2->Update(Pair.Value);
				}
				else if (OtherSlot3 && Pair.Key.SlotNumber == OtherSlot3->SlotIndex - 1)
				{
					OtherSlot3->Update(Pair.Value);
				}
				else if (OtherSlot4 && Pair.Key.SlotNumber == OtherSlot4->SlotIndex - 1)
				{
					OtherSlot4->Update(Pair.Value);
				}
				else if (OtherSlot5 && Pair.Key.SlotNumber == OtherSlot5->SlotIndex - 1)
				{
					OtherSlot5->Update(Pair.Value);
				}
				else if (OtherSlot6 && Pair.Key.SlotNumber == OtherSlot6->SlotIndex - 1)
				{
					OtherSlot6->Update(Pair.Value);
				}
			}
		}
	}
}

void UShooterPlayerInventory::SetCoins(const FText& InText)
{
	if (CoinsLabel)
	{
		CoinsLabel->SetText(InText);
	}
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE






