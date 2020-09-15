// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterInventoryItemWidget.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Inventory Item Widget(Player：背包Item)
 */
UShooterInventoryItemWidget::UShooterInventoryItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterInventoryItemWidget::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::NativeConstruct()"));
	Super::NativeConstruct();

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.75f));
	}
}

void UShooterInventoryItemWidget::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterInventoryItemWidget::Update(UShooterItem* InItem)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::Update()"));
	//SetSlotData(SlotData);
	SetBrush(InItem->ItemIcon);
}

void UShooterInventoryItemWidget::SetItem(UShooterItem* InItem)
{
	Item = InItem;
}

void UShooterInventoryItemWidget::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UShooterInventoryItemWidget::SetSlotData(int32 InSlotData)
{
	SlotData = InSlotData;
}

void UShooterInventoryItemWidget::SetBrush(const FSlateBrush& InBrush)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrush()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrush(InBrush);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushSize(FVector2D DesiredSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushSize()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushSize(DesiredSize);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushTintColor(FSlateColor TintColor)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushTintColor()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushTintColor(TintColor);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushFromAsset()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushFromAsset(Asset);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushFromTexture()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushFromTexture(Texture, bMatchSize);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushFromAtlasInterface()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushFromAtlasInterface(AtlasRegion, bMatchSize);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushFromTextureDynamic()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushFromTextureDynamic(Texture, bMatchSize);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetBrushFromMaterial(UMaterialInterface* Material)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterInventoryItemWidget::SetBrushFromMaterial()"));

	if (this->ImageIcon != nullptr)
	{
		this->ImageIcon->SetBrushFromMaterial(Material);
		this->ImageIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterInventoryItemWidget::SetText(const FText& InText)
{
	if (this->SlotNumLabel != nullptr)
	{
		this->SlotNumLabel->SetText(InText);
	}
}


/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE




