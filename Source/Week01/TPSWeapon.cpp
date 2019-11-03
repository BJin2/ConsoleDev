// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
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
	curSpread = baseSpread;
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
FVector Normalize(const FVector& a)
{
	float lengthA = FMath::Sqrt(FMath::Square(a.X) + FMath::Square(a.Y) + FMath::Square(a.Z));
	FVector result = a / lengthA;
	return result;
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
		FVector traceEnd = EyeLoc + 10000 * EyeRot.Vector();

		FVector rightVector = Normalize(FVector::CrossProduct(traceEnd, FVector::UpVector));
		FVector upVector = Normalize(FVector::CrossProduct(rightVector, traceEnd));

		float randomX = RandomFloat(-curSpread, curSpread);
		float randomY = RandomFloat(-curSpread, curSpread);
		FVector horizontalOffset = rightVector * randomX;
		FVector verticalOffset = upVector * randomY;
		curSpread *= SpreadIncreasingRate;

		traceEnd += (horizontalOffset + verticalOffset);

		FHitResult HitResult;
		FVector TrailEnd = traceEnd;


		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = true;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLoc, traceEnd, WeaponChannel, QueryParams))
		{
			//DO damage stuff
			AActor* HitActor = HitResult.GetActor();
			TrailEnd = HitResult.ImpactPoint;

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Cast<UPhysicalMaterial>(HitResult.PhysMaterial));
			UParticleSystem* ImpactEffectToPlay = NULL;
			float DamageToApply = BaseDamege;

			switch (SurfaceType)
			{
			case Flesh_Default:
				ImpactEffectToPlay = ImpactEffectBlood;
				break;
			case Flesh_Vulnerable:
				ImpactEffectToPlay = ImpactEffectBlood;
				DamageToApply *= DamageMultiplier;
				break;
			case Concrete:
				ImpactEffectToPlay = ImpactEffectConcrete;
				break;
			default:
				ImpactEffectToPlay = ImpactEffectConcrete;
				break;
			}


			UGameplayStatics::ApplyPointDamage(HitActor, DamageToApply, EyeRot.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);

			if (ImpactEffectToPlay)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectToPlay, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocket);
		}
		
		FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocket);
		if (Gunshot)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, Gunshot, MuzzlePosition);
		}
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
