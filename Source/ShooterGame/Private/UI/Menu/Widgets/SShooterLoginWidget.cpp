// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
#include "Engine/Console.h"
#include "ShooterMenuHelper.h"
#include "ShooterStyle.h"
#include "ShooterMenuWidgetStyle.h"
#include "ShooterUIHelpers.h"
#include "ShooterGameInstance.h"
#include "Player/ShooterLocalPlayer.h"
#include "ShooterGameUserSettings.h"
#include "Slate/SceneViewport.h"
#include "SShooterLoginWidget.h"


// Sets default values
void SShooterLoginWidget::Construct(const FArguments& InArgs)
{
	MenuStyle = &FShooterStyle::Get().GetWidgetStyle<FShooterMenuStyle>("DefaultShooterMenuStyle");

	bControlsLocked = false;
	//bConsoleVisible = false;
	//OutlineWidth = 20.0f;
	//SelectedIndex = 0;
	PlayerOwner = InArgs._PlayerOwner;
	bGameMenu = InArgs._IsGameMenu;
	//ControllerHideMenuKey = EKeys::Gamepad_Special_Right;
	//Visibility.Bind(this, &SShooterMenuWidget::GetSlateVisibility);
	FLinearColor MenuTitleTextColor = FLinearColor(FColor(155, 164, 182));
	//MenuHeaderHeight = 62.0f;
	//MenuHeaderWidth = 287.0f;

	// Calculate the size of the profile box based on the string it'll contain (+ padding)
	const FText PlayerName = PlayerOwner.IsValid() ? FText::FromString(PlayerOwner->GetNickname()) : FText::GetEmpty();
	const FText ProfileSwap = ShooterUIHelpers::Get().GetProfileSwapText();
	const TSharedRef< FSlateFontMeasure > FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const FSlateFontInfo PlayerNameFontInfo = FShooterStyle::Get().GetWidgetStyle<FTextBlockStyle>("ShooterGame.MenuProfileNameStyle").Font;
	const FSlateFontInfo ProfileSwapFontInfo = FShooterStyle::Get().GetWidgetStyle<FTextBlockStyle>("ShooterGame.MenuServerListTextStyle").Font;
	//MenuProfileWidth = FMath::Max(FontMeasure->Measure(PlayerName, PlayerNameFontInfo, 1.0f).X, FontMeasure->Measure(ProfileSwap.ToString(), ProfileSwapFontInfo, 1.0f).X) + 32.0f;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SShooterLoginWidget::GetBackgroundColor)
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 10.0f)
			.AutoHeight()
			[
				SNew(SEditableTextBox)
				.Visibility(EVisibility::Collapsed)
				.HintText(FText::FromString(TEXT("127.0.0.1")))
				.ToolTipText(FText::FromString(TEXT("Server IP Address.")))
				.ClearKeyboardFocusOnCommit(false)
				.OnTextCommitted_Lambda(
					[&](const FText& InText, ETextCommit::Type InCommitType)
					{
						if (InCommitType == ETextCommit::OnEnter)
						{
							//auto ActiveTabInfo = GetActiveTabInfo();
							//ScrollToText(ActiveTabInfo.ToSharedRef(), InText.ToString(), ActiveTabInfo->bLastFindWasUp);
						}
					})
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 10.0f)
			.AutoHeight()
			[
				SNew(SButton)
				.ButtonStyle(FCoreStyle::Get(), "Toolbar.Button")
				.ForegroundColor(FLinearColor(0.01f, 0.03f, 0.05f, 1.0f))
				.OnClicked_Lambda(
					[&]()
					{
						OnConnection();
						return FReply::Handled();
					})
				[
					SNew(STextBlock)
					.TextStyle(FShooterStyle::Get(), "ShooterGame.MenuHeaderTextStyle")
					.Text(FText::FromString(FString(TEXT("Connection"))))
				]
			]
		]
	];
}

// Called every frame
void SShooterLoginWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	//Always tick the super
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

}

// Called when the game starts or when spawned
FReply SShooterLoginWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//If we clicked anywhere, jump to the end
	//if (MenuWidgetAnimation.IsPlaying())
	//{
	//	MenuWidgetAnimation.JumpToEnd();
	//}

	//Set the keyboard focus 
	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SShooterLoginWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 UserIndex = InKeyEvent.GetUserIndex();
	bool bEventUserCanInteract = GetOwnerUserIndex() == -1 || UserIndex == GetOwnerUserIndex();

	if (!bControlsLocked && bEventUserCanInteract)
	{
		const FKey Key = InKeyEvent.GetKey();
		if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
		{
			//ControllerUpInputPressed();
			//int32 NextValidIndex = GetNextValidIndex(-1);
			//if (NextValidIndex != SelectedIndex)
			//{
			//	ButtonClicked(NextValidIndex);
			//}
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
		{
			//ControllerDownInputPressed();
			//int32 NextValidIndex = GetNextValidIndex(1);
			//if (NextValidIndex != SelectedIndex)
			//{
			//	ButtonClicked(NextValidIndex);
			//}
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
		{
			//ChangeOption(-1);
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
		{
			//ChangeOption(1);
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_FaceButton_Top)
		{
			//ProfileUISwap(UserIndex);
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Enter)
		{
			//ConfirmMenuItem();
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Virtual_Accept && !InKeyEvent.IsRepeat())
		{
			//ControllerFacebuttonDownPressed();
			//ConfirmMenuItem();
			Result = FReply::Handled();
		}
		else if ((Key == EKeys::Escape || Key == EKeys::Virtual_Back || Key == EKeys::Gamepad_Special_Left || Key == EKeys::Global_Back || Key == EKeys::Global_View) && !InKeyEvent.IsRepeat())
		{
			//MenuGoBack();
			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_FaceButton_Left)
		{
			//ControllerFacebuttonLeftPressed();
			Result = FReply::Handled();
		}
		else if ((Key == EKeys::Global_Play || Key == EKeys::Global_Menu) && !InKeyEvent.IsRepeat())
		{
			//OnToggleMenu.ExecuteIfBound();
			Result = FReply::Handled();
		}
	}
	return Result;
}

void SShooterLoginWidget::OnConnection()
{

}

FSlateColor SShooterLoginWidget::GetBackgroundColor() const
{
	return FLinearColor(0.02f, 0.04f, 0.08f, 0.2f);
}

void SShooterLoginWidget::LockControls(bool bEnable)
{
	bControlsLocked = bEnable;
}

int32 SShooterLoginWidget::GetOwnerUserIndex()
{
	return PlayerOwner.IsValid() ? PlayerOwner->GetControllerId() : 0;
}
