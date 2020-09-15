// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Blueprint/UserWidget.h"
#include "ShooterPlayerPhoto.generated.h"

/**
 * Game Player Photo Info View Widget(Player：头像面板)
 */
UCLASS()
class UShooterPlayerPhoto : public UUserWidget
{
	GENERATED_BODY()


public:

	UShooterPlayerPhoto(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPhoto", meta = (BindWidget))
	UImage* Preview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPhoto", meta = (BindWidget))
	UBorder* DieCooldownPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPhoto", meta = (BindWidget))
	UTextBlock* DieCooldownLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPhoto", meta = (BindWidget))
	UTextBlock* LevelLabel;

	
	
};
