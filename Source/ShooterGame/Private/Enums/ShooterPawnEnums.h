// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterPawnEnums.generated.h"

#pragma once

UENUM(BlueprintType)
enum class EShooterRaceType : uint8
{
	Unknown			UMETA(DisplayName = "未知"),
	EAncientGod		UMETA(DisplayName = "神族"),
	EHuman			UMETA(DisplayName = "人族"),
	EGiants			UMETA(DisplayName = "巨人族"),
	EDragon			UMETA(DisplayName = "龙族"),
	Evilkind		UMETA(DisplayName = "妖族"),
	EDemon			UMETA(DisplayName = "魔族"),
	Elf				UMETA(DisplayName = "精灵族"),
};

UENUM(BlueprintType)
enum class EShooterCampType : uint8
{
	ENeutral		UMETA(DisplayName = "中立"),
	EHostile		UMETA(DisplayName = "敌对"),
	EAllies			UMETA(DisplayName = "盟友"),
	EOwners			UMETA(DisplayName = "己方"),
};
