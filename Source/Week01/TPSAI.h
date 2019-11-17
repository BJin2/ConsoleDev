// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSCharacter.h"
#include "TPSAI.generated.h"

/**
 * 
 */
UCLASS()
class WEEK01_API ATPSAI : public ATPSCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void Death(bool head) override;
};
