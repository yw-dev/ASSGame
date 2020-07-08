// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterMenuItemWidgetStyle.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "ShooterBlueprintLibrary.h"
#include "GenericPlatformChunkInstall.h"
#include "Online/ShooterOnlineGameSettings.h"
#include "OnlineSubsystemSessionSettings.h"
#include "Online/ShooterGame_Menu.h"
#include "Player/ShooterCharacter.h"
#include "SShooterStoreWidget.h"


#define LOCTEXT_NAMESPACE "SShooterStoreWidget"

class UShooterBlueprintLibrary;
class UShooterGameUserSettings;
class UAssetManager;

// Sets default values
void SShooterStoreWidget::Construct(const FArguments& InArgs)
{
	//OwnerHUD = InArgs._OwnerHUD;

	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	PlayerOwner = InArgs._PlayerOwner;
	OwnerWidget = InArgs._OwnerWidget;
	MatchState = InArgs._MatchState.Get();

	MenuHeaderHeight = 62.0f;

	PlayerOwner->bShowMouseCursor = true;
	//PlayerOwner->bEnableMouseOverEvents = true;
	PlayerOwner->bEnableClickEvents = true;

	FLinearColor MenuTitleTextColor = FLinearColor(FColor(155, 164, 182));

	GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	//PlayerOwner = OwnerHUD->GetGameInstance()->GetFirstGamePlayer();

	ViewModel = FShooterStoreViewModel::CreateShared();
	ViewModel->SetOnCategoriesChanged(FShooterStoreViewModel::FOnCategoriesChanged::CreateSP(
		this, &SShooterStoreWidget::CategoriesChanged));
	ViewModel->SetOnContentSourcesChanged(FShooterStoreViewModel::FOnContentSourcesChanged::CreateSP(
		this, &SShooterStoreWidget::ContentSourcesChanged));
	ViewModel->SetOnSelectedContentSourceChanged(FShooterStoreViewModel::FOnSelectedContentSourceChanged::CreateSP(
		this, &SShooterStoreWidget::SelectedContentSourceChanged));

	//Viewport Size
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	//Viewport Center!
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	StoreViewHeight = ViewportSize.Y - 400;
	StoreViewWidth = ViewportSize.X - 400;
	// read user settings
#if SHOOTER_CONSOLE_UI
	bIsLanMatch = FParse::Param(FCommandLine::Get(), TEXT("forcelan"));
#else
	//UShooterGameUserSettings* const UserSettings = CastChecked<UShooterGameUserSettings>(GEngine->GetGameUserSettings());
	//bIsLanMatch = UserSettings->IsLanMatch();
	//bIsDedicatedServer = UserSettings->IsDedicatedServer();
	bIsLanMatch = false;
	OutlineWidth = 20.0f;
	bIsDedicatedServer = true;
#endif

	//HUDStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");
	//SShooterStoreWidget::FOnSelectedContentSourceChanged::CreateSP(this, &SShooterStoreWidget::SelectedContentSourceChanged)
	// Slate Attribute binding.
	//Score.Bind(this, &SShooterStoreWidget::GetScore);
	//Health.Bind(this, &SShooterStoreWidget::GetHealth);

	// Fill view source.
	//RebuildViewSource();

	// Layout
	ChildSlot
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.HeightOverride(StoreViewHeight)
			.WidthOverride(StoreViewWidth)
			[
				SNew(SBorder)
				.BorderImage(&MenuStyle->RightBackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
					.ColorAndOpacity(FLinearColor(FColor(0.02f, 0.04f, 0.08f, 0.75f)))
					.Image(&MenuStyle->HeaderBackgroundBrush)
				]
			]
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(OutlineWidth))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(&MenuStyle->RightBackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
					.ColorAndOpacity(MenuTitleTextColor)
					.Text(LOCTEXT("MatchRestartTimeString", "商店"))
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoWidth()
				[
					SNew(SBorder)
					.BorderImage(&MenuStyle->LeftBackgroundBrush)
					.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(FMargin(OutlineWidth/2))
					[
						//SAssignNew(CategoryTileView, SBox)
						//[
						//	CreateCategoryTabs()
						//]
						CreateCategoryTileView()
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(OutlineWidth))
						[
							SNew(SBox)
							.HeightOverride(StoreViewHeight)
							.WidthOverride(StoreViewWidth / 3)
							[
								CreateContentTileView()
							]
						]
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.AutoWidth()
					[
						SNew(SBorder)
						.BorderImage(&MenuStyle->RightBackgroundBrush)
						.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(OutlineWidth))
						[
							SNew(SBox)
							.HeightOverride(StoreViewHeight)
							.WidthOverride(StoreViewWidth / 3)
							[
								SAssignNew(ContentDetailView, SBox)
								[
									CreateContentSourceDetail(ViewModel->GetSelectedContent())
								]
							]
						]
					]
				]
			]
		]
	];
}

