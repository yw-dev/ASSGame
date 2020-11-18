// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Online/ShooterPlayerState.h"
#include "Common/UWidgets/ShooterSkillItemWidget.h"
#include "ShooterPlayerAbility.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHPChanged, float, float, float);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnMPChanged, float, float, float);

/**
 * Game Player Photo Info View Widget(Player：血条和能力面板)
 */
UCLASS()
class UShooterPlayerAbility : public UUserWidget
{
	GENERATED_BODY()


public:

	UShooterPlayerAbility(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* CurrentHealthLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* MaxHealthLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* RestoreHealthLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* CurrentManaLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* MaxManaLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UTextBlock* RestoreManaLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UShooterSkillItemWidget* SkillSlot1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UShooterSkillItemWidget* SkillSlot2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UShooterSkillItemWidget* SkillSlot3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UShooterSkillItemWidget* SkillSlot4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (BindWidget))
	UShooterSkillItemWidget* SkillSlot5;

	FOnHPChanged HPChangedDelegate;

	FOnMPChanged MPChangedDelegate;

public:

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	void InitWidget();

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	void UpdateAbility(const TMap<FShooterItemSlot, UShooterItem*>& SkillItems);

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	void UpdateMPWidget(float InMona, float InMaxMona, float InRestoreMona);

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	void UpdateHPWidget(float InHealth, float InMaxHealth, float InRestoreHealth);

	/** Sets the current Health value of the ProgressBar. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetHealthPercent(float InPercent);

	/** Sets the current Health value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetCurrentHealth(const FText& InText);

	/** Sets the Max Health value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetMaxHealth(const FText& InText);

	/** Sets the Restore Health value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetRestoreHealth(const FText& InText);

	/** Sets the current Mana value of the ProgressBar. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetManaPercent(float InPercent);

	/** Sets the current Mana value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetCurrentMana(const FText& InText);

	/** Sets the Max Mana value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetMaxMana(const FText& InText);

	/** Sets the Restore Mana value of the Progress Text. */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	void SetRestoreMana(const FText& InText);

	/** Gets the Health ProgressBar widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UProgressBar* GetHealthBar() const { return HealthBar; };

	/** Gets the Current Health Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetCurrentHealthLabel() const { return CurrentHealthLabel; };

	/** Gets the Max Health Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetMaxHealthLabel() const { return MaxHealthLabel; };

	/** Gets the Restore Health Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetRestoreHealthLabel() const { return RestoreHealthLabel; };

	/** Gets the Mana ProgressBar widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UProgressBar* GetManaBar() const { return ManaBar; };

	/** Gets the Current Mana Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetCurrentManaLabel() const { return CurrentManaLabel; };

	/** Gets the Max Mana Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetMaxManaLabel() const { return MaxManaLabel; };

	/** Gets the Restore Mana Text widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility | Progress")
	UTextBlock* GetRestoreManaLabel() const { return RestoreManaLabel; };

	/** Gets the Skill Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	UShooterSkillItemWidget* GetSkillSlot1() const { return SkillSlot1; };

	/** Gets the Skill Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	UShooterSkillItemWidget* GetSkillSlot2() const { return SkillSlot2; };
	
	/** Gets the Skill Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	UShooterSkillItemWidget* GetSkillSlot3() const { return SkillSlot3; };

	/** Gets the Skill Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	UShooterSkillItemWidget* GetSkillSlot4() const { return SkillSlot4; };

	/** Gets the Skill Item widget */
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility")
	UShooterSkillItemWidget* GetSkillSlot5() const { return SkillSlot5; };

	void OnHPChanged(float InHealth, float InMaxHealth, float InRestoreHealth);

	void OnMPChanged(float InMana, float InMaxMana, float InRestoreMana);

};
