// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Abilities/ShooterAttributeSet.h"
#include "GameplayEffect.h"
#include "Player/ShooterCharacter.h"
#include "GameplayEffectExtension.h"

// Sets default values
UShooterAttributeSet::UShooterAttributeSet()
	: CharacterLevel(1)
	, Health(1.f)
	, MaxHealth(1.f)
	, RestoreHealth(1.f)
	, Mana(0.f)
	, MaxMana(0.f)
	, RestoreMana(0.f)
	, Coins(0.f)
	, DeathCooldown(1.0f)
	, AttackPower(1.0f)
	, DefensePower(1.0f)
	, MoveSpeed(1.0f)
	, Damage(0.0f)
	, ProvideEXP(1.0f)
	, MaxEXP(0.0f)
{
}

void UShooterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, RestoreHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, RestoreMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, Coins, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, DeathCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, ProvideEXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UShooterAttributeSet, MaxEXP, COND_None, REPNOTIFY_Always);
}

void UShooterAttributeSet::OnRep_CharacterLevel()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, CharacterLevel);
}

void UShooterAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, Health);
}

void UShooterAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, MaxHealth);
}

void UShooterAttributeSet::OnRep_RestoreHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, RestoreHealth);
}

void UShooterAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, Mana);
}

void UShooterAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, MaxMana);
}

void UShooterAttributeSet::OnRep_RestoreMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, RestoreMana);
}

void UShooterAttributeSet::OnRep_Coins()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, Coins);
}

void UShooterAttributeSet::OnRep_DeathCooldown()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, DeathCooldown);
}

void UShooterAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, AttackPower);
}

void UShooterAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, DefensePower);
}

void UShooterAttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, MoveSpeed);
}

void UShooterAttributeSet::OnRep_ProvideEXP()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, ProvideEXP);
}

void UShooterAttributeSet::OnRep_MaxEXP()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UShooterAttributeSet, MaxEXP);
}

void UShooterAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet::AdjustAttributeForMaxChange()"));
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UShooterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet::PreAttributeChange()"));
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
}

void UShooterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("AttributeSet::PostGameplayEffectExecute()"));
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AShooterCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AShooterCharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Get the Source actor
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		AShooterCharacterBase* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			// Use the controller to find the source pawn
			if (SourceController)
			{
				SourceCharacter = Cast<AShooterCharacterBase>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<AShooterCharacterBase>(SourceActor);
			}

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);
		UE_LOG(LogTemp, Warning, TEXT("AttributeSet::PostGameplayEffectExecute( Damage = %d)"), GetDamage());

		if (LocalDamageDone > 0)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
			}

			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

			if (TargetCharacter && WasAlive)
			{
				// Play HitReact animation and sound with a multicast RPC.
				const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();

				/*
				FPointDamageEvent PointDmg;
				PointDmg.DamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				PointDmg.HitInfo = HitResult;
				PointDmg.Damage = LocalDamageDone;
				*/
				HitResult.GetActor()->TakeDamage(LocalDamageDone, FDamageEvent(UDamageType::StaticClass()), SourceController, SourceActor);
				//TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
				//FPointDamageEvent damageEvent;
				//damageEvent.HitInfo = HitResult;
				//TargetCharacter->TakeDamage(LocalDamageDone, damageEvent, SourceController, SourceActor);
				// This is proper damage
				//TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);
				//TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter->GetController(), SourceActor);

				// Call for all health changes
				TargetCharacter->HandleHealthChanged(-LocalDamageDone);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (TargetCharacter)
		{
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(DeltaValue);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Clamp mana
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));

		if (TargetCharacter)
		{
			// Call for all mana changes
			TargetCharacter->HandleManaChanged(DeltaValue);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		if (TargetCharacter)
		{
			// Call for all movespeed changes
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue);
		}
	}
}
