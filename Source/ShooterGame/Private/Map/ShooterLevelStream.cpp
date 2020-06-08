// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterLevelStream.h"


// Sets default values
AShooterLevelStream::AShooterLevelStream()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AShooterLevelStream::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &AShooterLevelStream::OverlapEnds);

}

// Called when the game starts or when spawned
void AShooterLevelStream::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterLevelStream::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterLevelStream::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter && MapToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, MapToLoad, true, true, LatentInfo);
	}
}

void AShooterLevelStream::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter && MapToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::UnloadStreamLevel(this, MapToLoad, LatentInfo, false);
	}
}

