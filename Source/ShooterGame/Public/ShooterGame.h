// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ParticleDefinitions.h"
#include "SoundDefinitions.h"
#include "Net/UnrealNetwork.h"
#include "ShooterGameMode.h"
#include "ShooterGameState.h"
#include "ShooterCharacter.h"
#include "ShooterCharacterMovement.h"
#include "ShooterPlayerController.h"
#include "ShooterGameClasses.h"


class UBehaviorTreeComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogShooter, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogShooterWeapon, Log, All);

/** when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/
#define COLLISION_TC_WEAPON			ECC_GameTraceChannel1
#define COLLISION_OC_PROJECTILE		ECC_GameTraceChannel2
#define COLLISION_OC_PICKUP			ECC_GameTraceChannel3
#define COLLISION_OC_WEAPON			ECC_GameTraceChannel4
#define COLLISION_OC_SKILL			ECC_GameTraceChannel5
#define COLLISION_TC_MARGIC			ECC_GameTraceChannel6
#define COLLISION_OC_MELEE			ECC_GameTraceChannel6
#define COLLISION_BUFF				ECC_GameTraceChannel7
#define COLLISION_DEBUFF			ECC_GameTraceChannel8

#define MAX_PLAYER_NAME_LENGTH 16

#define PLAYER_ALL_SLOT_COUNT 13
#define PLAYER_WEAPON_SLOT_COUNT 2
#define PLAYER_INVENTORY_SLOT_COUNT 6
#define PLAYER_ABILITY_SLOT_COUNT 5



/** Set to 1 to pretend we're building for console even on a PC, for testing purposes */
#define SHOOTER_SIMULATE_CONSOLE_UI	0

#if PLATFORM_PS4 || PLATFORM_XBOXONE || PLATFORM_SWITCH || SHOOTER_SIMULATE_CONSOLE_UI || PLATFORM_QUAIL
	#define SHOOTER_CONSOLE_UI 1
#else
	#define SHOOTER_CONSOLE_UI 0
#endif
