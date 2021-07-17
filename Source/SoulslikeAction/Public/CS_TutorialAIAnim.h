// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_EnemyAnim.h"
#include "CS_TutorialAIAnim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API UCS_TutorialAIAnim : public UCS_EnemyAnim
{
	GENERATED_BODY()
public:

	UCS_TutorialAIAnim();

	// Functions
protected:

	virtual void NativeInitializeAnimation() override;

	virtual void LoadMontageAssets() override;

	virtual void UpdateAnimationProperties() override;
};
