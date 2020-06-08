// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "ShooterTypes.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Weapons/ShooterRangedWeapon.h"
#include "Weapons/ShooterMeleeWeapon.h"
#include "Pickups/ShooterPickup_Ammo.h"

AShooterPickup_Ammo::AShooterPickup_Ammo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AmmoClips = 2;
}

bool AShooterPickup_Ammo::IsForWeapon(UClass* WeaponClass)
{
	return WeaponType->IsChildOf(WeaponClass);
}

bool AShooterPickup_Ammo::CanBePickedUp(AShooterCharacter* TestPawn) const
{
	AShooterWeaponBase* TestWeapon = (TestPawn ? TestPawn->FindWeapon(WeaponType) : NULL);
	if (TestWeapon->GetCurrentWeaponType() != EShooterWeaponType::Gun)
	{
		return false;
	}
	if (bIsActive)
	{
		return ((AShooterRangedWeapon*)TestWeapon)->GetCurrentAmmo() < ((AShooterRangedWeapon*)TestWeapon)->GetMaxAmmo();
	}

	return false;
}

void AShooterPickup_Ammo::GivePickupTo(class AShooterCharacter* Pawn)
{
	AShooterWeaponBase* Weapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
	if (Weapon && Weapon->GetCurrentWeaponType() == EShooterWeaponType::Gun)
	{
		int32 Qty = AmmoClips * ((AShooterRangedWeapon*)Weapon)->GetAmmoPerClip();
		((AShooterRangedWeapon*)Weapon)->GiveAmmo(Qty);

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

							FOnlineEventParms Params;		

							Params.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
							Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused

							Params.Add( TEXT( "ItemId" ), FVariantData( (int32)((AShooterRangedWeapon*)Weapon)->GetAmmoType() + 1 ) ); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
							Params.Add( TEXT( "AcquisitionMethodId" ), FVariantData( (int32)0 ) ); // unused
							Params.Add( TEXT( "LocationX" ), FVariantData( Location.X ) );
							Params.Add( TEXT( "LocationY" ), FVariantData( Location.Y ) );
							Params.Add( TEXT( "LocationZ" ), FVariantData( Location.Z ) );
							Params.Add( TEXT( "ItemQty" ), FVariantData( (int32)Qty ) );		

							Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
						}
					}
				}
			}
		}
	}
}
