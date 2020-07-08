// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterSource.h"


// Sets default values
AShooterSource::AShooterSource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterSource::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

