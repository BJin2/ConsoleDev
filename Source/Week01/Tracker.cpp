// Fill out your copyright notice in the Description page of Project Settings.


#include "Tracker.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "HealthComponent.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ATracker::ATracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	//RootComponent->Mobility = EComponentMobility::Movable;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);
	//MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCanEverAffectNavigation(false);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATracker::OnHealthChanged);

	MoveForce = 1000;
	bUseVelocityChange = true;
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
			//DrawDebugSphere(GetWorld(), path->PathPoints[1], 30, 12, FColor::Yellow, false, 3.f, 0, 3.f);
			return path->PathPoints[1];
		}
	}

	return GetActorLocation();
}

void ATracker::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float DeltaHealth, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Trackerbot Damaged " + FString::SanitizeFloat(Health));

	if (!MatInstance)
	{
		MatInstance = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInstance)
	{
		MatInstance->SetScalarParameterValue("LastTimeHit", UGameplayStatics::GetRealTimeSeconds(this));
	}

	if (Health <= 0)
	{
		SelfDestruct();
	}
}

void ATracker::SelfDestruct()
{
	if (!bDestroyed)
	{
		bDestroyed = true;
		FTransform transform = GetTransform();
		transform.SetScale3D(FVector(3.0f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, transform);
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Blue, false, 1.0f, 0, 1.0f);
		Destroy();
	}
}

// Called every frame
void ATracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float distanceToTarget = (NextPoint - GetActorLocation()).Size();

	if (distanceToTarget < 150)
	{
		//If I am close enough, calculate new next point
		NextPoint = GetNextPoint();
	}
	else
	{
		// Add force to ge to next point
		FVector force = NextPoint - GetActorLocation();
		force.Normalize();
		force *= MoveForce;

		MeshComp->AddForce(force, NAME_None, bUseVelocityChange);

		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + force, 20.f, FColor::Blue, false, 2*DeltaTime, 0, 3.f);
	}
}

// Called to bind functionality to input
void ATracker::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

