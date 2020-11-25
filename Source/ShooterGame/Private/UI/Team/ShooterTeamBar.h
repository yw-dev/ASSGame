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


	/** Sets the GameTime Text. */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | Time")
	void SetGameTime(FText InTime);

	/** Gets the GameTime Text */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | Time")
	UTextBlock* GetGameTime() const { return TimeLabel; };

	/** Sets the OwnerScore Text. */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | OwnerScore")
	void SetOwnerScore(FText InScore);

	/** Gets the OwnerScore Text */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | OwnerScore")
	UTextBlock* GetOwnerScore() const { return OwnerScoreLabel; };

	/** Sets the TargetScore Text. */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | TargetScore")
	void SetTargetScore(FText InScore);

	/** Gets the TargetScore Text */
	UFUNCTION(BlueprintCallable, Category = "TeamBar | TargetScore")
	UTextBlock* GetTargetScore() const { return TargetScoreLabel; };

	UFUNCTION(BlueprintCallable, Category = "TeamBar | Update")
	void Update(FString InTime, float InOwnerScore, float InTargetScore);

	
};
