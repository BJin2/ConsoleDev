// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Week01.h"

int32 DebugDrawWeapons = 0;
FAutoConsoleVariableRef CVARDrawWeapons = FAutoConsoleVariableRef(TEXT("TPS.DebugDrawWeapons"), DebugDrawWeapons, TEXT("Draw debug weapon line trace"), ECVF_Cheat);

// Sets default values
ATPSWeapon::ATPSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = (USceneComponent*)MeshComp;
	curMode = FIRE_MODE::AUTO;
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}
float RandomFloat(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float range = max - min;
	return (random * range) + min;
}
void ATPSWeapon::Fire()
{
	if (Ammo <= 0)
		return;

	Ammo--;
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		FVector EyeLoc;
		FRotator EyeRot;
		MyOwner->GetActorEyesViewPoint(EyeLoc, EyeRot);
		float randomX = RandomFloat(-curSpread, curSpread);
		float randomY = RandomFloat(-curSpread, curSpread);
		FVector traceEnd = EyeLoc + 10000 * EyeRot.Vector();

		FVector rightVector = FVector::CrossProduct(traceEnd, FVector::UpVector).GetSafeNormal();
		FVector upVector = FVector::CrossProduct(rightVector, traceEnd).GetSafeNormal();
		FVector horizontalOffset = rightVector * randomX;
		FVector verticalOffset = upVector * randomY;
		traceEnd += (horizontalOffset + verticalOffset);
		curSpread *= SpreadIncreasingRate;

		FHitResult HitResult;
		FVector TrailEnd = traceEnd;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLoc, traceEnd, ECC_Visibility))
		{
			//DO damage stuff
			AActor* HitActor = HitResult.GetActor();
			TrailEnd = HitResult.ImpactPoint;

			float DamageToApply = BaseDamege;




			UGameplayStatics::ApplyPointDamage(HitActor, DamageToApply, EyeRot.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocket);
		}
		
		FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocket);

		if (TrailEffect)
		{
			UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, MuzzlePosition);

			if (TrailComp)
			{
				TrailComp->SetVectorParameter(TrailEffectParameter, TrailEnd);
			}
		}
		// Camera Shake
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
		if (DebugDrawWeapons > 0)
		{
			DrawDebugLine(GetWorld(), EyeLoc, traceEnd, FColor::Red, false, 0.5f, 0, 1.0f);
		}
	}
}

void ATPSWeapon::StartFire()
{
	if (curMode == FIRE_MODE::SINGLE)
	{
		Fire();
	}
	else if(curMode == FIRE_MODE::AUTO)
	{
		curSpread = baseSpread;
		GetWorldTimerManager().SetTimer(BulletTimer, this, &ATPSWeapon::Fire, FireRate, true, 0.0f);
	}
}

void ATPSWeapon::EndFire()
{
	curSpread = baseSpread;
	GetWorldTimerManager().ClearTimer(BulletTimer);
}

void ATPSWeapon::ChangeFireMode()
{
	if (curMode == FIRE_MODE::SINGLE)
		curMode = FIRE_MODE::AUTO;
	else if (curMode == FIRE_MODE::AUTO)
		curMode = FIRE_MODE::SINGLE;
}
