// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAI.h"
#include "TPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void ATPSAI::Death(bool head)
{
	ATPSGameMode* mode = Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(this));

	if (!mode)
		return;

	int score = 10;
	if (head)
		score *= 5;

	mode->IncreaseScore(score);
	mode->IncreaseKillCount();
}