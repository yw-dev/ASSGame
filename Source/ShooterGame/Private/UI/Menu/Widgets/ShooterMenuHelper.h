// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GenericApplication.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterMenuItem.h"
#include "SShooterMenuWidget.h"


struct FShooterBitmapBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FShooterBitmapBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		FSlateBrush::AddReferencedObjects(Collector);
	}
};

/**
 * 通用 Menu Helper
 */
namespace MenuHelper
{


	//************************ Menu Item begin ************************
	FORCEINLINE void EnsureValid(TSharedPtr<FShooterMenuItem>& MenuItem)
	{
		if (!MenuItem.IsValid())
		{
			MenuItem = FShooterMenuItem::CreateRoot();
		}
	}

	//Helper functions for creating menu items
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItem(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	/** add standard item to menu with UObject delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItem(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text, UserClass* inObj, typename FShooterMenuItem::FOnConfirmMenuItem::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		Item->OnConfirmMenuItem.BindUObject(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	/** add standard item to menu with TSharedPtr delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuItemSP(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text, UserClass* inObj, typename FShooterMenuItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text));
		Item->OnConfirmMenuItem.BindSP(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuOption(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text, OptionsList));
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	/** add multi-choice item to menu with UObject delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuOption(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList, UserClass* inObj, typename FShooterMenuItem::FOnOptionChanged::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text, OptionsList));
		Item->OnOptionChanged.BindUObject(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	/** add multi-choice item to menu with TSharedPtr delegate */
	template< class UserClass >
	FORCEINLINE TSharedRef<FShooterMenuItem> AddMenuOptionSP(TSharedPtr<FShooterMenuItem>& MenuItem, const FText& Text, const TArray<FText>& OptionsList, UserClass* inObj, typename FShooterMenuItem::FOnOptionChanged::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(MenuItem);
		TSharedPtr<FShooterMenuItem> Item = MakeShareable(new FShooterMenuItem(Text, OptionsList));
		Item->OnOptionChanged.BindSP(inObj, inMethod);
		MenuItem->SubMenu.Add(Item);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterMenuItem> AddExistingMenuItem(TSharedPtr<FShooterMenuItem>& MenuItem, TSharedRef<FShooterMenuItem> SubMenuItem)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(SubMenuItem);
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterMenuItem> AddCustomMenuItem(TSharedPtr<FShooterMenuItem>& MenuItem, TSharedPtr<SWidget> CustomWidget)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(MakeShareable(new FShooterMenuItem(CustomWidget, EShooterCustomWidget::None)));
		return MenuItem->SubMenu.Last().ToSharedRef();
	}


	FORCEINLINE TSharedRef<FShooterMenuItem> AddCustomWidget(TSharedPtr<FShooterMenuItem>& MenuItem, TSharedPtr<SWidget> CustomWidget, EShooterCustomWidget::Type WidgetType)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Add(MakeShareable(new FShooterMenuItem(CustomWidget, WidgetType)));
		return MenuItem->SubMenu.Last().ToSharedRef();
	}

	FORCEINLINE void ClearSubMenu(TSharedPtr<FShooterMenuItem>& MenuItem)
	{
		EnsureValid(MenuItem);
		MenuItem->SubMenu.Empty();
	}

	template< class UserClass >
	FORCEINLINE void PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, int32 UserIndex, UserClass* inObj, typename FShooterMenuItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
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
