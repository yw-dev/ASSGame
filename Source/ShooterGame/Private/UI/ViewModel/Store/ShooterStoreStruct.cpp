// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterTypes.h"
#include "ShooterCharacter.h"
#include "ShooterStoreStruct.h"



FShooterStoreStruct::FShooterStoreStruct()
	: ActualDamage(0)
	, DamageTypeClass(NULL)
	, PawnInstigator(NULL)
	, DamageCauser(NULL)
	, DamageEventClassID(0)
	, bKilled(false)
	, EnsureReplicationByte(0)
{}

FDamageEvent& FShooterStoreStruct::GetDamageEvent()
{
	switch (DamageEventClassID)
	{
	case FPointDamageEvent::ClassID:
		if (PointDamageEvent.DamageTypeClass == NULL)
		{
			PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return PointDamageEvent;

	case FRadialDamageEvent::ClassID:
		if (RadialDamageEvent.DamageTypeClass == NULL)
		{
			RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return RadialDamageEvent;

	default:
		if (GeneralDamageEvent.DamageTypeClass == NULL)
		{
			GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
		}
		return GeneralDamageEvent;
	}
}

void FShooterStoreStruct::SetDamageEvent(const FDamageEvent& DamageEvent)
{
	DamageEventClassID = DamageEvent.GetTypeID();
	switch (DamageEventClassID)
	{
	case FPointDamageEvent::ClassID:
		PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
		break;
	case FRadialDamageEvent::ClassID:
		RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
		break;
	default:
		GeneralDamageEvent = DamageEvent;
	}

	DamageTypeClass = DamageEvent.DamageTypeClass;
}

void FShooterStoreStruct::EnsureReplication()
{
	EnsureReplicationByte++;
}

