// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPawnSourceViewModel.h"


// Sets default values
FShooterPawnSourceViewModel::FShooterPawnSourceViewModel()
{
}

TSharedRef<FShooterPawnSourceViewModel> FShooterPawnSourceViewModel::CreateShared()
{
	TSharedPtr<FShooterPawnSourceViewModel> Shared = MakeShareable(new FShooterPawnSourceViewModel());
	Shared->Initialize();
	return Shared.ToSharedRef();
}

void FShooterPawnSourceViewModel::Initialize()
{/*
	IAddContentDialogModule& AddContentDialogModule = FModuleManager::LoadModuleChecked<IAddContentDialogModule>("AddContentDialog");

	for (auto ContentSourceProvider : *AddContentDialogModule.GetContentSourceProviderManager()->GetContentSourceProviders())
	{
		ContentSourceProviders.Add(ContentSourceProvider);
		ContentSourceProvider->SetContentSourcesChanged(FOnContentSourcesChanged::CreateSP(this, &FAddContentWidgetViewModel::ContentSourcesChanged));
	}
	*/
	//ContentSourceFilter = TSharedPtr<ContentSourceTextFilter>(new ContentSourceTextFilter(ContentSourceTextFilter::FItemToStringArray::CreateSP(this, &FShooterPawnSourceViewModel::TransformContentSourceToStrings)));

	BuildContentSourceViewModels();
}

