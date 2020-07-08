// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FSlateBrush;
enum class EShooterSourceCategory : uint8;


/** A view model for displaying a content source category in the UI. */
class FShooterCategoryViewModel
{
public:
	FShooterCategoryViewModel();

	FShooterCategoryViewModel(EShooterSourceCategory InCategory);

	/** Gets the display name of the category. */
	FText GetText() const;

	/** Gets the brush which should be used to draw the icon for the category. */
	const FSlateBrush* GetIconBrush() const;

	inline bool operator==(const FShooterCategoryViewModel& Other) const
	{
		return Other.Text.EqualTo(Text) && (Other.IconBrush == IconBrush);
	}

	bool operator<(FShooterCategoryViewModel const& Other) const
	{
		return SortID < Other.SortID;
	}

	uint32 GetTypeHash() const;

private:
	void Initialize();

private:
	EShooterSourceCategory Category;
	FText Text;
	const FSlateBrush* IconBrush;
	int SortID;
};

inline uint32 GetTypeHash(const FShooterCategoryViewModel& CategoryViewModel)
{
	return CategoryViewModel.GetTypeHash();
}

