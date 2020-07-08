// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
#include "ShooterCategoryViewModel.h"


#define LOCTEXT_NAMESPACE "ShooterSourceViewModel"


FShooterCategoryViewModel::FShooterCategoryViewModel()
{
	Category = EShooterSourceCategory::Unknown;
	Initialize();
}

FShooterCategoryViewModel::FShooterCategoryViewModel(EShooterSourceCategory InCategory)
{
	Category = InCategory;
	Initialize();
}

FText FShooterCategoryViewModel::GetText() const
{
	return Text;
}

const FSlateBrush* FShooterCategoryViewModel::GetIconBrush() const
{
	return IconBrush;
}

uint32 FShooterCategoryViewModel::GetTypeHash() const
{
	return (uint32)Category;
}

void FShooterCategoryViewModel::Initialize()
{
	switch (Category)
	{
	case EShooterSourceCategory::Token:
		Text = LOCTEXT("Token", "货币");
		//IconBrush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100));
		SortID = 0;
		break;
	case EShooterSourceCategory::Skill:
		Text = LOCTEXT("Skill", "技能");
		//IconBrush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100));
		SortID = 1;
		break;
	case EShooterSourceCategory::Potion:
		Text = LOCTEXT("Potion", "药品");
		//IconBrush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100));
		SortID = 2;
		break;
	case EShooterSourceCategory::Equipment:
		Text = LOCTEXT("Equipment", "装备");
		//IconBrush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100));
		SortID = 3;
		break;
	default:
		Text = LOCTEXT("Unknown", "其它");
		//IconBrush = FShooterImageBrush(TEXT("/Game/UI/Icons/Races/INV_Misc_Ticket_Tarot_Blessings.INV_Misc_Ticket_Tarot_Blessings"), FVector2D(100, 100));
		SortID = 5;
		break;
	}
}

#undef LOCTEXT_NAMESPACE
