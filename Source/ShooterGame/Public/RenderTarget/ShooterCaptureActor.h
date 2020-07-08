// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterCaptureActor.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterCaptureActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
//public:	
	// Sets default values for this actor's properties
	//AShooterCaptureActor(const FObjectInitializer& ObjectInitializer)

private:

	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Scene, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Scene;

	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture2D, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* Capture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RenderTarget, meta = (AllowPrivateAccess = "true"))
	class UTextureRenderTarget2D* RenderTarget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float RotationSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
