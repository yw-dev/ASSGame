// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Weapons/ShooterRangedWeapon.h"
#include "Weapons/ShooterMeleeWeapon.h"
#include "ShooterPickup_Equips.h"



AShooterPickup_Equips::AShooterPickup_Equips(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void AShooterPickup_Equips::GivePickupTo(class AShooterCharacter* Pawn)
{
	AShooterWeaponBase* Weapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
	if (Weapon)
	{
		//int32 Qty = AmmoClips * Weapon->GetAmmoPerClip();
		//Weapon->GiveAmmo(Qty);

		// Fire event for collected ammo
		if (Pawn)
		{
			const auto Events = Online::GetEventsInterface();
			const auto Identity = Online::GetIdentityInterface();

			if (Events.IsValid() && Identity.IsValid())
			{
				AShooterPlayerController* PC = Cast<AShooterPlayerController>(Pawn->Controller);
				if (PC)
				{
					ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);

					if (LocalPlayer)
					{
						const int32 UserIndex = LocalPlayer->GetControllerId();
						TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
						if (UniqueID.IsValid())
						{
							FVector Location = Pawn->GetActorLocation();
							FRotator Rotation = Pawn->GetActorRotation();
							Pawn->EquipWeapon(Weapon);
							/*
							FOnlineEventParms Params;

							Params.Add(TEXT("SectionId"), FVariantData((int32)0)); // unused
							Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1)); // @todo determine game mode (ffa v tdm)
							Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

							Params.Add(TEXT("ItemId"), FVariantData((int32)Weapon->GetAmmoType() + 1)); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
							Params.Add(TEXT("AcquisitionMethodId"), FVariantData((int32)0)); // unused
							Params.Add(TEXT("LocationX"), FVariantData(Location.X));
							Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
							Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
							Params.Add(TEXT("ItemQty"), FVariantData((int32)Qty));

							Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
							*/
						}
					}
				}
			}
		}
	}
}
