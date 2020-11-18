// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerAbility.h"


#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
/**
 * Game Player Photo Info View Widget(Player：血条和能力面板)
 */
UShooterPlayerAbility::UShooterPlayerAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UShooterPlayerAbility::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::Initialize()"));
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UShooterPlayerAbility::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::NativeConstruct()"));
	Super::NativeConstruct();

	HPChangedDelegate.AddUObject(this, &UShooterPlayerAbility::OnHPChanged);
	MPChangedDelegate.AddUObject(this, &UShooterPlayerAbility::OnMPChanged);

	InitWidget();
}

void UShooterPlayerAbility::SynchronizeProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::SynchronizeProperties()"));
	Super::SynchronizeProperties();
}

void UShooterPlayerAbility::InitWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::InitWidget()"));

	//AShooterPlayerController* PC = Cast<AShooterPlayerController>(GetOwningPlayer());
	AShooterPlayerState* PS = GetOwningPlayer() ? Cast<AShooterPlayerState>(GetOwningPlayer()->PlayerState) : NULL;
	//AShooterCharacter* Pawn = Cast<AShooterCharacter>(PC->GetPawn());
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::InitWidget( Health = %d, MaxHealth = %d)"), PS->GetHealth(), PS->GetMaxHealth());

		// Health
		SetHealthPercent(PS->GetHealth() / PS->GetMaxHealth());
		SetCurrentHealth(FText::FromString(FString::FromInt(PS->GetHealth())));
		SetMaxHealth(FText::FromString(FString::FromInt(PS->GetMaxHealth())));
		SetRestoreHealth(FText::FromString(FString::SanitizeFloat(PS->GetRestoreHealth())));
		// Mana
		SetManaPercent(PS->GetMana() / PS->GetMaxMana());
		SetCurrentMana(FText::FromString(FString::FromInt(PS->GetMana())));
		SetMaxMana(FText::FromString(FString::FromInt(PS->GetMaxMana())));
		SetRestoreMana(FText::FromString(FString::SanitizeFloat(PS->GetRestoreMana())));
		if (SkillSlot1)
		{
			SkillSlot1->SetSlotIndex(1);
		}
		if (SkillSlot2)
		{
			SkillSlot2->SetSlotIndex(2);
		}
		if (SkillSlot3)
		{
			SkillSlot3->SetSlotIndex(3);
		}
		if (SkillSlot4)
		{
			SkillSlot4->SetSlotIndex(4);
		}
		if (SkillSlot5)
		{
			SkillSlot5->SetSlotIndex(5);
		}
	}
}

void UShooterPlayerAbility::UpdateAbility(const TMap<FShooterItemSlot, UShooterItem*>& SkillItems)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerInventory::UpdateInventory(SkillItems.Num = %d)"), SkillItems.Num());
	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : SkillItems)
	{
		if (Pair.Key.ItemType == UShooterAssetManager::SkillItemType)
		{
			if (SkillSlot1 && Pair.Key.SlotNumber == SkillSlot1->SlotIndex - 1)
			{
				SkillSlot1->Update(Pair.Value);
			}
			else if (SkillSlot2 && Pair.Key.SlotNumber == SkillSlot2->SlotIndex - 1)
			{
				SkillSlot2->Update(Pair.Value);
			}
			else if (SkillSlot3 && Pair.Key.SlotNumber == SkillSlot3->SlotIndex - 1)
			{
				SkillSlot3->Update(Pair.Value);
			}
			else if (SkillSlot4 && Pair.Key.SlotNumber == SkillSlot4->SlotIndex - 1)
			{
				SkillSlot4->Update(Pair.Value);
			}
			else if (SkillSlot5 && Pair.Key.SlotNumber == SkillSlot5->SlotIndex - 1)
			{
				SkillSlot5->Update(Pair.Value);
			}
		}
	}

}

void UShooterPlayerAbility::UpdateHPWidget(float InHealth, float InMaxHealth, float InRestoreHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerAbility::UpdateHPWidget()"));
	SetHealthPercent(InHealth / InMaxHealth);
	SetCurrentHealth(FText::FromString(FString::FromInt(InHealth)));
	SetMaxHealth(FText::FromString(FString::FromInt(InMaxHealth)));
	SetRestoreHealth(FText::FromString(FString::FromInt(InRestoreHealth)));

}

void UShooterPlayerAbility::UpdateMPWidget(float InMona, float InMaxMona, float InRestoreMona)
{
	SetManaPercent(InMona / InMaxMona);
	SetCurrentMana(FText::FromString(FString::FromInt(InMona)));
	SetMaxMana(FText::FromString(FString::FromInt(InMaxMona)));
	SetRestoreMana(FText::FromString(FString::FromInt(InRestoreMona)));
}

void UShooterPlayerAbility::OnHPChanged(float InHealth, float InMaxHealth, float InRestoreHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::OnHPChanged()"));

}

void UShooterPlayerAbility::OnMPChanged(float InMana, float InMaxMana, float InRestoreMana)
{
	UE_LOG(LogTemp, Warning, TEXT("UShooterPlayerAbility::OnMPChanged()"));
}

void UShooterPlayerAbility::SetHealthPercent(float InPercent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(InPercent);
	}
}

void UShooterPlayerAbility::SetManaPercent(float InPercent)
{
	if (ManaBar)
	{
		ManaBar->SetPercent(InPercent);
	}
}

void UShooterPlayerAbility::SetCurrentHealth(const FText& InText)
{
	if (CurrentHealthLabel)
	{
		CurrentHealthLabel->SetText(InText);
	}
}

void UShooterPlayerAbility::SetCurrentMana(const FText& InText)
{
	if (CurrentManaLabel)
	{
		CurrentManaLabel->SetText(InText);
	}
}

void UShooterPlayerAbility::SetMaxHealth(const FText& InText)
{
	if (MaxHealthLabel)
	{
		MaxHealthLabel->SetText(InText);
	}
}

void UShooterPlayerAbility::SetMaxMana(const FText& InText)
{
	if (MaxManaLabel)
	{
		MaxManaLabel->SetText(InText);
	}
}

void UShooterPlayerAbility::SetRestoreHealth(const FText& InText)
{
	if (RestoreHealthLabel)
	{
		RestoreHealthLabel->SetText(InText);
	}
}

void UShooterPlayerAbility::SetRestoreMana(const FText& InText)
{
	if (RestoreManaLabel)
	{
		RestoreManaLabel->SetText(InText);
	}
}




/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE



