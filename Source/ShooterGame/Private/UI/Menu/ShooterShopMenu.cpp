// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterShopMenu.h"
#include "ShooterStyle.h"
#include "ShooterMenuSoundsWidgetStyle.h"
#include "Online.h"
#include "OnlineExternalUIInterface.h"
#include "ShooterGameInstance.h"
#include "UI/ShooterHUD.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"

#if PLATFORM_SWITCH
#	define FRIENDS_SUPPORTED 0
#else
#	define FRIENDS_SUPPORTED 1
#endif


void FShooterShopMenu::Construct(ULocalPlayer* _PlayerOwner)
{
	PlayerOwner = _PlayerOwner;
	bIsGameMenuUp = false;

	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	//todo:  don't create ingame menus for remote players.
	const UShooterGameInstance* GameInstance = nullptr;
	if (PlayerOwner)
	{
		GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	}

	if (!GameMenuWidget.IsValid())
	{
		SAssignNew(GameMenuWidget, SShooterTabWidget)
			.PlayerOwner(MakeWeakObjectPtr(PlayerOwner))
			.Cursor(EMouseCursor::Default)
			.IsGameMenu(true);
		
		int32 const OwnerUserIndex = GetOwnerUserIndex();

		// setup the exit to main menu submenu.  We wanted a confirmation to avoid a potential TRC violation.
		// fixes TTP: 322267
		TSharedPtr<FShooterTabItem> MainMenuRoot = FShooterTabItem::CreateRoot();

		MainMenuItem = TabMenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Tab1", "功法"));
		TabMenuHelper::AddMenuItemSP(MainMenuItem, LOCTEXT("No", "NO"), this, &FShooterShopMenu::OnCancelExitToMain);
		TabMenuHelper::AddMenuItemSP(MainMenuItem, LOCTEXT("Yes", "YES"), this, &FShooterShopMenu::OnConfirmExitToMain);
		//TabMenuHelper::AddCustomMenuItem(LeaderboardItem, SAssignNew(LeaderboardWidget, SShooterLeaderboard).OwnerWidget(MenuWidget).PlayerOwner(GetPlayerOwner()));

		MainMenuItem = TabMenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Tab2", "装备"));
		TabMenuHelper::AddMenuItemSP(MainMenuItem, LOCTEXT("No", "NO"), this, &FShooterShopMenu::OnCancelExitToMain);
		TabMenuHelper::AddMenuItemSP(MainMenuItem, LOCTEXT("Yes", "YES"), this, &FShooterShopMenu::OnConfirmExitToMain);
		
		/*
		ShooterOptions = MakeShareable(new FShooterOptions());
		ShooterOptions->Construct(PlayerOwner);
		ShooterOptions->TellInputAboutKeybindings();
		ShooterOptions->OnApplyChanges.BindSP(this, &FShooterShopMenu::CloseSubMenu);

		TabMenuHelper::AddExistingMenuItem(RootMenuItem, ShooterOptions->CheatsItem.ToSharedRef());
		TabMenuHelper::AddExistingMenuItem(RootMenuItem, ShooterOptions->OptionsItem.ToSharedRef());
		*/
#if FRIENDS_SUPPORTED
		if (GameInstance && GameInstance->GetOnlineMode() == EOnlineMode::Online)
		{
#if !PLATFORM_XBOXONE
			/*
			ShooterFriends = MakeShareable(new FShooterFriends());
			ShooterFriends->Construct(PlayerOwner, OwnerUserIndex);
			ShooterFriends->TellInputAboutKeybindings();
			ShooterFriends->OnApplyChanges.BindSP(this, &FShooterShopMenu::CloseSubMenu);

			TabMenuHelper::AddExistingMenuItem(RootMenuItem, ShooterFriends->FriendsItem.ToSharedRef());

			ShooterRecentlyMet = MakeShareable(new FShooterRecentlyMet());
			ShooterRecentlyMet->Construct(PlayerOwner, OwnerUserIndex);
			ShooterRecentlyMet->TellInputAboutKeybindings();
			ShooterRecentlyMet->OnApplyChanges.BindSP(this, &FShooterShopMenu::CloseSubMenu);

			TabMenuHelper::AddExistingMenuItem(RootMenuItem, ShooterRecentlyMet->RecentlyMetItem.ToSharedRef());
		*/
#endif		

#if SHOOTER_CONSOLE_UI			
			TSharedPtr<FShooterTabItem> ShowInvitesItem = TabMenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Invite Players", "INVITE PLAYERS (via System UI)"));
			ShowInvitesItem->OnConfirmMenuItem.BindRaw(this, &FShooterShopMenu::OnShowInviteUI);
#endif
		}
#endif

		if (FSlateApplication::Get().SupportsSystemHelp())
		{
			TSharedPtr<FShooterTabItem> HelpSubMenu = TabMenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Help", "HELP"));
			HelpSubMenu->OnConfirmMenuItem.BindStatic([]() { FSlateApplication::Get().ShowSystemHelp(); });
		}

		TabMenuHelper::AddExistingMenuItem(RootMenuItem, MainMenuItem.ToSharedRef());

#if !SHOOTER_CONSOLE_UI
		TabMenuHelper::AddMenuItemSP(RootMenuItem, LOCTEXT("Quit", "QUIT"), this, &FShooterShopMenu::OnUIQuit);
#endif

		GameMenuWidget->MainMenu = GameMenuWidget->CurrentMenu = RootMenuItem->SubMenu;
		GameMenuWidget->OnMenuHidden.BindSP(this, &FShooterShopMenu::DetachGameMenu);
		GameMenuWidget->OnToggleMenu.BindSP(this, &FShooterShopMenu::ToggleGameMenu);
		//GameMenuWidget->OnGoBack.BindSP(this, &FShooterShopMenu::OnMenuGoBack);
	}
}

