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
	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_WaveStarter;
	FTimerHandle TimerHandle_Combo;

	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	float timeBetweenWaves;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int waveNumber = 0;
	int numberOfBotsToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int score = 0;
	int scoreMultiplier = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scoring")
	int killCount = 0;

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
	void SpawnNewBot();
	void SapwnBotTimerElapsed();
	void StartWave();
	void EndWave();
	void PrepareForNextWave();
	void CheckWaveState();

public :
	ATPSGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;
	void IncreaseScore(int amt);
	void IncreaseKillCount();
};