void FShooterPawnSourceViewModel::BuildContentSourceViewModels()
{
	Categories.Empty();
	//ContentSource.Empty();
	FilteredContentSource.Empty();
	//CategoryToSelectedContentSourceMap.Empty();
	//~~~~~~~~~~~~~~~~~~~
	//Root Level 
	// FShooterPawnItem(const FShooterPawnItemPtr IN_ParentElement, 
	//	const FString& IN_DirectoryPath, 
	//	const FString& IN_CategotyName, 
	//	const FSlateBrush& IN_CategotyIcon, 
	//	const FString& IN_CategotyDescrip, 
	//	const FString& IN_PawnName, 
	//	const FSlateBrush& IN_PawnIcon, 
	//	const TAssetSubclassOf<AActor>& IN_Pawn)
	//FSlateBrush Brush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100));
	TSharedRef<FShooterPawnItem> RootDir = MakeShareable(new FShooterPawnItem(NULL, 
		TEXT("Category1"), 
		FString("Category1"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Category1Descrip")
	));
	Categories.Add(RootDir);
	TSharedRef<FShooterPawnItem> RootDir2 = MakeShareable(new FShooterPawnItem(NULL, 
		TEXT("Category2"),
		FString("Category2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100)),
		FString("Category2Descrip")
		));
	Categories.Add(RootDir2);

	//~~~~~~~~~~~~~~~~~~~

	//Root1 Category
	FShooterPawnItemPtr ParentCategory = RootDir;
	FShooterPawnItemPtr EachSubDir = MakeShareable(new FShooterPawnItem(ParentCategory, 
		TEXT("Joy"),
		FString("Joy"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("JoyDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	RootDir->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterPawnItem(ParentCategory,
		TEXT("Song"),
		FString("Song"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("SongDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	RootDir->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterPawnItem(ParentCategory,
		TEXT("Dance"),
		FString("Dance"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("DanceDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	RootDir->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterPawnItem(ParentCategory,
		TEXT("Rainbows"),
		FString("Rainbows"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("RainbowsDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	RootDir->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterPawnItem(ParentCategory, 
		TEXT("Butterflies"),
		FString("Butterflies"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("SongDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	RootDir->AddSubDirectory(EachSubDir);

	//Root2 Category
	FShooterPawnItemPtr ParentCategory2 = RootDir2;
	FShooterPawnItemPtr EachSubDir2 = MakeShareable(new FShooterPawnItem(ParentCategory2, 
		TEXT("Joy2"),
		FString("Joy2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	RootDir2->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterPawnItem(ParentCategory2, 
		TEXT("Song2"),
		FString("Song2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	RootDir2->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterPawnItem(ParentCategory2,
		TEXT("Dance2"),
		FString("Dance2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Dance2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	RootDir2->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterPawnItem(ParentCategory2,
		TEXT("Rainbows2"),
		FString("Rainbows2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Rainbows2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	RootDir2->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterPawnItem(ParentCategory2, 
	TEXT("Butterflies2"),
		FString("Butterflies2"),
		FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	RootDir2->AddSubDirectory(EachSubDir2);

	//ContentSource.Reset();
	// Default selected current Category value.
	SelectedCategory = Categories[0];
	//ContentSource = SelectedCategory->AccessSubDirectories();
	SelectedContent = SelectedCategory->AccessSubDirectories()[0];

	if (Categories.Num() > 0)
	{
		UpdateFilteredContentSourcesAndSelection(false);
	}

	OnCategoriesChanged.ExecuteIfBound();
}

void FShooterPawnSourceViewModel::UpdateFilteredContentSourcesAndSelection(bool bAllowEmptySelection)
{
	FilteredContentSource.Empty();
	for (auto ContentItem : Categories)
	{
		if (ContentItem == SelectedCategory)
		{
			FilteredContentSource.Append(ContentItem->AccessSubDirectories());
		}
	}
	OnContentSourcesChanged.ExecuteIfBound();

	if (FilteredContentSource.Contains(GetSelectedContent()) == false)
	{
		FShooterPawnItemPtr NewSelectedContentSource;
		if (bAllowEmptySelection == false && FilteredContentSource.Num() > 0)
		{
			NewSelectedContentSource = FilteredContentSource[0];
		}
		SetSelectedContent(NewSelectedContentSource);
	}
}

void FShooterPawnSourceViewModel::TransformContentSourceToStrings(FShooterPawnItemPtr Item, OUT TArray<FString>& Array)
{
	Array.Add(Item->GetName());
}

void FShooterPawnSourceViewModel::SetSelectedCategory(FShooterPawnItemPtr SelectedCategoryIn)
{
	SelectedCategory = SelectedCategoryIn;
	UpdateFilteredContentSourcesAndSelection(true);
	OnSelectedContentSourceChanged.ExecuteIfBound();
}

void FShooterPawnSourceViewModel::SetSelectedContent(FShooterPawnItemPtr SelectedContentSourceIn)
{
	// Ignore selecting the currently selected item.
	SelectedContent = SelectedContentSourceIn;
	OnSelectedContentSourceChanged.ExecuteIfBound();
}

const TArray<FShooterPawnItemPtr>* FShooterPawnSourceViewModel::GetCategories()
{
	return &Categories;
}

const TArray<FShooterPawnItemPtr>* FShooterPawnSourceViewModel::GetContentSources()
{
	return &FilteredContentSource;
}

FShooterPawnItemPtr FShooterPawnSourceViewModel::GetSelectedCategory()
{
	//return SelectedCategory;
	//FShooterPawnItemPtr InSelectedCategory = SelectedCategory;
	if (SelectedCategory)
	{
		return SelectedCategory;
	}
	return TSharedPtr<FShooterPawnItem>();
}

FShooterPawnItemPtr FShooterPawnSourceViewModel::GetSelectedContent()
{
	//FShooterPawnItemPtr InSelectedContent = SelectedContent;
	if (SelectedContent)
	{
		return SelectedContent;
	}
	return TSharedPtr<FShooterPawnItem>();
}

void FShooterPawnSourceViewModel::ContentSourcesChanged()
{
	BuildContentSourceViewModels();
}

void FShooterPawnSourceViewModel::SetOnViewSourceChanged(FOnViewSourceChanged OnViewSourceChangedIn)
{
	OnViewSourceChanged = OnViewSourceChangedIn;
}

void FShooterPawnSourceViewModel::SetOnCategoriesChanged(FOnCategoriesChanged OnCategoriesChangedIn)
{
	OnCategoriesChanged = OnCategoriesChangedIn;
}

void FShooterPawnSourceViewModel::SetOnContentSourcesChanged(FOnContentSourcesChanged OnContentSourcesChangedIn)
{
	OnContentSourcesChanged = OnContentSourcesChangedIn;
}

void FShooterPawnSourceViewModel::SetOnSelectedContentSourceChanged(FOnSelectedContentSourceChanged OnSelectedContentSourceChangedIn)
{
	OnSelectedContentSourceChanged = OnSelectedContentSourceChangedIn;
}