void FShooterShopMenu::CloseSubMenu()
{
	GameMenuWidget->MenuGoBack();
}

void FShooterShopMenu::OnMenuGoBack(MenuPtr Menu)
{
	// if we are going back from options menu
	/*
	if (ShooterOptions.IsValid() && ShooterOptions->OptionsItem->SubMenu == Menu)
	{
		ShooterOptions->RevertChanges();
	}*/
}

bool FShooterShopMenu::GetIsGameMenuUp() const
{
	return bIsGameMenuUp;
}

void FShooterShopMenu::UpdateFriendsList()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineFriendsPtr OnlineFriendsPtr = OnlineSub->GetFriendsInterface();
		if (OnlineFriendsPtr.IsValid())
		{
			OnlineFriendsPtr->ReadFriendsList(GetOwnerUserIndex(), EFriendsLists::ToString(EFriendsLists::OnlinePlayers));
		}
	}
}

void FShooterShopMenu::DetachGameMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(GameMenuContainer.ToSharedRef());
	}
	bIsGameMenuUp = false;

	AShooterPlayerController* const PCOwner = PlayerOwner ? Cast<AShooterPlayerController>(PlayerOwner->PlayerController) : nullptr;
	if (PCOwner)
	{
		PCOwner->SetPause(false);

		// If the game is over enable the scoreboard
		AShooterHUD* const ShooterHUD = PCOwner->GetShooterHUD();
		if ((ShooterHUD != NULL) && (ShooterHUD->IsMatchOver() == true) && (PCOwner->IsPrimaryPlayer() == true))
		{
			ShooterHUD->ShowScoreboard(true, true);
		}
	}
}

