// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_EnemyAnim.h"
#include "CS_Boss1Anim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API UCS_Boss1Anim : public UCS_EnemyAnim
{
	GENERATED_BODY()
public:

	UCS_Boss1Anim();

	// Functions
protected:

	virtual void NativeInitializeAnimation() override;

	virtual void LoadMontageAssets() override;

	virtual void UpdateAnimationProperties() override;

	// Montage
public:

	
};
