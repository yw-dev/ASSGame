// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "SShooterOptionWidget.h"
#include "ShooterStyle.h"
#include "ShooterMenuWidgetStyle.h"


#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

static const FString Options[] = { TEXT("New"), TEXT("Save"), TEXT("Load") };

void SShooterOptionWidget::Construct(const FArguments& InArgs)
{
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultDedicatedMenuStyle");

	PlayerOwner = InArgs._PlayerOwner;
	OwnerWidget = InArgs._OwnerWidget;

	BoxWidth = 125;

	bIsActiveMenu = false;

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SBox)
		.WidthOverride(600)
		.HeightOverride(400)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonBgColor)
				.Image(&MenuStyle->HeaderBackgroundBrush)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Fill)
					[
						SNew(STextBlock)
						.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
						.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
						.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
						.Text(FText::FromString(TEXT("System")))
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Fill)
					.Padding(2)
					[
						SNew(SBox)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SBorder)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.Padding(FMargin(15, 0, 15, 0))
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
									.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
									.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
									.Text(FText::FromString(TEXT("Graphics")))
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
									.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
									.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
									.Text(FText::FromString(TEXT("Advance")))
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
									.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
									.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
									.Text(FText::FromString(TEXT("Network")))
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
									.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
									.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
									.Text(FText::FromString(TEXT("Language")))
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.TextStyle(FShooterStyle::Get(), "DedicatedGame.ScoreboardListTextStyle")
									.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
									.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
									.Text(FText::FromString(TEXT("Voice")))
								]
							]
						]
					]
					+ SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(2)
					[
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(5))
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Fill)
							.Padding(0, 5, 0, 0)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListCheckboxTextStyle")
								.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
								.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
								.Text(FText::FromString(TEXT("Display")))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Fill)
							[
								SNew(SBorder)
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								.Padding(5)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									.Padding(1)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("ViewMode")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("Window")))
												]
											]
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Screen")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("MainDisplay")))
												]
											]
										]
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("DPI")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("1920*1080")))
												]
											]
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("MSAA")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("2x")))
												]
											]
										]
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Screen Hz")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("60Hz")))
												]
											]
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Right)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Vertical Sync")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Left)
										.VAlign(VAlign_Center)
										.Padding(1)
										[
											SNew(SBox)
											.WidthOverride(80)
											[
												SNew(SBorder)
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Fill)
												[
													SNew(STextBlock)
													.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
													.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
													.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
													.Text(FText::FromString(TEXT("OFF")))
												]
											]
										]
									]
								]
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Fill)
							.Padding(0, 5, 0, 0)
							[
								SNew(STextBlock)
								.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListCheckboxTextStyle")
								.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
								.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
								.Text(FText::FromString(TEXT("Graphics")))
							]
							+ SVerticalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(SBorder)
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								.Padding(5)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.AutoHeight()
									.HAlign(HAlign_Fill)
									.Padding(2)
									[
										SNew(SSlider)
									]
									+ SVerticalBox::Slot()
									.AutoHeight()
									.HAlign(HAlign_Fill)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Low")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Medium")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Hight")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Epic")))
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
											.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
											.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
											.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
											.Text(FText::FromString(TEXT("Cinematic")))
										]
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									.Padding(2)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Fill)
										.Padding(0, 0, 3, 0)
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
												.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
												.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
												.Text(FText::FromString(TEXT("Texture")))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Fill)
											[
												SNew(SBorder)
												.HAlign(HAlign_Fill)
												.VAlign(VAlign_Fill)
												.Padding(5)
												[
													SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Distinguishability")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Filter")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Trilinear")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Projection")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("OFF")))
																]
															]
														]
													]
												]
											]
										]
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Fill)
										.Padding(3, 0, 0, 0)
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
												.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
												.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
												.Text(FText::FromString(TEXT("Effect")))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Fill)
											[
												SNew(SBorder)
												.HAlign(HAlign_Fill)
												.VAlign(VAlign_Fill)
												.Padding(5)
												[
													SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Shadow Effect")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Low")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Fluid Effect")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Light")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("ON")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Particle Density")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
												]
											]
										]
									]
									+ SVerticalBox::Slot()
									.AutoHeight()
									.HAlign(HAlign_Left)
									.Padding(2)
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Top)
										.Padding(2)
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
												.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
												.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
												.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
												.Text(FText::FromString(TEXT("Environment")))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Fill)
											[
												SNew(SBorder)
												.HAlign(HAlign_Fill)
												.VAlign(VAlign_Fill)
												.Padding(5)
												[
													SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Sight Range")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Details")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Fill)
													.VAlign(VAlign_Fill)
													[
														SNew(SHorizontalBox)
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Right)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(STextBlock)
															.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
															.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
															.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
															.Text(FText::FromString(TEXT("Landscape")))
														]
														+ SHorizontalBox::Slot()
														.HAlign(HAlign_Left)
														.VAlign(VAlign_Center)
														.Padding(1)
														[
															SNew(SBox)
															.WidthOverride(80)
															[
																SNew(SBorder)
																.HAlign(HAlign_Right)
																.VAlign(VAlign_Fill)
																[
																	SNew(STextBlock)
																	.TextStyle(FShooterStyle::Get(), "DedicatedGame.DemoListDetialTextStyle")
																	.ColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextColor)
																	.ShadowColorAndOpacity(this, &SShooterOptionWidget::GetButtonTextShadowColor)
																	.Text(FText::FromString(TEXT("Normal")))
																]
															]
														]
													]
												]
											]
										]
									]
								]
							]
						]
					]
				]
			]
		]
	];
}

void SShooterOptionWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	//Always tick the super
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FSlateColor SShooterOptionWidget::GetButtonBgColor() const
{
	return FLinearColor(0.f, 0.f, 0.f, 0.35f);
}


FSlateColor SShooterOptionWidget::GetButtonTextColor() const
{
	return FLinearColor(FColor(155, 164, 182));
}

FLinearColor SShooterOptionWidget::GetButtonTextShadowColor() const
{
	FLinearColor Result;
	if (bIsActiveMenu)
	{
		Result = FLinearColor(0, 0, 0, 1);
	}
	else
	{
		Result = FLinearColor(0, 0, 0, 0.5f);
	}
	return Result;
}


#undef LOCTEXT_NAMESPACE

