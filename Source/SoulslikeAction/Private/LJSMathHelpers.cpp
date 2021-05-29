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
	// �� ������ ����, Vec1�� Vec2�� ���̰��� ���Ѵ�.
	float dot = FVector::DotProduct(Vec1, Vec2);

	// Radian�� Degree�� ��ȯ
	float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// �� ������ ����, Vec1�� Vec2�� �������͸� ���Ѵ�.
	FVector cross = FVector::CrossProduct(Vec1, Vec2);

	float TurnAngle = 0;

	// ���� �������� ���� �Ǵ� ���������� ȸ������ �Ǻ�
	if (cross.Z > 0)
	{
		TurnAngle = angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360���� ���ԵǸ� ����� ���� �����ϰ� �ȴ�.
		TurnAngle = -angle;
	}

	return TurnAngle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToDamageCauser(AActor* OwnActor, AActor* DamageCauser)
{
	// Ÿ�ٰ� ���� ������ ���� ���͸� ����
	FVector direction = (DamageCauser->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// ���� ������ forward���Ϳ� direction ������ ����, ���Ȱ��� ����
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// ȣ�� -> ���� ��ȯ
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// �� ������ ����, ���� Forward���Ϳ� ������ ������ �������͸� ���Ѵ�.
	FVector cross = FVector::CrossProduct(FVector::ForwardVector, direction);

	float TurnAngle = 0;

	// ���� �������� ���� �Ǵ� ������ �Ǵ�
	if (cross.Z > 0)
	{
		TurnAngle = Angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360���� ���ԵǸ� ����� ���� �����ϰ� �ȴ�.
		TurnAngle = -Angle;
	}

	return TurnAngle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToTarget(AActor* OwnActor, AActor* Target)
{
	// Ÿ�ٰ� ���� ������ ���� ���͸� ����
	FVector direction = (Target->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// ���� ������ forward���Ϳ� direction ������ ����, ���Ȱ��� ����
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// ȣ�� -> ���� ��ȯ
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	//if (Angle <= 45.f)
	//	UE_LOG(LogTemp, Warning, TEXT("Angle to Target: %f"), Angle);

	return Angle;
}

LJS_MATH_FORCEINLINE
float ULJSMathHelpers::GetAngleToTargetCross(AActor* OwnActor, AActor* Target)
{
	// Ÿ�ٰ� ���� ������ ���� ���͸� ����
	FVector direction = (Target->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// ���� ������ forward���Ϳ� direction ������ ����, ���Ȱ��� ����
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// ȣ�� -> ���� ��ȯ
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	// �� ������ ����, ���� Forward���Ϳ� ������ ������ �������͸� ���Ѵ�.
	FVector cross = FVector::CrossProduct(FVector::ForwardVector, direction);

	float TurnAngle = 0;

	// ���� �������� ���� �Ǵ� ������ �Ǵ�
	if (cross.Z > 0)
	{
		TurnAngle = Angle;
	}
	else if (cross.Z < 0)
	{
		//TurnAngle = 360 - angle; //360���� ���ԵǸ� ����� ���� �����ϰ� �ȴ�.
		TurnAngle = -Angle;
	}

	return TurnAngle;
}