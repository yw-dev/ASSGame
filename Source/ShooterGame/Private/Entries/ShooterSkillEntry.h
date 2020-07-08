// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShooterSkillEntry.generated.h"

/*
*	Skill's Data.(技能类Entry)
*/
USTRUCT(BlueprintType)
struct FShooterSkillEntry : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties	
	FShooterSkillEntry()
	{
	}


	
};
