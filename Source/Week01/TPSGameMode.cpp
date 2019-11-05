// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGameMode.h"
#include "TimerManager.h"

void ATPSGameMode::StartPlay()
{
	Super::StartPlay();
	StartWave();
}

void ATPSGameMode::StartWave()
{
	waveNumber++;
	numberOfBotsToSpawn = 2 * waveNumber;
	//set up timer  to SapwnBotTimerElapsed()
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ATPSGameMode::SapwnBotTimerElapsed, 1.0f, true, 0.0f);
}

void ATPSGameMode::SapwnBotTimerElapsed()
{
	//Spawn the bots using SpawnNewBot()
	SpawnNewBot();
	numberOfBotsToSpawn--;
	//Once completed, call EndWave()
	if (numberOfBotsToSpawn == 0)
		EndWave();	
}

void ATPSGameMode::EndWave()
{
	//Stop the bot spawn timer
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	PrepareForNextWave();
}

void ATPSGameMode::PrepareForNextWave()
{
	//set up timer for StartWave()
	GetWorldTimerManager().SetTimer(TimerHandle_WaveStarter, this, &ATPSGameMode::StartWave, timeBetweenWaves, false);
}