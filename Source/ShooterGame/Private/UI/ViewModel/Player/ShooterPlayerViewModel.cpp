// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterPlayerViewModel.h"


// Sets default values
AShooterPlayerViewModel::AShooterPlayerViewModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterPlayerViewModel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterPlayerViewModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

