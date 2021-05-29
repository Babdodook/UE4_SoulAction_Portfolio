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

	/**	Vec1���͸� �������� �ش� Vec2���ͱ��� ȸ������ ��ȯ�Ѵ� */
	UFUNCTION(BlueprintCallable, Category="LJSMath")
	static float GetAngleToDirection(FVector Vec1, FVector Vec2);

	/**	DamageCauser�� ���� ���� ȸ������ ��ȯ�Ѵ� */
	UFUNCTION(BlueprintCallable, Category = "LJSMath")
	static float GetAngleToDamageCauser(AActor* OwnActor, AActor* DamageCauser);

	/** ���� Forward���͸� �������� Ÿ�ٰ��� ���̰� ��ȯ */
	UFUNCTION(BlueprintCallable, Category="LJSMath")
	static float GetAngleToTarget(AActor* OwnActor, AActor* Target);

	/** ���� Forward���͸� �������� Ÿ�ٰ��� ���̰��� �������͸� ���� ��ȯ*/
	UFUNCTION(BlueprintCallable, Category = "LJSMath")
	static float GetAngleToTargetCross(AActor* OwnActor, AActor* Target);
};
