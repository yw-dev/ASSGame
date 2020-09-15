// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ShooterAssetManager.generated.h"

class UShooterItem;

/**
 * Game implementation of asset manager, overrides functionality and stores game-specific types
 * It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic
 * This is used by setting AssetManagerClassName in DefaultEngine.ini
 */
UCLASS(config=game)
class SHOOTERGAME_API UShooterAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UShooterAssetManager() {}

	/** Static types for items */ 
	static const FPrimaryAssetType	CategoryItemType;	// 资源类型
	static const FPrimaryAssetType	PotionItemType;		// 药品
	static const FPrimaryAssetType	SkillItemType;		// 技能
	static const FPrimaryAssetType	TokenItemType;		// 货币
	static const FPrimaryAssetType	WeaponItemType;		// 武器
	static const FPrimaryAssetType	PawnItemType;		// 角色
	static const FPrimaryAssetType	ArmorItemType;		// 护具

	/** Returns the current AssetManager object */
	static UShooterAssetManager& Get();

	/**
	 * Synchronously loads an RPGItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UShooterItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};
