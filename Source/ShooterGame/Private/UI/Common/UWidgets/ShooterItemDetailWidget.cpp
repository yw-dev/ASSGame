// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterItemDetailWidget.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Store Slot Item Detail Info View(商店：道具详情面板)
 */
UShooterItemDetailWidget::UShooterItemDetailWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterItemDetailWidget::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterItemDetailWidget::NativeConstruct()"));
	Super::NativeConstruct();

	if (CurrentItem)
	{
		SetPreview(CurrentItem->ItemIcon);
		SetTitle(CurrentItem->ItemName);
		SetDescrip(CurrentItem->ItemDescription);
	}

}

void UShooterItemDetailWidget::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterItemDetailWidget::SynchronizeProperties()"));

	Super::SynchronizeProperties();

	if (CurrentItem)
	{
		SetPreview(CurrentItem->ItemIcon);
		SetTitle(CurrentItem->ItemName);
		SetDescrip(CurrentItem->ItemDescription);
	}
}

void UShooterItemDetailWidget::SetCurrentItem(UShooterItem* InItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterItemDetailWidget::SetCurrentItem( InItems = %s)"), *InItems->ItemName.ToString());
	CurrentItem = InItems;
}

void UShooterItemDetailWidget::SetPreview(const FSlateBrush& InBrush)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterItemDetailWidget::SetPreview()"));

	if (PreviewWidget != nullptr)
	{
		PreviewWidget->SetBrush(InBrush);
	}
}

void UShooterItemDetailWidget::SetTitle(const FText& InText)
{
	if (this->TitleWidget != nullptr)
	{
		this->TitleWidget->SetText(InText);
	}
}

void UShooterItemDetailWidget::SetDescrip(const FText& InText)
{
	if (this->DescripWidget != nullptr)
	{
		this->DescripWidget->SetText(InText);
	}
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
