// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "TPSWeapon.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HealthComponent.h"
#include "Week01.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATPSCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	EndZoom();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeaponClass, spawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		CurrentWeapon->SetOwner(this);
	}
}

void ATPSCharacter::OnHealthChanged(UHealthComponent * OwningHealthComp, float Health, float DeltaHealth, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Health <= 0 && !bDead)
	{
		bDead = true;
		StopWeapon();
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(5);
		GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, deathMaterial);
		GetMesh()->SetScalarParameterValueOnMaterials("StartTime", UGameplayStatics::GetRealTimeSeconds(this));
		Death(headshot);
	}
	else
	{
		if(DeltaHealth != 0)
			StartRestore();
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	dt = DeltaTime;
	if (bInCover)
	{
		if (bIsAiming)
		{
			GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		}
		else
		{
			if (OverlappingCoverVolume)
			{
				GetMesh()->SetWorldRotation(OverlappingCoverVolume->GetComponentRotation().Add(0, 40, 0));
			}
		}
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	}
}

void ATPSCharacter::MoveForward(float val)
{
	if (!bInCover)
	{
		GetCharacterMovement()->AddInputVector(GetActorForwardVector() * val);
	}
}

void ATPSCharacter::MoveSide(float val)
{
	if (bInCover && OverlappingCoverVolume)
	{
		FVector targetPos = GetActorLocation() + (OverlappingCoverVolume->GetRightVector() * val * dt);
		FVector offsetFromCoverCenter = targetPos - (OverlappingCoverVolume->GetComponentLocation());
		FVector overlappingWorldPos = OverlappingCoverVolume->GetComponentLocation();
		float offset_coverRight_dot = FVector::DotProduct(offsetFromCoverCenter, OverlappingCoverVolume->GetRightVector());
		bool isTargetInRange = abs(offset_coverRight_dot) < (OverlappingCoverVolume->GetScaledBoxExtent().Y - 50);
		bool isMovingTowardCenter = offset_coverRight_dot * val < 0;
		
		FVector traceStart = overlappingWorldPos;
		FVector traceEnd = OverlappingCoverVolume->GetRightVector() * offset_coverRight_dot + overlappingWorldPos;
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		if (isTargetInRange || isMovingTowardCenter)
		{
			GetCharacterMovement()->AddInputVector(OverlappingCoverVolume->GetRightVector() * val);
		}
	}
	else
	{
		GetCharacterMovement()->AddInputVector(GetActorRightVector() * val);
	}
}

void ATPSCharacter::BeginCrouch()
{
	Crouch();
}

void ATPSCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPSCharacter::Death(bool head)
{
}

void ATPSCharacter::StartRestore()
{
}

void ATPSCharacter::StartZoom()
{
	bIsAiming = true;
	//CameraComp->SetFieldOfView(zoomFOV);
}

void ATPSCharacter::EndZoom()
{
	bIsAiming = false;
	//CameraComp->SetFieldOfView(defaultFOV);
}

void ATPSCharacter::FireWeapon()
{
	if (CurrentWeapon)
	{
		if (bInCover)
		{
			if (bIsAiming)
			{
				CurrentWeapon->StartFire();
			}
		}
		else
		{
			CurrentWeapon->StartFire();
		}
		
	}
}

void ATPSCharacter::StopWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}

void ATPSCharacter::ChangeWeaponMode()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->ChangeFireMode();
	}
}

void ATPSCharacter::TakeCover()
{
	if (OverlappingCoverVolume)
	{
		if (bInCover)
		{
			bInCover = false;
		}
		else
		{
			FVector traceStart = GetActorLocation();
			FVector traceEnd = OverlappingCoverVolume->GetForwardVector() * 10000 + traceStart;
			FHitResult HitResult;

			FVector TrailEnd = traceEnd;

			if (GetWorld()->LineTraceSingleByChannel(HitResult, traceStart, traceEnd, CoverChannel))
			{
				FVector targetLocation = HitResult.Location;
				targetLocation -= OverlappingCoverVolume->GetForwardVector() * (GetCapsuleComponent()->GetScaledCapsuleRadius());
				DrawDebugSphere(GetWorld(), targetLocation, 10, 24, FColor::Yellow, false, 5, 0, 2);
				SetActorLocation(targetLocation);

				bInCover = true;
			}
		}
	}
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	return Super::GetPawnViewLocation();
}

void ATPSCharacter::SetHeadshot(bool h)
{
	headshot = h;
}

