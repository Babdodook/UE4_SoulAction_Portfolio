// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LJSMathHelpers.generated.h"

// Whether to inline functions at all
#define LJS_MATH_INLINE_ENABLED	(!UE_BUILD_DEBUG)

/**
 * 
 */
UCLASS(meta = (BlueprintThreadSafe, ScriptName = "MathLibrary"))
class SOULSLIKEACTION_API ULJSMathHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**	Vec1벡터를 기준으로 해당 Vec2벡터까지 회전값을 반환한다 */
	UFUNCTION(BlueprintCallable, Category="LJSMath")
	static float GetAngleToDirection(FVector Vec1, FVector Vec2);

	/**	DamageCauser와 액터 사이 회전값을 반환한다 */
	UFUNCTION(BlueprintCallable, Category = "LJSMath")
	static float GetAngleToDamageCauser(AActor* OwnActor, AActor* DamageCauser);

	/** 액터 Forward벡터를 기준으로 타겟과의 사이각 반환 */
	UFUNCTION(BlueprintCallable, Category="LJSMath")
	static float GetAngleToTarget(AActor* OwnActor, AActor* Target);

	/** 액터 Forward벡터를 기준으로 타겟과의 사이각을 법선벡터를 통해 반환*/
	UFUNCTION(BlueprintCallable, Category = "LJSMath")
	static float GetAngleToTargetCross(AActor* OwnActor, AActor* Target);
};
