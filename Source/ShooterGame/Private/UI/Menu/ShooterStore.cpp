// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterStyle.h"
#include "ShooterStore.h"

#define LOCTEXT_NAMESPACE "ShooterGame.HUD.Menu"


// Sets default values
void FShooterStore::Construct(ULocalPlayer* _PlayerOwner)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void FShooterStore::ToggleStoreMenu()
{

	
}

// Called every frame
bool FShooterStore::IsActive() const
{

	return true;
}

