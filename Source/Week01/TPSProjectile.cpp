// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProjectile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/DamageType.h"
#include "Engine/Engine.h"

// Sets default values
ATPSProjectile::ATPSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = (USceneComponent*)MeshComp;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Comp"));
}

// Called when the game starts or when spawned
void ATPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSProjectile::Explode()
{
	if (ExplosionEffect)
	{
		FTransform transform = GetTransform();
		transform.SetScale3D(FVector(3.0f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, transform, true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), 200, 32, FColor::Red, false, 1.0f, 0, 1.0f);
		if (UGameplayStatics::ApplyRadialDamage(GetWorld(), 300, GetActorLocation(), 200, UDamageType::StaticClass(), TArray<AActor*>(), nullptr, nullptr, true))
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), 200, 32, FColor::Blue, false, 1.0f, 0, 1.0f);
		}
		else
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), 200, 32, FColor::Red, false, 1.0f, 0, 1.0f);
		}


		/*/
		FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, this);

		SphereParams.AddIgnoredActors(TArray<AActor*>());
		TArray<FOverlapResult> Overlaps;
		if (UWorld * World = GEngine->GetWorldFromContextObject(GetWorld(), EGetWorldErrorMode::LogAndReturnNull))
		{
			World->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(200), SphereParams);
		}

		int num = Overlaps.Num();
		UE_LOG(LogTemp, Warning, TEXT("%d"), num);
		for (int i = 0; i < num; i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Overlaps[i].GetActor()->GetFName().ToString());
		}

		// collate into per-actor list of hit components
		TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
		for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
		{
			FOverlapResult const& Overlap = Overlaps[Idx];
			AActor* const OverlapActor = Overlap.GetActor();

			if (OverlapActor &&
				OverlapActor->bCanBeDamaged &&
				OverlapActor != this &&
				Overlap.Component.IsValid())
			{
				FHitResult Hit;
				if (ECC_MAX == ECC_MAX)
				{
					TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
					HitList.Add(Hit);
				}
			}
		}

		int num2 = OverlapComponentMap.Num();
		UE_LOG(LogTemp, Warning, TEXT("Overlap Comp : %d"), num2);

		//*/
		
		Destroy();
	}
}