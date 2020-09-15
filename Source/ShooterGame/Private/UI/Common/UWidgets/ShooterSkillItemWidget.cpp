// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterSkillItemWidget.h"



#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Skill Item View Widget(Player：技能插槽)
 */
UShooterSkillItemWidget::UShooterSkillItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterSkillItemWidget::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::NativeConstruct()"));
	Super::NativeConstruct();
}

void UShooterSkillItemWidget::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterSkillItemWidget::Update(UShooterItem* InItem)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::Update()"));
	//SetSlotData(SlotData);
	SetBrush(InItem->ItemIcon);
}

void UShooterSkillItemWidget::SetItem(UShooterItem* InItem)
{
	Item = InItem;
}

void UShooterSkillItemWidget::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UShooterSkillItemWidget::SetBrush(const FSlateBrush& InBrush)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrush()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrush(InBrush);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushSize(FVector2D DesiredSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushSize()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushSize(DesiredSize);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushTintColor(FSlateColor TintColor)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushTintColor()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushTintColor(TintColor);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushFromAsset()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushFromAsset(Asset);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushFromTexture()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushFromTexture(Texture, bMatchSize);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushFromAtlasInterface()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushFromAtlasInterface(AtlasRegion, bMatchSize);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushFromTextureDynamic()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushFromTextureDynamic(Texture, bMatchSize);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetBrushFromMaterial(UMaterialInterface* Material)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSkillItemWidget::SetBrushFromMaterial()"));

	if (this->SkillIcon != nullptr)
	{
		this->SkillIcon->SetBrushFromMaterial(Material);
		this->SkillIcon->SetColorAndOpacity(FLinearColor::White);
	}
}

void UShooterSkillItemWidget::SetText(const FText& InText)
{
	if (this->ConsumeLabel != nullptr)
	{
		this->ConsumeLabel->SetText(InText);
	}
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE


