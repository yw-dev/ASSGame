// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GenericApplication.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterTypes.h"
#include "ShooterTabItem.h"
#include "SShooterTabWidget.h"


/**
 * 通用 Tab Menu Helper
 */
namespace TabHelper
{

	//************************ Menu Item begin ************************
	FORCEINLINE void EnsureValid(TSharedPtr<FShooterTabItem>& MenuItem)
	{
		if (!MenuItem.IsValid())
		{
			MenuItem = FShooterTabItem::CreateRoot();
		}
	}

	//Helper functions for creating menu items
	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuItem(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text));
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	/** add standard item to menu with UObject delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuItem(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text, UserClass* inObj, typename FShooterTabItem::FOnConfirmMenuItem::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text));
		Item->OnConfirmMenuItem.BindUObject(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	/** add standard item to menu with TSharedPtr delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuItemSP(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text, UserClass* inObj, typename FShooterTabItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text));
		Item->OnConfirmMenuItem.BindSP(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuOption(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text, OptionsList));
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	/** add multi-choice item to menu with UObject delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuOption(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList, UserClass* inObj, typename FShooterTabItem::FOnOptionChanged::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text, OptionsList));
		Item->OnOptionChanged.BindUObject(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	/** add multi-choice item to menu with TSharedPtr delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterTabItem> AddMenuOptionSP(TSharedPtr<FShooterTabItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList, UserClass* inObj, typename FShooterTabItem::FOnOptionChanged::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterTabItem> Item = MakeShareable(new FShooterTabItem(Text, OptionsList));
		Item->OnOptionChanged.BindSP(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterTabItem> AddExistingMenuItem(TSharedPtr<FShooterTabItem>& MenuItem, TSharedRef<FShooterTabItem> SubMenuItem)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(SubMenuItem);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterTabItem> AddCustomMenuItem(TSharedPtr<FShooterTabItem>& MenuItem, TSharedPtr<SWidget> CustomWidget)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(MakeShareable(new FShooterTabItem(CustomWidget, EShooterCustomWidget::None)));
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterTabItem> AddCustomWidget(TSharedPtr<FShooterTabItem>& MenuItem, TSharedPtr<SWidget> CustomWidget, EShooterCustomWidget::Type WidgetType)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(MakeShareable(new FShooterTabItem(CustomWidget, WidgetType)));
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	FORCEINLINE void ClearSubMenu(TSharedPtr<FShooterTabItem>& MenuItem)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Empty();
	}

	template< class UserClass >
	FORCEINLINE void PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, int32 UserIndex, UserClass* inObj, typename FShooterTabItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound, UserIndex);
		if (World)
		{
			const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
			FTimerHandle DummyHandle;
			World->GetTimerManager().SetTimer(DummyHandle, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);
		}
		else
		{
			FTimerDelegate D = FTimerDelegate::CreateSP(inObj, inMethod);
			D.ExecuteIfBound();
		}
	}

	//************************ Menu Item end ************************


}