SShooterStoreWidget::~SShooterStoreWidget()
{

}

TSharedRef<SWidget> SShooterStoreWidget::CreateCategoryTileView()
{
	UE_LOG(LogTemp, Warning, TEXT("StoreWidget::CreateCategoryTileView()"));
	SAssignNew(CategoryTileView, SShooterStoreTileView)
		.ListItemsSource(ViewModel->GetCategories())
		.OnGenerateTile(this, &SShooterStoreWidget::Category_OnGenerateTile)
		.OnSelectionChanged(this, &SShooterStoreWidget::Category_OnSelectionChanged)
		.ItemWidth(64)
		.ItemHeight(64)
		.SelectionMode(ESelectionMode::Single);
	CategoryTileView->SetSelection(ViewModel->GetSelectedCategory(), ESelectInfo::Direct);
	return CategoryTileView.ToSharedRef();
}

TSharedRef<ITableRow> SShooterStoreWidget::Category_OnGenerateTile(FShooterWidgetItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(STableRow<FShooterWidgetItemPtr>, OwnerTable)
			[
				SNew(STextBlock)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
				.Text(FText::FromString("THIS WAS NULL SOMEHOW"))
			];
	}
	return SNew(STableRow<FShooterWidgetItemPtr>, OwnerTable)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(FMargin(5))
			[
				SNew(SBox)
				.WidthOverride(64)
				.HeightOverride(64)
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SImage)
						.Image(Item->GetIconBrush().Get())
					]
				]
			]
			/*
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			.Padding(FMargin(3, 0, 3, 3))
			[
				SNew(STextBlock)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
				.Text(FText::FromString(Item->GetName()))
				//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
				.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-2, 2))
				.WrapTextAt(64)
				.Justification(ETextJustify::Center)
			]*/
		];
}

void SShooterStoreWidget::Category_OnSelectionChanged(FShooterWidgetItemPtr SelectedCategory, ESelectInfo::Type SelectInfo)
{
	//SetCurrentCategory(SelectedCategory);
	//UE_LOG(LogTemp, Warning, TEXT("Item Selected: %s"), *SelectedCategory->GetDisplayName());
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Category_OnSelectionChanged( %s )"), *SelectedCategory->GetName()));
	/*if (CurrentCategory->GetName() != SelectedCategory->GetName())
	{
		CurrentCategory = SelectedCategory;
		//const auto& SubCategories = SelectedCategory->GetSubDirectories();
		SetContentSource(SelectedCategory->AccessSubDirectories());
	}*/
	ViewModel->SetSelectedCategory(SelectedCategory);
}

void SShooterStoreWidget::SetContentSource(TArray<FShooterWidgetItemPtr> InContentSource)
{
	ContentSource = InContentSource;
}

TSharedRef<SWidget> SShooterStoreWidget::CreateContentTileView()
{
	UE_LOG(LogTemp, Warning, TEXT("StoreWidget::CreateContentTileView()"));
	SAssignNew(ContentTileView, SShooterStoreTileView)
		.ListItemsSource(ViewModel->GetContentSources())
		.OnGenerateTile(this, &SShooterStoreWidget::Content_OnGenerateTile)
		.OnSelectionChanged(this, &SShooterStoreWidget::Content_OnSelectionChanged)
		.ItemWidth(100)
		.ItemHeight(100)
		.SelectionMode(ESelectionMode::Single);
	ContentTileView->SetSelection(ViewModel->GetSelectedContent(), ESelectInfo::Direct);
	//ContentTileView->RequestListRefresh();
	return ContentTileView.ToSharedRef();
}

