// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterTypes.h"
#include "Engine/DataTable.h"
#include "ShooterArmorEntry.generated.h"

/*
*	Armor's Data.(护具类Entry)
*/
USTRUCT(BlueprintType)
struct FShooterArmorEntry : public FTableRowBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	FShooterArmorEntry()
	{

	}

	
};
