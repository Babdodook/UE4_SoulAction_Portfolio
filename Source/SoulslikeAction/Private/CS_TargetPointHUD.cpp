// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_TargetPointHUD.h"
#include "Kismet/GameplayStatics.h"
#include "CS_Player.h"
#include "CS_Enemy.h"
#include "CS_TargetingSystem.h"
#include "CS_TargetPointComponent.h"

ACS_TargetPointHUD::ACS_TargetPointHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture> TargetPoint_Texture(TEXT("Texture2D'/Game/HUD/HUDTexture/TargetPoint.TargetPoint'"));
	if (TargetPoint_Texture.Succeeded())
	{
		TargetPointTexture = TargetPoint_Texture.Object;
	}

	TPTextureSize = 25.0f;
}

void ACS_TargetPointHUD::DrawHUD()
{
	Super::DrawHUD();

	
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ACS_Player* Player = Cast<ACS_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ACS_Enemy* Target = Cast<ACS_Enemy>(Player->GetTargetSystemComponent()->GetTargetEnemy());
	if (Target != nullptr)
	{
		FVector2D ScreenLocation;
		UGameplayStatics::ProjectWorldToScreen(PlayerController, Target->GetTargetPointComponent()->GetComponentLocation(), ScreenLocation, false);

		ScreenLocation = FVector2D(ScreenLocation.X - TPTextureSize / 2.0f, ScreenLocation.Y - TPTextureSize / 2.0f);

		DrawTexture(TargetPointTexture, ScreenLocation.X, ScreenLocation.Y, TPTextureSize, TPTextureSize, 0.0f, 0.0f, 1.0f, 1.0f);
		
	}
	
}