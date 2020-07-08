// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Slate.h"
#include "ShooterTypes.h"
#include "ShooterMenuHUD.h"
#include "ShooterGameInstance.h"
#include "ViewModel/ShooterStoreViewModel.h"

typedef STileView<FShooterWidgetItemPtr> SShooterStoreTileView;

// Lays out and controls the HUD UI.
class SShooterStoreWidget : public SCompoundWidget
{

	SLATE_BEGIN_ARGS(SShooterStoreWidget)
	{
	}

	//SLATE_ARGUMENT(TWeakObjectPtr<class AShooterMenuHUD>, OwnerHUD);
	SLATE_ARGUMENT(TWeakObjectPtr<APlayerController>, PlayerOwner)

	SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)

	SLATE_ATTRIBUTE(EShooterMatchState::Type, MatchState)

	SLATE_END_ARGS()

public:

	/** play menu item delegate */
	DECLARE_DELEGATE(FOnBeginPlayMenu);

	DECLARE_DELEGATE(FOnCategoryChanged)
	DECLARE_DELEGATE(FOnContentSourcesChanged);
	DECLARE_DELEGATE(FOnSelectedContentSourceChanged);

	/**
	 * Constructs and lays out the Tutorial HUD UI Widget.
	 *
	 * \args Arguments structure that contains widget-specific setup information.
	 **/
	void Construct(const FArguments& args);

	/** Destructor */
	~SShooterStoreWidget();

private:
	/** Handles the available categories changing on the view model. */
	void CategoriesChanged();

	/** Handles the available content sources changing on the view model. */
	void ContentSourcesChanged();

	/** Handles the selected content source changing on the view model. */
	void SelectedContentSourceChanged();

	void SpawnPawnActor(FShooterWidgetItemPtr Item);

	/** Hosts a game, using the passed in game type */
	void HostGame(const FString& GameType);

	/** Hosts team deathmatch game */
	void HostTeamDeathMatch();

	/** Checks the ChunkInstaller to see if the selected map is ready for play */
	bool IsMapReady() const;

	/** callback for when one of the N buttons is clicked */
	FReply OnBeginPlay() const;

protected:

	enum class EMap
	{
		ESancturary,
		EHighRise,
		EMax,
	};

	/** Owning game instance */
	TWeakObjectPtr<UShooterGameInstance> GameInstance;

	/** Owning player */
	TWeakObjectPtr<class APlayerController> PlayerOwner;

	/** pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;

	/** get state of current match */
	EShooterMatchState::Type MatchState;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;

	/** menu header height */
	float MenuHeaderHeight;

	/** menu header height */
	float StoreViewHeight;

	/** menu header height */
	float StoreViewWidth;

	/** lan game? */
	bool bIsLanMatch;

	/** space between menu item and border */
	float OutlineWidth;

	/** Dedicated server? */
	bool bIsDedicatedServer;

private:

	/** The view model which represents the current data of the UI. */
	TSharedPtr<FShooterStoreViewModel> ViewModel;

	/** The tile view of the category List. */
	TSharedPtr<SShooterStoreTileView> CategoryTileView;

	/** The tile view of the category List. */
	TSharedPtr<SShooterStoreTileView> ContentTileView;

	/** The placeholder widget which holds the detail view for the currently selected content source. */
	TSharedPtr<SBox> ContentDetailView;

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterWidgetItemPtr> Categories;

	FShooterWidgetItemPtr CurrentCategory;

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterWidgetItemPtr> ContentSource;

	FShooterWidgetItemPtr CurrentContent;

	/**
	 * Stores a weak reference to the HUD owning this widget.
	 **/
	TWeakObjectPtr<class AShooterMenuHUD> OwnerHUD;

	/**
	 * A reference to the Slate Style used for this HUD's widgets.
	 **/
	 //const struct FGlobalStyle* HUDStyle;
	 /**
	  * Attribute storing the binding for the player's score.
	  **/
	TAttribute<FText> Score;

	/**
	 * Attribute storing the binding for the player's health.
	 **/
	TAttribute<FText> Health;

	/**
	 * Attribute storing the binding for the Pawn's preview point.
	 **/
	TAttribute<AActor*> PreviewActor;

	/**
	 * Attribute storing the binding for the Pawn's preview List.
	 **/
	TAttribute<TArray<AActor*>> PreviewActors;

	/**
	 * Attribute storing the binding for the player's health.
	 **/
	//TAttribute<FShooterStoreViewModel> StoreViewModel;

	/**
	 * Our Score will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetScore() const;

	/**
	 * Our Health will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	FText GetHealth() const;

	/**
	 * Our Health will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
	 **/
	 //AActor* GetPreviewPoint() const;

	void LoadActorResCallBack();

	void SetContentSource(TArray<FShooterWidgetItemPtr> InContentSource);

	/** Create Pawn Category List View. **/
	TSharedRef<SWidget> CreateCategoryTileView();

	/** Create Pawn Source List View by Category. **/
	TSharedRef<SWidget> CreateContentTileView();

	/** Creates a widget representing detailed information about a single content source. */
	TSharedRef<SWidget> CreateContentSourceDetail(FShooterWidgetItemPtr ContentItem);

	/** Called to generate a widget for the specified Tile item */
	TSharedRef<ITableRow> Category_OnGenerateTile(FShooterWidgetItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** Given a Tile item, fills an array of child items */
	void Category_OnSelectionChanged(FShooterWidgetItemPtr SelectedCategory, ESelectInfo::Type SelectInfo);

	/** Called to generate a widget for the specified Tile item */
	TSharedRef<ITableRow> Content_OnGenerateTile(FShooterWidgetItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** Given a Tile item, fills an array of child items */
	void Content_OnSelectionChanged(FShooterWidgetItemPtr SelectedCategory, ESelectInfo::Type SelectInfo);

	/** SWidget overrides */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Handles the add content to project button being clicked. */
	FReply AddButtonClicked();

	
};
