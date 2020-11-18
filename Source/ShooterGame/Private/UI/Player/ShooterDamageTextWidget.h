// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ShooterDamageTextWidget.generated.h"

/**
 * For the floating Damage Numbers when a Character receives damage.
 */
UCLASS()
class UShooterDamageTextWidget : public UWidgetComponent
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage);
	
	
};
