// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ShooterCharacter.h"
#include "AnimNotifyState_WeaponTrace.h"

void UAnimNotifyState_WeaponTrace::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Player = Cast<AShooterCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		CurrentWeapon = Player->GetWeapon();
		WeaponMesh = CurrentWeapon->GetWeaponMesh();
		ActorsToIgnore = { MeshComp->GetOwner() };
		TraceLocation1 = WeaponMesh->GetSocketLocation("TraceSocket1");
		TraceLocation2 = WeaponMesh->GetSocketLocation("TraceSocket2");
		TraceLocation3 = WeaponMesh->GetSocketLocation("TraceSocket3");
	}
}

void UAnimNotifyState_WeaponTrace::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (Player)//检测是否报空//
	{
		//射线检测//  
		const FHitResult Impact = CurrentWeapon->WeaponTraceSingle(TraceLocation1, WeaponMesh->GetSocketLocation("TraceSocket1"));
		//UKismetSystemLibrary::LineTraceMulti(Player->GetWorld(), TraceLocation1, WeaponMesh->GetSocketLocation("TraceSocket1"), ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Red, FLinearColor::Green, 10.f);
		//for (int i = 0; i < HitResults.Num(); i++)
		//{
		if(Impact.GetActor())
		{
			AActor*HitActor = Impact.GetActor();//获取本次射线击中的Actor//
			if (!HitActors.Contains(HitActor))//查询数组中是否有本次击中的Actor，如果没有则添加进数组并调用自带伤害函数，防止一次通知内多次击中的情况//
			{
				HitActors.Add(HitActor);
				//UGameplayStatics::ApplyDamage(HitActor, 10.f, EventInstigator, Player, DamageTypeClass);
				CurrentWeapon->WeaponHit(Impact, TraceLocation1, WeaponMesh->GetSocketLocation("TraceSocket1"), 0.f, 0.f);
			}
		}
		UKismetSystemLibrary::LineTraceMulti(Player->GetWorld(), TraceLocation2, WeaponMesh->GetSocketLocation("TraceSocket2"), ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Red, FLinearColor::Green, 10.f);
		for (int i = 0; i < HitResults.Num(); i++)
		{
			AActor*HitActor = HitResults[i].GetActor();//获取本次射线击中的Actor//
			if (!HitActors.Contains(HitActor))//查询数组中是否有本次击中的Actor，如果没有则添加进数组并调用自带伤害函数，防止一次通知内多次击中的情况//
			{
				HitActors.Add(HitActor);
				//UGameplayStatics::ApplyDamage(HitActor, 10.f, EventInstigator, Player, DamageTypeClass);
				CurrentWeapon->WeaponHit(HitResults[i], TraceLocation2, WeaponMesh->GetSocketLocation("TraceSocket2"), 0.f, 0.f);
			}
		}
		UKismetSystemLibrary::LineTraceMulti(Player->GetWorld(), TraceLocation3, WeaponMesh->GetSocketLocation("TraceSocket3"), ETraceTypeQuery::TraceTypeQuery4, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Red, FLinearColor::Green, 10.f);
		for (int i = 0; i < HitResults.Num(); i++)
		{
			AActor*HitActor = HitResults[i].GetActor();//获取本次射线击中的Actor//
			if (!HitActors.Contains(HitActor))//查询数组中是否有本次击中的Actor，如果没有则添加进数组并调用自带伤害函数，防止一次通知内多次击中的情况//
			{
				HitActors.Add(HitActor);
				//UGameplayStatics::ApplyDamage(HitActor, 10.f, EventInstigator, Player, DamageTypeClass);
				CurrentWeapon->WeaponHit(HitResults[i], TraceLocation3, WeaponMesh->GetSocketLocation("TraceSocket3"), 0.f, 0.f);
			}
		}
		//}
		//一次Tick过后更新当前插槽的位置变量，下一次再与存储的变量做差值//
		TraceLocation1 = WeaponMesh->GetSocketLocation("TraceSocket1");
		TraceLocation2 = WeaponMesh->GetSocketLocation("TraceSocket2");
		TraceLocation3 = WeaponMesh->GetSocketLocation("TraceSocket3");
	}
}

void UAnimNotifyState_WeaponTrace::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	HitActors.Empty();
}

