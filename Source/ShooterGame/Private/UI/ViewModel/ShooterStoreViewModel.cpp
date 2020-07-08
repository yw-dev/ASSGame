// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStoreViewModel.h"


// Sets default values
FShooterStoreViewModel::FShooterStoreViewModel()
{

}

TSharedRef<FShooterStoreViewModel> FShooterStoreViewModel::CreateShared()
{
	TSharedPtr<FShooterStoreViewModel> Shared = MakeShareable(new FShooterStoreViewModel());
	Shared->Initialize();
	return Shared.ToSharedRef();
}

void FShooterStoreViewModel::Initialize()
{/*
	IAddContentDialogModule& AddContentDialogModule = FModuleManager::LoadModuleChecked<IAddContentDialogModule>("AddContentDialog");

	for (auto ContentSourceProvider : *AddContentDialogModule.GetContentSourceProviderManager()->GetContentSourceProviders())
	{
		ContentSourceProviders.Add(ContentSourceProvider);
		ContentSourceProvider->SetContentSourcesChanged(FOnContentSourcesChanged::CreateSP(this, &FAddContentWidgetViewModel::ContentSourcesChanged));
	}
	*/
	//ContentSourceFilter = TSharedPtr<ContentSourceTextFilter>(new ContentSourceTextFilter(ContentSourceTextFilter::FItemToStringArray::CreateSP(this, &FShooterStoreViewModel::TransformContentSourceToStrings)));

	BuildContentSourceViewModels();
}


