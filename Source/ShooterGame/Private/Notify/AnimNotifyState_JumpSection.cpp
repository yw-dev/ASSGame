// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Player/ShooterCharacter.h"
#include "AnimNotifyState_JumpSection.h"


UAnimNotifyState_JumpSection::UAnimNotifyState_JumpSection() 
{
	bInstantJump = false;
}

void UAnimNotifyState_JumpSection::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	/*
	if (MeshComp->GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}*/

	AShooterCharacter* Pawn = Cast<AShooterCharacter>(MeshComp->GetOwner());
	//UE_LOG(LogTemp, Warning, TEXT("JumpSection::NotifyBegin(EnableComboPeriod = %s)"), Pawn->GetEnableComboPeriod() == true ? TEXT("True") : TEXT("False"));
	if (Pawn)
	{
		Pawn->SetEnableComboPeriod(true);
		Pawn->SetJumpSectionNS(this);
	}
	Received_NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UAnimNotifyState_JumpSection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	/*
	if (MeshComp->GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}*/

	UE_LOG(LogTemp, Warning, TEXT("JumpSection::NotifyTick()"));
	AShooterCharacter* Pawn = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (Pawn)
	{
		Pawn->JumpSectionForCombo();
	}
	Received_NotifyTick(MeshComp, Animation, FrameDeltaTime);
}

void UAnimNotifyState_JumpSection::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	/*
	if (MeshComp->GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}*/

	UE_LOG(LogTemp, Warning, TEXT("JumpSection::NotifyEnd()"));
	AShooterCharacter* Pawn = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (Pawn)
	{
		Pawn->SetEnableComboPeriod(false);
	}
	Received_NotifyEnd(MeshComp, Animation);
}


TArray<FName> UAnimNotifyState_JumpSection::GetJumpSetctions()
{
	return JumpSections;
}


bool UAnimNotifyState_JumpSection::IsInstantJump()
{
	return bInstantJump;
}