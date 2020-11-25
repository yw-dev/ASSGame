// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterBlueprintLibrary.h"



UShooterBlueprintLibrary::UShooterBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UShooterBlueprintLibrary::PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime)
{
	//IActionShooterLoadingScreenModule& LoadingScreenModule = IActionShooterLoadingScreenModule::Get();
	//LoadingScreenModule.StartInGameLoadingScreen(bPlayUntilStopped, PlayTime);
}

void UShooterBlueprintLibrary::StopLoadingScreen()
{
	//IActionShooterLoadingScreenModule& LoadingScreenModule = IActionShooterLoadingScreenModule::Get();
	//LoadingScreenModule.StopInGameLoadingScreen();
}

bool UShooterBlueprintLibrary::IsInEditor()
{
	return GIsEditor;
}

bool UShooterBlueprintLibrary::EqualEqual_ItemSlot(const FShooterItemSlot& A, const FShooterItemSlot& B)
{
	return A == B;
}

bool UShooterBlueprintLibrary::NotEqual_ItemSlot(const FShooterItemSlot& A, const FShooterItemSlot& B)
{
	return A != B;
}

bool UShooterBlueprintLibrary::IsValidItemSlot(const FShooterItemSlot& ItemSlot)
{
	return ItemSlot.IsValid();
}

bool UShooterBlueprintLibrary::DoesEffectContainerSpecHaveEffects(const FShooterGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UShooterBlueprintLibrary::DoesEffectContainerSpecHaveTargets(const FShooterGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FShooterGameplayEffectContainerSpec UShooterBlueprintLibrary::AddTargetsToEffectContainerSpec(const FShooterGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FShooterGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UShooterBlueprintLibrary::ApplyExternalEffectContainerSpec(const FShooterGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

APlayerController* UShooterBlueprintLibrary::GetLocalPlayerController(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (PlayerController->IsLocalController())
			{
				// For this project, we will only ever have one local player.
				return PlayerController;
			}
		}
	}
	return nullptr;
}

bool UShooterBlueprintLibrary::IsValidIP(const FString& Value, const FString Reg)
{
	//const FString IPRegex = "((1\d{2}|25[0-5]|2[0-4]\d|[1-9]?\d)\.){3}(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)";
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, Value);
	regMatcher.SetLimits(0, Value.Len());
	return regMatcher.FindNext();
}

void UShooterBlueprintLibrary::DrawDebugLineTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16.f, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
}

FGameplayTag UShooterBlueprintLibrary::GetGameplayTag(const FString& TagName)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(FName(*TagName));
}