// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API ACS_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:

	/** UMG ���� / ���� ü�¹� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;
	
	/** ���� ����� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

protected:
	virtual void BeginPlay() override;

};
