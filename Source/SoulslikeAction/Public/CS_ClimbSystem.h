// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CS_ClimbSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKEACTION_API UCS_ClimbSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCS_ClimbSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AActor* OwnerActor;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float ForwardTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HeightTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HipToLedgeMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HipToLedgeMax;

	bool bIsClimbingLedge;
	bool bIsHanging;

	bool bForwardHit;
	bool bHeightHit;

	FVector WallLocation;		// ForwardTrace ��Ʈ ��ġ
	FVector WallNormal;			// ��Ʈ�� ������Ʈ�� ���� ����
	FVector HeightLocation;		// HeightTrace ��Ʈ ��ġ

	void SetOwnerActor(AActor* Actor) { OwnerActor = Actor; }
	AActor* GetOwnerActor() { return OwnerActor; }

	void UpdateClimbSystem();

	/** ���� �������� ���Ǿ� Ʈ���̽� */
	void ForwardTrace();

	/** ���� ���� ���� ���Ǿ� Ʈ���̽� */
	void HeightTrace();

	/** Pelvis ������ ��ġ�� Height Location �� ������ ���� �ȿ� ��ġ���ִ��� ��ȯ�Ѵ�. */
	bool IsHipToLedge();

	/** ���� �Ŵ޸��� */
	void GrabLedge();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ExitLedge();

	FVector MoveToLocation();

	/** GrabLedge �Լ��� MoveComponentTo�� �������� ȣ���Ѵ�. */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedgeMoveFinished();

	/** ���� ��������? �Ŵ޸��� ���� */
	bool IsCanJump();

	/** ���� ������ */
	void ClimbLedge();

	/** ���� ������ �� */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbingEnd();
};
