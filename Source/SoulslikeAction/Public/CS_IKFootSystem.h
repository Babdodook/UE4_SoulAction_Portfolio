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
	// Leg IK 알파 블렌드 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float RFootAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float LFootAlpha;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float AlphaDivValue;

	float RFootDistance;
	float LFootDistance;
	// Mesh Z위치 offset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float HipOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float InterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	float TraceDistance;

	// 캡슐 컴포넌트 절반 높이
	float CapsuleHalfHeight;
	
	// Socket Names
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName RFootSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName LFootSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK Foot")
	FName RootSocketName;

	// 지면에 따른 발 회전값
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
	/** 발밑으로 트레이스를 하여 발이 위치해야할 블렌드값을 반환한다. */
	float IKFootTrace(FName SocketName, FVector &Normal);

	float IKHipTrace(FName SocketName);

	/** 루트본이 캡슐보다 얼마나 내려가야하는지 offset을 반환한다. */
	float HipDisplacement(float distanceToGround);

	float FootDisplacement(float distanceToGround, float footDistance);

	FRotator FootRotate(FRotator FootRotation, FVector Normal);
};
