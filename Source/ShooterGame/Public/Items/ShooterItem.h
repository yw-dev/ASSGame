// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Engine/DataAsset.h"
#include "ShooterItem.generated.h"

class UShooterGameplayAbility;

/**
 *  Base class for all items, do not blueprint directly
 */
UCLASS(BlueprintType)
class SHOOTERGAME_API UShooterItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Constructor */
	UShooterItem()
		: Price(0)
		, MaxCount(1)
		, MaxLevel(1)
	{}

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/** @return Returns the parent or NULL if this is a root */
	const TWeakObjectPtr<class UShooterItem> GetParentCategory() const
	{
		if (HasParent)
		{
			return ParentElement;
		}
		return nullptr;
	}

	/** @return the path on hard disk, read-only */
	const FString GetDirectoryPath() const
	{
		return DirectoryPath;
	}

	/** @return Returns all subdirectories, read-only */
	const TArray<UShooterItem*> GetSubDirectories() const
	{
		return SubDirectories;
	}

	/** @return Returns all subdirectories, read or write */
	TArray<UShooterItem*> AccessSubDirectories()
	{
		return SubDirectories;
	}

	/** Add a subdirectory to this node in the tree! */
	void AddSubDirectory(UShooterItem* NewSubDir)
	{
		SubDirectories.Add(NewSubDir);
	}

	/** Parent item or NULL if this is a root  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TWeakObjectPtr<class UShooterItem> ParentElement;

	/** Child categories */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TArray<UShooterItem*> SubDirectories;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	uint8 HasParent : 1;

	/** Full path of this directory in the tree */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FString DirectoryPath;

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FName IconPath;

	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item, DisplayName = "Preview")
	FSlateBrush Preview;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	uint8 bSelected:1;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UShooterGameplayAbility> GrantedAbility;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

};
