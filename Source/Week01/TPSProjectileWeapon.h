// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSProjectileWeapon.generated.h"

/**
 * 
 */
class ATPSProjectile;

UCLASS()
class WEEK01_API ATPSProjectileWeapon : public ATPSWeapon
{
	GENERATED_BODY()
	
protected:
	void Fire() override;
	void StartFire() override;
	void EndFire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ATPSProjectile> ProjectileObject;
};