void FShooterStoreViewModel::BuildContentSourceViewModels()
{
	Categories.Empty();
	//ContentSource.Empty();
	FilteredContentSource.Empty();
	//CategoryToSelectedContentSourceMap.Empty();
	//~~~~~~~~~~~~~~~~~~~
	//Root Level 
	// FShooterWidgetItem(const FShooterWidgetItemPtr IN_ParentElement, 
	//	const FString& IN_DirectoryPath, 
	//	const FString& IN_CategotyName, 
	//	const FSlateBrush& IN_CategotyIcon, 
	//	const FString& IN_CategotyDescrip, 
	//	const FString& IN_PawnName, 
	//	const FSlateBrush& IN_PawnIcon, 
	//	const TAssetSubclassOf<AActor>& IN_Pawn)
	//FSlateBrush Brush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100));
	//TSharedPtr<FSlateBrush> IconBrush = FSlateDynamicImageBrush::CreateWithImageData(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100));

	// FShooterImageBrush brush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Beasts_01.INV_Misc_Ticket_Tarot_Beasts_01"), FVector2D(100, 100)),
	TSharedRef<FShooterWidgetItem> Armors = MakeShareable(new FShooterWidgetItem(NULL,
		TEXT("Armors"),
		FString("Armors"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Shirt_08.INV_Shirt_08"), FVector2D(64, 64))),
		FString("ArmorsDescrip")
	));
	Categories.Add(Armors);
	TSharedRef<FShooterWidgetItem> Weapons = MakeShareable(new FShooterWidgetItem(NULL,
		TEXT("Weapons"),
		FString("Weapons"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_47.INV_Mace_47"), FVector2D(64, 64))),
		FString("WeaponsDescrip")
	));
	Categories.Add(Weapons);

	TSharedRef<FShooterWidgetItem> Skills = MakeShareable(new FShooterWidgetItem(NULL,
		TEXT("Skills"),
		FString("Skills"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_09.INV_Misc_Book_09"), FVector2D(64, 64))),
		FString("SkillsDescrip")
	));
	Categories.Add(Skills);

	TSharedRef<FShooterWidgetItem> Postions = MakeShareable(new FShooterWidgetItem(NULL,
		TEXT("Postions"),
		FString("Postions"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Postions/INV_Potion_08.INV_Potion_08"), FVector2D(64, 64))),
		FString("PostionsDescrip")
	));
	Categories.Add(Postions);
	//~~~~~~~~~~~~~~~~~~~

	//Armors Category
	FShooterWidgetItemPtr Armor = Armors;
	FShooterWidgetItemPtr EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy"),
		FString("Joy"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Leather_12.INV_Chest_Leather_12"), FVector2D(64, 64))),
		FString("JoyDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Song"),
		FString("Song"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Leather_13.INV_Chest_Leather_13"), FVector2D(64, 64))),
		FString("SongDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Rainbows"),
		FString("Rainbows"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_CHEST_MAIL_02.INV_CHEST_MAIL_02"), FVector2D(64, 64))),
		FString("RainbowsDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Butterflies"),
		FString("Butterflies"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_CHEST_MAIL_04.INV_CHEST_MAIL_04"), FVector2D(64, 64))),
		FString("SongDescrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Mail_06.INV_Chest_Mail_06"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Plate02.INV_Chest_Plate02"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Plate06.INV_Chest_Plate06"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Plate08.INV_Chest_Plate08"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Plate18.INV_Chest_Plate18"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
		TEXT("Joy2"),
		FString("Joy2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_Chest_Plate21.INV_Chest_Plate21"), FVector2D(64, 64))),
		FString("Joy2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Armors->AddSubDirectory(EachSubDir);
	for (int32 i = 0; i <= 35; i++) {
		EachSubDir = MakeShareable(new FShooterWidgetItem(Armor,
			TEXT("Dance") + FString::FormatAsNumber(i),
			TEXT("Dance") + FString::FormatAsNumber(i),
			MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Props/INV_CHEST_LEATHER_15.INV_CHEST_LEATHER_15"), FVector2D(64, 64))),
			FString("DanceDescrip"),
			TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
		));
		Armors->AddSubDirectory(EachSubDir);
	}
	
	//Weapons Category
	FShooterWidgetItemPtr Weapon = Weapons;
	FShooterWidgetItemPtr EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_40.INV_Mace_40"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Dance2"),
		FString("Dance2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_41.INV_Mace_41"), FVector2D(64, 64))),
		FString("Dance2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Rainbows2"),
		FString("Rainbows2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_42.INV_Mace_42"), FVector2D(64, 64))),
		FString("Rainbows2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Butterflies2"),
		FString("Butterflies2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_43.INV_Mace_43"), FVector2D(64, 64))),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Butterflies2"),
		FString("Butterflies2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_44.INV_Mace_44"), FVector2D(64, 64))),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Butterflies2"),
		FString("Butterflies2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_45.INV_Mace_45"), FVector2D(64, 64))),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Butterflies2"),
		FString("Butterflies2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_50.INV_Mace_50"), FVector2D(64, 64))),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);
	EachSubDir2 = MakeShareable(new FShooterWidgetItem(Weapon,
		TEXT("Butterflies2"),
		FString("Butterflies2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Weapons/INV_Mace_51.INV_Mace_51"), FVector2D(64, 64))),
		FString("Butterflies2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/PlayerPawn.PlayerPawn_C'")
	));
	Weapons->AddSubDirectory(EachSubDir2);

	//Skills Category
	FShooterWidgetItemPtr Skill = Skills;
	FShooterWidgetItemPtr EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_10.INV_Misc_Book_10"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_01.INV_Misc_Book_01"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_02.INV_Misc_Book_02"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_04.INV_Misc_Book_04"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_05.INV_Misc_Book_05"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_06.INV_Misc_Book_06"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_07.INV_Misc_Book_07"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_08.INV_Misc_Book_08"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);
	EachSubDir3 = MakeShareable(new FShooterWidgetItem(Skill,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Skills/INV_Misc_Book_11.INV_Misc_Book_11"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Skills->AddSubDirectory(EachSubDir3);

	//Postions Category
	FShooterWidgetItemPtr Postion = Postions;
	FShooterWidgetItemPtr EachSubDir4 = MakeShareable(new FShooterWidgetItem(Postion,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Postions/INV_Potion_142.INV_Potion_142"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Postions->AddSubDirectory(EachSubDir4);
	EachSubDir4 = MakeShareable(new FShooterWidgetItem(Postion,
		TEXT("Song2"),
		FString("Song2"),
		MakeShareable(new FShooterImageBrush(TEXT("/Game/UI/Icons/Menus/Postions/INV_Potion_148.INV_Potion_148"), FVector2D(64, 64))),
		FString("Song2Descrip"),
		TEXT("Blueprint'/Game/BluePrints/Pawns/HeroA.HeroA_C'")
	));
	Postions->AddSubDirectory(EachSubDir4);

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

void FShooterStoreViewModel::UpdateFilteredContentSourcesAndSelection(bool bAllowEmptySelection)
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
		FShooterWidgetItemPtr NewSelectedContentSource;
		if (bAllowEmptySelection == false && FilteredContentSource.Num() > 0)
		{
			NewSelectedContentSource = FilteredContentSource[0];
		}
		SetSelectedContent(NewSelectedContentSource);
	}
}

void FShooterStoreViewModel::TransformContentSourceToStrings(FShooterWidgetItemPtr Item, OUT TArray<FString>& Array)
{
	Array.Add(Item->GetName());
}

void FShooterStoreViewModel::SetSelectedCategory(FShooterWidgetItemPtr SelectedCategoryIn)
{
	SelectedCategory = SelectedCategoryIn;
	UpdateFilteredContentSourcesAndSelection(true);
	OnSelectedContentSourceChanged.ExecuteIfBound();
}

void FShooterStoreViewModel::SetSelectedContent(FShooterWidgetItemPtr SelectedContentSourceIn)
{
	// Ignore selecting the currently selected item.
	SelectedContent = SelectedContentSourceIn;
	OnSelectedContentSourceChanged.ExecuteIfBound();
}

const TArray<FShooterWidgetItemPtr>* FShooterStoreViewModel::GetCategories()
{
	return &Categories;
}

const TArray<FShooterWidgetItemPtr>* FShooterStoreViewModel::GetContentSources()
{
	return &FilteredContentSource;
}

FShooterWidgetItemPtr FShooterStoreViewModel::GetSelectedCategory()
{
	//return SelectedCategory;
	//FShooterWidgetItemPtr InSelectedCategory = SelectedCategory;
	if (SelectedCategory)
	{
		return SelectedCategory;
	}
	return TSharedPtr<FShooterWidgetItem>();
}

FShooterWidgetItemPtr FShooterStoreViewModel::GetSelectedContent()
{
	//FShooterWidgetItemPtr InSelectedContent = SelectedContent;
	if (SelectedContent)
	{
		return SelectedContent;
	}
	return TSharedPtr<FShooterWidgetItem>();
}

void FShooterStoreViewModel::ContentSourcesChanged()
{
	BuildContentSourceViewModels();
}

void FShooterStoreViewModel::SetOnViewSourceChanged(FOnViewSourceChanged OnViewSourceChangedIn)
{
	OnViewSourceChanged = OnViewSourceChangedIn;
}

void FShooterStoreViewModel::SetOnCategoriesChanged(FOnCategoriesChanged OnCategoriesChangedIn)
{
	OnCategoriesChanged = OnCategoriesChangedIn;
}

void FShooterStoreViewModel::SetOnContentSourcesChanged(FOnContentSourcesChanged OnContentSourcesChangedIn)
{
	OnContentSourcesChanged = OnContentSourcesChangedIn;
}

void FShooterStoreViewModel::SetOnSelectedContentSourceChanged(FOnSelectedContentSourceChanged OnSelectedContentSourceChangedIn)
{
	OnSelectedContentSourceChanged = OnSelectedContentSourceChangedIn;
}


