// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WEEK01_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected : 
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
	void SpawnNewBot();
	void SapwnBotTimerElapsed();
	void StartWave();
	void EndWave();
	void PrepareForNextWave();

};
