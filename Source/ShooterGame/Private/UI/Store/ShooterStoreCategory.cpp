// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStoreCategory.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Store Item Category View (商店：道具类别菜单面板)
 */
UShooterStoreCategory::UShooterStoreCategory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UShooterStoreCategory::NativeConstruct()
{
	Super::NativeConstruct();

	// ItemTitle can be nullptr if we haven't created it in the
	// Blueprint subclass

	if (CategoryWidget)
	{
		CategoryWidget->SetEntryHeight(EntryHeight);
		CategoryWidget->SetEntryWidth(EntryWidth);
	}
}

void UShooterStoreCategory::SynchronizeProperties()
{
	Super::SynchronizeProperties();

}



/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

