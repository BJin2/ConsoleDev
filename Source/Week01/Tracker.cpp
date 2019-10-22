// Fill out your copyright notice in the Description page of Project Settings.


#include "Tracker.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATracker::ATracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	//RootComponent->Mobility = EComponentMobility::Movable;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComp;
	//MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void ATracker::BeginPlay()
{
	Super::BeginPlay();
	navSystem = UNavigationSystemV1::GetCurrent(this);
	NextPoint = GetNextPoint();
}

FVector ATracker::GetNextPoint()
{
	ATPSCharacter* player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (player)
	{
		UNavigationPath* path = navSystem->FindPathToActorSynchronously(this, GetActorLocation(), player);
		if (path->PathPoints.Num() > 1)
		{
			DrawDebugSphere(GetWorld(), path->PathPoints[1], 30, 12, FColor::Yellow, false, 20.f, 0, 3.f);
			return path->PathPoints[1];
		}
	}

	return GetActorLocation();
}

// Called every frame
void ATracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATracker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

