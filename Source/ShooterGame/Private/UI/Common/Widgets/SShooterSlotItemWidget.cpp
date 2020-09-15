// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterSlotItemWidgetStyle.h"
#include "SShooterSlotItemWidget.h"

#define LOCTEXT_NAMESPACE "UMG"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
// Sets default values
void SShooterSlotItemWidget::Construct(const FArguments& InArgs)
{
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::Construct()"));

	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterSlotItemStyle>("DefaultShooterSlotItemStyle");

	Title = InArgs._Title;
	ImageIcon = InArgs._ImageIcon;
	LabelText = InArgs._LabelText;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	ButtonClicked = InArgs._ButtonClicked;
	OnMouseButtonDownHandler = InArgs._OnMouseButtonDown;

	ItemPadding = 10;
	LabelText = LOCTEXT("Play", "开始游戏");

	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.Padding(FMargin(ItemPadding.Get()))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SButton)
			.ButtonStyle(&MenuStyle->SlotItemStyle)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.HeightOverride(128)
				.WidthOverride(128)
				[
					SNew(SImage)
					//.Image_Lambda([this] {return ImageIcon.Get(); })
					.Image(ImageIcon)
				]
			]
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(2)
		.AutoHeight()
		[
			SNew(STextBlock)
			.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuButtonTextNormalStyle")
			//.Text(LabelText)
			.Text_Lambda([this] { return FText::FromString(Title.Get()); })
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

//////////////////////////////////////////////////////////////////
/** Widget Attribute Function */

void SShooterSlotItemWidget::SetItemPadding(const TAttribute<float>& InVal)
{
	ItemPadding = InVal;
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::SetItemPadding( ItemPadding = %d)"), ItemPadding.Get());
}

void SShooterSlotItemWidget::SetItemPadding(float InVal)
{
	ItemPadding = InVal;
}

void SShooterSlotItemWidget::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	if (!ColorAndOpacity.IdenticalTo(InColorAndOpacity))
	{
		ColorAndOpacity = InColorAndOpacity;
		Invalidate(EInvalidateWidget::PaintAndVolatility);
	}
}

void SShooterSlotItemWidget::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	if (!ColorAndOpacity.IdenticalTo(InColorAndOpacity))
	{
		ColorAndOpacity = InColorAndOpacity;
		Invalidate(EInvalidateWidget::PaintAndVolatility);
	}
}

void SShooterSlotItemWidget::SetLabelText(FText InLabel)
{
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::SetLabelText()"));
	LabelText = InLabel;
}

void SShooterSlotItemWidget::SetLabelText(const TAttribute<FText>& InLabel)
{
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::SetLabelText()"));
		LabelText = InLabel;
}

void SShooterSlotItemWidget::SetLabelText(const FText& InLabel)
{
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::SetLabelText()"));
		LabelText = InLabel;
}


void SShooterSlotItemWidget::SetImage(TAttribute<const FSlateBrush*> InImage)
{
	UE_LOG(LogTemp, Warning, TEXT("SShooterSlotItemWidget::SetImage()"));
	if (!ImageIcon.IdenticalTo(InImage))
	{
		ImageIcon = InImage;
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

void SShooterSlotItemWidget::SetButtonClicked(FOnClicked InButtonClicked)
{
	ButtonClicked = InButtonClicked;
}

void SShooterSlotItemWidget::SetOnMouseButtonDown(FPointerEventHandler EventHandler)
{
	OnMouseButtonDownHandler = EventHandler;
}


#undef LOCTEXT_NAMESPACE
