// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterSlotItemWidget.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Store Slot Item View
 */
UShooterSlotItemWidget::UShooterSlotItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterSlotItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	/*
	this->ButtonWidget = Cast<UButton>(GetWidgetFromName(FName(TEXT("ButtonWidget"))));
	if (this->ButtonWidget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( ButtonWidget != nullptr)"));
		ButtonWidget->OnClicked.AddDynamic(this, &UShooterSlotItemWidget::OnButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( ButtonWidget == nullptr)"));
	}*/

	this->ImageWidget = Cast<UImage>(GetWidgetFromName(FName(TEXT("ImageWidget"))));
	if (this->ImageWidget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( ImageWidget != nullptr)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( ImageWidget == nullptr)"));
	}

	this->TextWidget = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("TextWidget"))));
	if (this->TextWidget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( TextWidget != nullptr)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::NativeConstruct( TextWidget == nullptr)"));
	}
}


void UShooterSlotItemWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

bool UShooterSlotItemWidget::Initialize()
{
	Super::Initialize();

	return true;
}

void UShooterSlotItemWidget::OnButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::OnButtonClicked()"));
}

void UShooterSlotItemWidget::SetBrush(const FSlateBrush& InBrush)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrush()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrush(InBrush);
	}
}

void UShooterSlotItemWidget::SetBrushSize(FVector2D DesiredSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushSize()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushSize(DesiredSize);
	}
}

void UShooterSlotItemWidget::SetBrushTintColor(FSlateColor TintColor)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushTintColor()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushTintColor(TintColor);
	}
}

void UShooterSlotItemWidget::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushFromAsset()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushFromAsset(Asset);
	}
}

void UShooterSlotItemWidget::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushFromTexture()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushFromTexture(Texture, bMatchSize);
	}
}

void UShooterSlotItemWidget::SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushFromAtlasInterface()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushFromAtlasInterface(AtlasRegion, bMatchSize);
	}
}

void UShooterSlotItemWidget::SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushFromTextureDynamic()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushFromTextureDynamic(Texture, bMatchSize);
	}
}

void UShooterSlotItemWidget::SetBrushFromMaterial(UMaterialInterface* Material)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterSlotItemWidget::SetBrushFromMaterial()"));

	if (this->ImageWidget != nullptr)
	{
		this->ImageWidget->SetBrushFromMaterial(Material);
	}
}

FText UShooterSlotItemWidget::GetText() const
{
	if (this->TextWidget != nullptr)
	{
		return this->TextWidget->GetText();
	}

	return FText::FromString(TEXT(""));
}

void UShooterSlotItemWidget::SetText(const FText& InText)
{
	if (this->TextWidget != nullptr)
	{
		this->TextWidget->SetText(InText);
	}
}



/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
