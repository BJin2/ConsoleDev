// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "TPSGameMode.h"
#include "Engine/Engine.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ATPSPlayer::ATPSPlayer()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComp);
}


void ATPSPlayer::Death(bool head)
{
	ATPSGameMode* mode = Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!mode)
		return;
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Death");
	mode->GameOver();
}

void ATPSPlayer::StartZoom()
{
	Super::StartZoom();
	CameraComp->SetFieldOfView(zoomFOV);
}

void ATPSPlayer::EndZoom()
{
	Super::EndZoom();
	CameraComp->SetFieldOfView(defaultFOV);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveSide", this, &ATPSPlayer::MoveSide);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSide", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSPlayer::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSPlayer::EndCrouch);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSPlayer::StartZoom);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSPlayer::EndZoom);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSPlayer::FireWeapon);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSPlayer::StopWeapon);
	PlayerInputComponent->BindAction("FireMode", IE_Pressed, this, &ATPSPlayer::ChangeWeaponMode);
	PlayerInputComponent->BindAction("TakeCover", IE_Pressed, this, &ATPSPlayer::TakeCover);
}

FVector ATPSPlayer::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}