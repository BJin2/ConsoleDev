// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProjectileWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TPSProjectile.h"

void ATPSProjectileWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLoc;
		FRotator EyeRot;
		MyOwner->GetActorEyesViewPoint(EyeLoc, EyeRot);

		FVector traceEnd = EyeLoc + 10000 * EyeRot.Vector();
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLoc, traceEnd, ECC_Visibility))
		{
			//DO damage stuff
			AActor* HitActor = HitResult.GetActor();
			//UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, EyeRot.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);
		}

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocket);
		}

		if (ProjectileObject)
		{
			FVector socketLoc;
			FRotator socketRot;
			MeshComp->GetSocketWorldLocationAndRotation(TEXT("MuzzleFlashSocket"), socketLoc, socketRot);
			GetWorld()->SpawnActor<ATPSProjectile>(ProjectileObject, socketLoc, EyeRot);
		}

		DrawDebugLine(GetWorld(), EyeLoc, traceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

void ATPSProjectileWeapon::StartFire()
{
	Fire();
}

void ATPSProjectileWeapon::EndFire()
{
	//Does nothing
}
