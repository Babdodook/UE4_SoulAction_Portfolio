// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CS_IKFootSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKEACTION_API UCS_IKFootSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCS_IKFootSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	AActor* OwnerActor;

public:
	// Leg IK ���� ���� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float RFootAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float LFootAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float AlphaDivValue;

	float RFootDistance;
	float LFootDistance;
	// Mesh Z��ġ offset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float HipOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float InterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float TraceDistance;

	// ĸ�� ������Ʈ ���� ����
	float CapsuleHalfHeight;
	
	// Socket Names
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName RFootSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName LFootSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName RootSocketName;

	// ���鿡 ���� �� ȸ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FRotator RFootRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FRotator LFootRotation;

	FVector RNormal;
	FVector LNormal;

public:
	void SetOwnerActor(AActor* Actor) { OwnerActor = Actor; }
	void SetCapsuleHalfHeight(float Value) { CapsuleHalfHeight = Value; }

	void IKProcessing();

private:
	/** �߹����� Ʈ���̽��� �Ͽ� ���� ��ġ�ؾ��� ���尪�� ��ȯ�Ѵ�. */
	float IKFootTrace(FName SocketName, FVector &Normal);

	float IKHipTrace(FName SocketName);

	/** ��Ʈ���� ĸ������ �󸶳� ���������ϴ��� offset�� ��ȯ�Ѵ�. */
	float HipDisplacement(float distanceToGround);

	float FootDisplacement(float distanceToGround, float footDistance);

	FRotator FootRotate(FRotator FootRotation, FVector Normal);
};
