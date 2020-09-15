// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStoreContent.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Store Item Content View(商店：道具一览面板)
 */
UShooterStoreContent::UShooterStoreContent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UShooterStoreContent::NativeConstruct()
{
	Super::NativeConstruct();
	// ItemTitle can be nullptr if we haven't created it in the
	// Blueprint subclass

	if (ContentListWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::NativeConstruct( ContentWidgetClass != nullptr)"));
		ContentListWidget->SetEntryHeight(EntryHeight);
		ContentListWidget->SetEntryWidth(EntryWidth);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::NativeConstruct( ContentWidget == nullptr)"));
	}
}

void UShooterStoreContent::SynchronizeProperties()
{
	Super::SynchronizeProperties();

}

void UShooterStoreContent::SetText(FText InText)
{
	if (TitleWidget)
	{
		TitleWidget->SetText(InText);
	}
}


void UShooterStoreContent::SetContentEntryHeight(float NewHeight)
{
	if (ContentListWidget)
	{
		ContentListWidget->SetEntryHeight(EntryHeight);
	}
}

void UShooterStoreContent::SetContenEntryWidth(float NewWidth)
{
	if (ContentListWidget)
	{
		ContentListWidget->SetEntryWidth(EntryWidth);
	}
}


/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE




