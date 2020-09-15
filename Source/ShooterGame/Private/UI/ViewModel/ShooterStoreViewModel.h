// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/TextFilter.h"
#include "IShooterSource.h"
#include "ShooterSourceModel.h"
#include "Brushes/SlateDynamicImageBrush.h"

typedef TSharedPtr<class FShooterWidgetItem> FShooterWidgetItemPtr;

/**
 * The Data for a single node in the Directory Tree
 */
class FShooterWidgetItem
{

public:

	/** @return Returns the parent or NULL if this is a root */
	const FShooterWidgetItemPtr GetParentCategory() const
	{
		return ParentElement.Pin();
	}

	/** @return the path on hard disk, read-only */
	const FString& GetDirectoryPath() const
	{
		return DirectoryPath;
	}

	/** @return name to Categoty in file tree view! read-only */
	const FString& GetName() const
	{
		return Name;
	}

	/** @return Description to Categoty in file tree view! read-only */
	const FString& GetDescrip() const
	{
		return Descrip;
	}

	/** @return name to Categoty in file tree view! read-only */
	const TSharedPtr<FSlateBrush> GetIconBrush() const
	{
		return IconBrush;
	}

	/** @return name to Categoty in file tree view! read-only */
	const FString& GetPawn() const
	{
		return Pawn;
	}

	/** @return Returns all subdirectories, read-only */
	const TArray<FShooterWidgetItemPtr> GetSubDirectories() const
	{
		return SubDirectories;
	}

	/** @return Returns all subdirectories, read or write */
	TArray<FShooterWidgetItemPtr> AccessSubDirectories()
	{
		return SubDirectories;
	}

	/** Add a subdirectory to this node in the tree! */
	void AddSubDirectory(const FShooterWidgetItemPtr NewSubDir)
	{
		SubDirectories.Add(NewSubDir);
	}

public:

	/** Constructor for FShooterPawnItem */
	FShooterWidgetItem(FShooterWidgetItemPtr IN_ParentElement,
		FString IN_DirectoryPath,
		FString IN_Name,
		TSharedPtr<FSlateBrush> IN_IconBrush,
		FString IN_Descrip,
		FString IN_Pawn
	)
		: ParentElement(IN_ParentElement)
		, DirectoryPath(IN_DirectoryPath)
		, Name(IN_Name)
		, IconBrush(IN_IconBrush)
		, Descrip(IN_Descrip)
		, Pawn(IN_Pawn)
	{
	}


private:
	/** Parent item or NULL if this is a root  */
	TWeakPtr<FShooterWidgetItem> ParentElement;

	/** Full path of this directory in the tree */
	FString DirectoryPath;

	/** Categoty name of the category */
	FString Name;

	TSharedPtr<FSlateBrush> IconBrush;

	//FString CategotyIconPath;

	FString Descrip;

	FString Pawn;

	/** Child categories */
	TArray<FShooterWidgetItemPtr> SubDirectories;
};

class FShooterStoreViewModel : public TSharedFromThis<FShooterStoreViewModel>
{

public:
	DECLARE_DELEGATE(FOnViewSourceChanged)
	DECLARE_DELEGATE(FOnCategoriesChanged)
	DECLARE_DELEGATE(FOnContentSourcesChanged);
	DECLARE_DELEGATE(FOnSelectedContentSourceChanged);

	typedef TTextFilter<FShooterWidgetItemPtr> ContentSourceTextFilter;

	/** Creates a shared reference to a new view model. */
	static TSharedRef<FShooterStoreViewModel> CreateShared();

	/** Sets the currently selected category view model. Only content sources which match the selected
		category will be returned by GetContentSources(). */
	void SetSelectedCategory(FShooterWidgetItemPtr SelectedCategoryIn);

	/** Sets the currently selected content source. */
	void SetSelectedContent(FShooterWidgetItemPtr SelectedContentSourceIn);

	/** Gets the view models for the current set of content source categories. */
	const TArray<FShooterWidgetItemPtr>* GetCategories();

	/** Gets a filtered array of content sources which match both the selected category and the search
		text if it has been set. */
	const TArray<FShooterWidgetItemPtr>* GetContentSources();

	/** Gets the view model for the currently selected category.  Only content sources which match
		the selected category will be returned by GetSelectedCategory(). */
	FShooterWidgetItemPtr GetSelectedCategory();

	/** Gets the currently selected content source. */
	FShooterWidgetItemPtr GetSelectedContent();

	/** Sets the delegate which should be executed when the set of categories changes. */
	void SetOnViewSourceChanged(FOnViewSourceChanged OnViewSourceChangedIn);

	/** Sets the delegate which should be executed when the set of categories changes. */
	void SetOnCategoriesChanged(FOnCategoriesChanged OnCategoriesChangedIn);

	/** Sets the delegate which should be executed when the current set of content sources returned by
		GetContentSources changes. */
	void SetOnContentSourcesChanged(FOnContentSourcesChanged OnContentSourcesChangedIn);

	/** Sets the delegate which should be executed when the selected content source changes. */
	void SetOnSelectedContentSourceChanged(FOnSelectedContentSourceChanged OnSelectedContentSourceChangedIn);


private:

	FShooterStoreViewModel();

	void Initialize();

	/** Builds view models for all available content sources. */
	void BuildContentSourceViewModels();

	/** Filters the current set of content sources based on the selected category and the search text, and updates the selection.
	  * @param bAllowEmptySelection Determines whether or not to select the first available item after filtering if the selected item is null, or not in the filtered set. */
	void UpdateFilteredContentSourcesAndSelection(bool bAllowEmptySelection);

	/** Converts a content source item to an array of strings for processing by the TTextFilter. */
	void TransformContentSourceToStrings(FShooterWidgetItemPtr Item, OUT TArray< FString >& Array);

	/** Handles notification from the content source providers when their content source arrays change. */
	void ContentSourcesChanged();

private:

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterWidgetItemPtr> Categories;

	//FShooterWidgetItemPtr CurrentCategory;

	/** The view model for the currently selected category. */
	FShooterWidgetItemPtr SelectedCategory;

	/** The Core Data for the Tree Viewer! */
	//TArray<FShooterWidgetItemPtr> ContentSource;

	//FShooterWidgetItemPtr CurrentContent;

	/** The view model for the currently selected Content Pawn. */
	FShooterWidgetItemPtr SelectedContent;

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterWidgetItemPtr> FilteredContentSource;

	/** The delegate which is executed when the available ViewSource change. */
	FOnViewSourceChanged OnViewSourceChanged;

	/** The delegate which is executed when the available categories change. */
	FOnCategoriesChanged OnCategoriesChanged;

	/** The delegate which is executed when the filtered content sources change. */
	FOnContentSourcesChanged OnContentSourcesChanged;

	/** The delegate which is executed when the currently selected content source changes. */
	FOnSelectedContentSourceChanged OnSelectedContentSourceChanged;

	/** The filter which is used to filter the content sources based on the search text. */
	TSharedPtr<ContentSourceTextFilter> ContentSourceFilter;





};







