// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "RenderUtils.h"
#include "Engine/TextureRenderTarget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ShooterCaptureActor.h"


// Sets default values
AShooterCaptureActor::AShooterCaptureActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(Scene);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());	
	//Mesh->bOnlyOwnerSee = true;
	Mesh->bOwnerNoSee = true;
	//Mesh->bCastDynamicShadow = false;
	//Mesh->bReceivesDecals = false;
	//Mesh1P->SetHiddenInGame(true);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a follow camera
	Capture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture2D"));
	Capture2D->SetupAttachment(Camera); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	RotationSpeed = 10;

}

// Called when the game starts or when spawned
void AShooterCaptureActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShooterCaptureActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

