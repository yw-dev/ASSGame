// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterPlayerStatusBar.generated.h"

/**
 * 
 */
UCLASS()
class UShooterPlayerStatusBar : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaPercentage(float ManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCharacterName(const FText& NewName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateStatusBar(float HealthPercentage, float ManaPercentage);

};
