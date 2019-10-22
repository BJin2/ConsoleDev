// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tracker.generated.h"

class UNavigationSystemV1;
UCLASS()
class WEEK01_API ATracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tracker Properties")
	UStaticMeshComponent* MeshComp;

	UNavigationSystemV1* navSystem;
	FVector NextPoint;
	FVector GetNextPoint();

	float MoveForce;
	bool bUseVelocityChange;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
