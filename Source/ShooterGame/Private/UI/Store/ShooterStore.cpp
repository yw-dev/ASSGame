// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Items/ShooterCategoryItem.h"
#include "ShooterStore.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Store View(商店主面板)
 */
UShooterStore::UShooterStore(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UShooterStore::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterStore::NativeConstruct( CategoryItems.Num = %d | ContentItems.Num = %d)"), CategoryItems.Num(), ContentItems.Num());
	Super::NativeConstruct();

	if (CategoryWidget)
	{
		CategoryWidget->GetListWidget()->SetListItems(CategoryItems);
		CategoryWidget->GetListWidget()->OnTitleItemSelectionChanged.AddUObject(this, &UShooterStore::OnCategoryItemSelected);
		ContentWidget->GetListWidget()->OnTitleItemSelectionChanged.AddUObject(this, &UShooterStore::OnContentItemSelected);
	}
	if (ContentWidget)
	{
		ContentWidget->SetText(CurrentCategory->ItemName);
		ContentWidget->GetListWidget()->SetListItems(ContentItems);
	}

	if (DetailWidget)
	{
		DetailWidget->SetPreview(CurrentContent->Preview);
		DetailWidget->SetTitle(CurrentContent->ItemName);
		DetailWidget->SetDescrip(CurrentContent->ItemDescription);
		DetailWidget->GetPurchaseButton()->OnClicked.AddDynamic(this, &UShooterStore::OnItemPurchase);
	}
}

void UShooterStore::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SynchronizeProperties( ContentItems.Num = %d)"), ContentItems.Num());
	Super::SynchronizeProperties();

	if (CategoryWidget && CategoryItems.Num() > 0)
	{
		CategoryWidget->GetListWidget()->SetListItems(CategoryItems);
	}

	if (ContentWidget && CurrentCategory && ContentItems.Num()>0)
	{
		ContentWidget->SetText(CurrentCategory->ItemName);
		ContentWidget->GetListWidget()->SetListItems(ContentItems);
	}
}

void UShooterStore::FindContentItem(UShooterItem* InItems, TArray<UShooterItem*>& OutItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterStore::FindContentItem()"));
	AShooterPlayerController* PC = Cast<AShooterPlayerController>(GetOwningPlayer());

	UShooterCategoryItem* CategoryItem = Cast<UShooterCategoryItem>(InItems);
	FPrimaryAssetId AssetId = FPrimaryAssetId(CategoryItem->Category.Type, CategoryItem->Category.Type.GetName());

	//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::FindContentItem( %s )"), *FString::Printf(TEXT("Item = %s"), *CategoryItem->ItemName.ToString()));
	PC->FindStoreAssetsByID(AssetId, OutItems);
}

void UShooterStore::OnCategoryItemSelected(UObject* Item, bool isSelected)
{
	if (Item != nullptr) {
		ContentItems.Reset();

		SetCurrentCategory(Cast<UShooterItem>(Item));
		FindContentItem(CurrentCategory, ContentItems);
		if (ContentWidget)
		{
			ContentWidget->SetText(CurrentCategory->ItemName);
			ContentWidget->GetListWidget()->SetListItems(ContentItems);
		}

		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnCategoryItemSelected( ContentItems.Num = %d)"), ContentItems.Num());
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnCategoryItemSelected(isSelected = %s)"), isSelected == false ? TEXT("false") : TEXT("true"));		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnCategoryItemSelected(Item is valid)"));
	}
}

void UShooterStore::OnContentItemSelected(UObject* Item, bool isSelected)
{
	if (Item != nullptr) {
		UShooterItem* SelectedItem = Cast<UShooterItem>(Item);
		if (CurrentContent != SelectedItem)
		{
			CurrentContent->bSelected = false;
			SetCurrentContent(SelectedItem);
			SelectedContentCategory->bSelected = false;
			SetSelectedContentCategory(CurrentCategory);
		}
		if (DetailWidget)
		{
			DetailWidget->SetPreview(CurrentContent->Preview);
			DetailWidget->SetTitle(CurrentContent->ItemName);
			DetailWidget->SetDescrip(CurrentContent->ItemDescription);
		}
		//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnContentItemSelected( %s )"), *CurrentContent->ItemName.ToString());
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnContentItemSelected(isSelected = %s)"), isSelected == false ? TEXT("false") : TEXT("true"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnContentItemSelected(Item is valid)"));
	}
}

void UShooterStore::OnItemPurchase()
{
	//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::OnItemPurchase( CurrentContent = %s)"), *CurrentContent->ItemName.ToString());

	AShooterPlayerController* PC = Cast<AShooterPlayerController>(GetOwningPlayer());
	AShooterCharacter* AC = Cast<AShooterCharacter>(PC->GetCharacter());
	PC->PurchaseItem(CurrentContent);
	if (AC) {

	}
}

void UShooterStore::DeSelectedAll()
{
	CurrentCategory->bSelected = false;
	CurrentContent->bSelected = false;
	SelectedContentCategory->bSelected = false;
}

void UShooterStore::SetCategoryItem(const TArray<class UShooterItem*>& InItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SetCategoryItem( InItems.Num = %d)"), InItems.Num());
	CategoryItems = InItems;
}

void UShooterStore::SetContentItem(const TArray<class UShooterItem*>& InItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SetContentItem( InItems.Num = %d)"), InItems.Num());
	ContentItems = InItems;
}

void UShooterStore::SetCurrentCategory(UShooterItem* InItems)
{
	//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SetCurrentCategory( Item = %s )"), *InItems->ItemName.ToString());
	CurrentCategory = InItems;
}

void UShooterStore::SetCurrentContent(UShooterItem* InItems)
{
	//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SetCurrentContent( Item = %s )"), *InItems->ItemName.ToString());
	CurrentContent = InItems;
}

void UShooterStore::SetSelectedContentCategory(UShooterItem* InItems)
{
	//UE_LOG(LogTemp, Warning, TEXT("UShooterStore::SetSelectedContentCategory( Item = %s )"), *InItems->ItemName.ToString());
	SelectedContentCategory = InItems;
}


/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
