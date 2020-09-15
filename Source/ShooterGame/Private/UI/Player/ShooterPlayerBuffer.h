// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterPlayerBuffer.generated.h"

/**
 * Game Player Photo Info View Widget(Player：Buffer和DeBuffer面板)
 */
UCLASS()
class UShooterPlayerBuffer : public UUserWidget
{
	GENERATED_BODY()

public:

	UShooterPlayerBuffer(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

protected:

	virtual void NativeConstruct() override;

public:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerBuffer", meta = (BindWidget))
	//UImage* Preview;



	
	
	
};
