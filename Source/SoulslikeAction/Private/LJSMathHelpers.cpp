// Fill out your copyright notice in the Description page of Project Settings.


#include "LJSMathHelpers.h"

// Our own easily changed inline options
#if LJS_MATH_INLINE_ENABLED
#define LJS_MATH_FORCEINLINE	FORCEINLINE_DEBUGGABLE
#define LJS_MATH_INLINE			inline
#else
#define LJS_MATH_FORCEINLINE	// nothing
#define LJS_MATH_INLINE			// nothing
#endif

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToDirection(FVector Vec1, FVector Vec2)
{
	// 두 벡터의 내적, Vec1과 Vec2의 사이각을 구한다.
	float dot = FVector::DotProduct(Vec1, Vec2);

	// Radian을 Degree로 변환
	float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// 두 벡터의 외적, Vec1과 Vec2의 법선벡터를 구한다.
	FVector cross = FVector::CrossProduct(Vec1, Vec2);

	float TurnAngle = 0;

	// 액터 기준으로 왼쪽 또는 오른쪽으로 회전할지 판별
	if (cross.Z > 0)
	{
		TurnAngle = angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360에서 뺴게되면 양수로 각을 리턴하게 된다.
		TurnAngle = -angle;
	}

	return TurnAngle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToDamageCauser(AActor* OwnActor, AActor* DamageCauser)
{
	// 타겟과 현재 액터의 방향 벡터를 구함
	FVector direction = (DamageCauser->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// 현재 액터의 forward벡터와 direction 벡터의 내적, 라디안각도 구함
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// 호도 -> 각도 변환
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// 두 벡터의 외적, 액터 Forward벡터와 구르기 방향의 법선벡터를 구한다.
	FVector cross = FVector::CrossProduct(FVector::ForwardVector, direction);

	float TurnAngle = 0;

	// 액터 기준으로 왼쪽 또는 오른쪽 판단
	if (cross.Z > 0)
	{
		TurnAngle = Angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360에서 뺴게되면 양수로 각을 리턴하게 된다.
		TurnAngle = -Angle;
	}

	return TurnAngle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToTarget(AActor* OwnActor, AActor* Target)
{
	// 타겟과 현재 액터의 방향 벡터를 구함
	FVector direction = (Target->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// 현재 액터의 forward벡터와 direction 벡터의 내적, 라디안각도 구함
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// 호도 -> 각도 변환
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	//if (Angle <= 45.f)
	//	UE_LOG(LogTemp, Warning, TEXT("Angle to Target: %f"), Angle);

	return Angle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToTargetCross(AActor* OwnActor, AActor* Target)
{
	// 타겟과 현재 액터의 방향 벡터를 구함
	FVector direction = (Target->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// 현재 액터의 forward벡터와 direction 벡터의 내적, 라디안각도 구함
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// 호도 -> 각도 변환
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// 두 벡터의 외적, 액터 Forward벡터와 구르기 방향의 법선벡터를 구한다.
	FVector cross = FVector::CrossProduct(FVector::ForwardVector, direction);

	float TurnAngle = 0;

	// 액터 기준으로 왼쪽 또는 오른쪽 판단
	if (cross.Z > 0)
	{
		TurnAngle = Angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360에서 뺴게되면 양수로 각을 리턴하게 된다.
		TurnAngle = -Angle;
	}

	return TurnAngle;
}