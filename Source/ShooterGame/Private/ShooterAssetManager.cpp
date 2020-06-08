// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Items/ShooterItem.h"
#include "ShooterAssetManager.h"

const FPrimaryAssetType	UShooterAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	UShooterAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UShooterAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UShooterAssetManager::WeaponItemType = TEXT("Weapon");
const FPrimaryAssetType	UShooterAssetManager::PawnItemType = TEXT("Pawn");


UShooterAssetManager& UShooterAssetManager::Get()
{
	UShooterAssetManager* This = Cast<UShooterAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be UShooterAssetManager!"));
		return *NewObject<UShooterAssetManager>(); // never calls this
	}
}

UShooterItem* UShooterAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UShooterItem* LoadedItem = Cast<UShooterItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}