void FShooterShopMenu::ToggleGameMenu()
{
	//Update the owner in case the menu was opened by another controller
	//UpdateMenuOwner();

	if (!GameMenuWidget.IsValid())
	{
		return;
	}

	// check for a valid user index.  could be invalid if the user signed out, in which case the 'please connect your control' ui should be up anyway.
	// in-game menu needs a valid userindex for many OSS calls.
	if (GetOwnerUserIndex() == -1)
	{
		UE_LOG(LogShooter, Log, TEXT("Trying to toggle in-game menu for invalid userid"));
		return;
	}

	if (bIsGameMenuUp && GameMenuWidget->CurrentMenu != RootMenuItem->SubMenu)
	{
		GameMenuWidget->MenuGoBack();
		return;
	}

	AShooterPlayerController* const PCOwner = PlayerOwner ? Cast<AShooterPlayerController>(PlayerOwner->PlayerController) : nullptr;
	if (!bIsGameMenuUp)
	{
		// Hide the scoreboard
		if (PCOwner)
		{
			AShooterHUD* const ShooterHUD = PCOwner->GetShooterHUD();
			if (ShooterHUD != NULL)
			{
				ShooterHUD->ShowScoreboard(false);
			}
		}

		GEngine->GameViewport->AddViewportWidgetContent(
			SAssignNew(GameMenuContainer, SWeakWidget)
			.PossiblyNullContent(GameMenuWidget.ToSharedRef())
		);
		/*
		int32 const OwnerUserIndex = GetOwnerUserIndex();
		if (ShooterOptions.IsValid())
		{
			ShooterOptions->UpdateOptions();
		}
		if (ShooterRecentlyMet.IsValid())
		{
			ShooterRecentlyMet->UpdateRecentlyMet(OwnerUserIndex);
		}*/
		GameMenuWidget->BuildAndShowMenu();
		bIsGameMenuUp = true;

		if (PCOwner)
		{
			// Disable controls while paused
			PCOwner->SetCinematicMode(true, false, false, true, true);

			PCOwner->SetPause(true);
		}
	}
	else
	{
		//Start hiding animation
		GameMenuWidget->HideMenu();
		if (PCOwner)
		{
			// Make sure viewport has focus
			FSlateApplication::Get().SetAllUserFocusToGameViewport();

			// Don't renable controls if the match is over
			AShooterHUD* const ShooterHUD = PCOwner->GetShooterHUD();
			if ((ShooterHUD != NULL) && (ShooterHUD->IsMatchOver() == false))
			{
				PCOwner->SetCinematicMode(false, false, false, true, true);
			}
		}
	}
}

void FShooterShopMenu::OnCancelExitToMain()
{
	CloseSubMenu();
}

void FShooterShopMenu::OnConfirmExitToMain()
{
	UShooterGameInstance* const GameInstance = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LabelPlayerAsQuitter(PlayerOwner);

		// tell game instance to go back to main menu state
		GameInstance->GotoState(ShooterGameInstanceState::MainMenu);
	}
}

void FShooterShopMenu::OnUIQuit()
{
	//Start hiding animation
	ToggleGameMenu();
	/*
	UShooterGameInstance* const GI = Cast<UShooterGameInstance>(PlayerOwner->GetGameInstance());
	if (GI)
	{
		GI->LabelPlayerAsQuitter(PlayerOwner);
	}
	*/
	//GameMenuWidget->LockControls(true);
	//GameMenuWidget->HideMenu();
	/*
	UWorld* const World = PlayerOwner ? PlayerOwner->GetWorld() : nullptr;
	if (World)
	{
		const FShooterMenuSoundsStyle& MenuSounds = FShooterStyle::Get().GetWidgetStyle<FShooterMenuSoundsStyle>("DefaultShooterMenuSoundsStyle");
		TabMenuHelper::PlaySoundAndCall(World, MenuSounds.ExitGameSound, GetOwnerUserIndex(), this, &FShooterShopMenu::Quit);
	}*/
}

void FShooterShopMenu::Quit()
{
	APlayerController* const PCOwner = PlayerOwner ? PlayerOwner->PlayerController : nullptr;
	if (PCOwner)
	{
		PCOwner->ConsoleCommand("quit");
	}
}

void FShooterShopMenu::OnShowInviteUI()
{
	const auto ExternalUI = Online::GetExternalUIInterface();

	if (!ExternalUI.IsValid())
	{
		UE_LOG(LogShooter, Warning, TEXT("OnShowInviteUI: External UI interface is not supported on this platform."));
		return;
	}

	ExternalUI->ShowInviteUI(GetOwnerUserIndex());
}

int32 FShooterShopMenu::GetOwnerUserIndex() const
{
	return PlayerOwner ? PlayerOwner->GetControllerId() : 0;
}


#undef LOCTEXT_NAMESPACE
