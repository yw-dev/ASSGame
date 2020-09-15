// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterTeamBar.generated.h"

/**
 * Game Player Team Header Bar Widget(Player：HUD Header 团队)
 */
UCLASS()
class UShooterTeamBar : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UShooterTeamBar(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamBar", meta = (BindWidget))
	UTextBlock* TimeLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamBar", meta = (BindWidget))
	UTextBlock* OwnerScoreLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TeamBar", meta = (BindWidget))
	UTextBlock* TargetScoreLabel;


	
	
};