TSharedRef<ITableRow> SShooterStoreWidget::Content_OnGenerateTile(FShooterWidgetItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!Item.IsValid())
	{
		return SNew(STableRow<FShooterWidgetItemPtr>, OwnerTable)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(FMargin(OutlineWidth))
				[
					SNew(STextBlock)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
					.Text(FText::FromString("THIS WAS NULL SOMEHOW"))
				]
			];
	}
	return SNew(STableRow<FShooterWidgetItemPtr>, OwnerTable)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(64)
				.HeightOverride(64)
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SImage)
						.Image(Item->GetIconBrush().Get())
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
				.Text(FText::FromString(Item->GetName()))
				.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ShadowOffset(FIntPoint(-2, 2))
				.WrapTextAt(64)
				.Justification(ETextJustify::Center)
			]
		];
}

void SShooterStoreWidget::Content_OnSelectionChanged(FShooterWidgetItemPtr SelectedCategory, ESelectInfo::Type SelectInfo)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Category_OnSelectionChanged( %s )"), *SelectedCategory->GetName()));
	/*if (CurrentContent->GetName() != SelectedCategory->GetName())
	{
		CurrentContent = SelectedCategory;
		//const auto& SubCategories = SelectedCategory->GetSubDirectories();
		//SetContentSource(SelectedCategory->AccessSubDirectories());
	}*/
	ViewModel->SetSelectedContent(SelectedCategory);
}

TSharedRef<SWidget> SShooterStoreWidget::CreateContentSourceDetail(FShooterWidgetItemPtr ContentItem)
{
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);
	if (ContentItem.IsValid())
	{
		VerticalBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
					.WidthOverride(256)
					.HeightOverride(256)
					[
						SNew(SBorder)
						.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SImage)
							.Image(ContentItem->GetIconBrush().Get())
						]
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(SBox)
								.WidthOverride(32)
								.HeightOverride(32)
								[
									SNew(SBorder)
									.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SImage)
										.Image(ContentItem->GetIconBrush().Get())
									]
								]
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(FMargin(5, 0, 5, 5))
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
								.Text(FText::FromString(ContentItem->GetName()))
								//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
								.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
								.ShadowColorAndOpacity(FLinearColor::Black)
							]
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(5)
			[
				SNew(SBox)
				.HeightOverride(100)
				[
					SNew(SBorder)
					.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(5)
					[
						SNew(STextBlock)
						.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SNew(SButton)
				//.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
				.OnClicked(this, &SShooterStoreWidget::AddButtonClicked)
				.ContentPadding(FMargin(5))
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.AutoWidth()
					.Padding(0, 0, 2, 0)
					[
						SNew(STextBlock)
						.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.TextStyle(FShooterStyle::Get(), "ShooterGame.DefaultScoreboard.Row.ItemTextStyle")
						.Text(FText::FromString(ContentItem->GetName()))
						//.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 12))
						.ColorAndOpacity(FLinearColor(1, 0, 1, 1))
						.ShadowColorAndOpacity(FLinearColor::Black)
					]
				]
			]
		];
	}
	return VerticalBox;
}

FReply SShooterStoreWidget::AddButtonClicked()
{
	/*if (ViewModel->GetSelectedContent().IsValid())
	{
		ViewModel->GetSelectedContent()->GetContentSource()->InstallToProject("/Game");
	}*/
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ButtonClicked( %s )"), *ViewModel->GetSelectedContent()->GetName()));
	return FReply::Handled();
}

void SShooterStoreWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// Call parent implementation
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	//can do things here every tick
}

void SShooterStoreWidget::CategoriesChanged()
{
	CategoryTileView->RequestListRefresh();
}

void SShooterStoreWidget::ContentSourcesChanged()
{
	ContentTileView->RequestListRefresh();
}

void SShooterStoreWidget::SelectedContentSourceChanged()
{
	//ContentTileView->SetSelection(ViewModel->GetSelectedContent(), ESelectInfo::Direct);
	FShooterWidgetItemPtr Item = ViewModel->GetSelectedContent();
	ContentDetailView->SetContent(CreateContentSourceDetail(ViewModel->GetSelectedContent()));

	//AShooterPlayerController_Menu* PCM = Cast<AShooterPlayerController_Menu>(OwnerHUD->GetOwningPlayerController());
	// PCM 调用 Spawn  character 方法
	if (Item)
	{
		//SpawnPawnActor(Item);
	}
}




#undef LOCTEXT_NAMESPACE