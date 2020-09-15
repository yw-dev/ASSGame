// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterTargetPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UShooterTargetPlayer : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UShooterTargetPlayer(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTarget", meta = (BindWidget))
	UImage* Preview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTarget", meta = (BindWidget))
	UTextBlock* LevelLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTarget", meta = (BindWidget))
	UImage* ImageIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTarget", meta = (BindWidget))
	UTextBlock* PlayerName;

	
	
};
