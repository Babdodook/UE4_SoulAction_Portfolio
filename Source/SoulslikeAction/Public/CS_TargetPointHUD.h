// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CS_TargetPointHUD.generated.h"

/**
 * 플레이어가 락온한 Target에 TargetPoint 텍스쳐를 표시하는 클래스이다
 */
UCLASS()
class SOULSLIKEACTION_API ACS_TargetPointHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ACS_TargetPointHUD();

public:
	virtual void DrawHUD() override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture* TargetPointTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	float TPTextureSize;
};
