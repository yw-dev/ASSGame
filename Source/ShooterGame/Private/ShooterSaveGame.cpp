// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterSaveGame.h"
#include "ShooterGameInstance.h"


void UShooterSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != EShooterSaveGameVersion::LatestVersion)
	{
		if (SavedDataVersion < EShooterSaveGameVersion::AddedItemData)
		{
			// Convert from list to item data map
			for (const FPrimaryAssetId& ItemId : InventoryItems_DEPRECATED)
			{
				AssetDataSource.Add(ItemId, FShooterItemData(1, 1));
			}

			InventoryItems_DEPRECATED.Empty();
		}

		SavedDataVersion = EShooterSaveGameVersion::LatestVersion;
	}
}


