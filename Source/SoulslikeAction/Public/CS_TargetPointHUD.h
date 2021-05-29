// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CS_TargetPointHUD.generated.h"

/**
 * �÷��̾ ������ Target�� TargetPoint �ؽ��ĸ� ǥ���ϴ� Ŭ�����̴�
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
