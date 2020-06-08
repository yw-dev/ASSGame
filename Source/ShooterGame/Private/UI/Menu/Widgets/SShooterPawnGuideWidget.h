// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "ShooterMenuHUD.h"
#include "ShooterGameInstance.h"
#include "ViewModel/ShooterPawnSourceViewModel.h"

// Lays out and controls the Tutorial HUD UI.


typedef STileView<FShooterPawnItemPtr> SShooterTileView;

class SShooterPawnGuideWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SShooterPawnGuideWidget)
	{
	}

	//SLATE_ARGUMENT(TWeakObjectPtr<class AShooterMenuHUD>, OwnerHUD);
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

	SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)

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
	~SShooterPawnGuideWidget();

	/** Returns the player that owns the main menu. */
	ULocalPlayer* GetPlayerOwner() const;

	/** Returns the controller id of player that owns the main menu. */
	int32 GetPlayerOwnerControllerId() const;

private:
	/** Handles the available categories changing on the view model. */
	void CategoriesChanged();

	/** Handles the available content sources changing on the view model. */
	void ContentSourcesChanged();

	/** Handles the selected content source changing on the view model. */
	void SelectedContentSourceChanged();

	void SpawnPawnActor(FShooterPawnItemPtr Item);

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
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	/** pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;

	/** style for the menu widget */
	const struct FShooterMenuStyle *MenuStyle;

	/** lan game? */
	bool bIsLanMatch;

	/** space between menu item and border */
	float OutlineWidth;

	/** Dedicated server? */
	bool bIsDedicatedServer;

private:

	/** The view model which represents the current data of the UI. */
	TSharedPtr<FShooterPawnSourceViewModel> ViewModel;

	/** The tile view of the category List. */
	TSharedPtr<SShooterTileView> CategoryTileView;

	/** The tile view of the category List. */
	TSharedPtr<SShooterTileView> ContentTileView;

	/** The placeholder widget which holds the detail view for the currently selected content source. */
	TSharedPtr<SBox> ContentDetailView;

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterPawnItemPtr> Categories;

	FShooterPawnItemPtr CurrentCategory;

	/** The Core Data for the Tree Viewer! */
	TArray<FShooterPawnItemPtr> ContentSource;

	FShooterPawnItemPtr CurrentContent;


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

	void SetContentSource(TArray<FShooterPawnItemPtr> InContentSource);

	/** Create Pawn Category List View. **/
	TSharedRef<SWidget> CreateCategoryTileView();

	/** Create Pawn Source List View by Category. **/
	TSharedRef<SWidget> CreateContentTileView();

	/** Creates a widget representing detailed information about a single content source. */
	TSharedRef<SWidget> CreateContentSourceDetail(FShooterPawnItemPtr ContentItem);

	/** Called to generate a widget for the specified Tile item */
	TSharedRef<ITableRow> Category_OnGenerateTile(FShooterPawnItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);
	
	/** Given a Tile item, fills an array of child items */
	void Category_OnSelectionChanged(FShooterPawnItemPtr SelectedCategory, ESelectInfo::Type SelectInfo);

	/** Called to generate a widget for the specified Tile item */
	TSharedRef<ITableRow> Content_OnGenerateTile(FShooterPawnItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** Given a Tile item, fills an array of child items */
	void Content_OnSelectionChanged(FShooterPawnItemPtr SelectedCategory, ESelectInfo::Type SelectInfo);

	/** SWidget overrides */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Handles the add content to project button being clicked. */
	FReply AddButtonClicked();


};
