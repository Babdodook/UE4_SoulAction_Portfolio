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

	FVector WallLocation;		// ForwardTrace 히트 위치
	FVector WallNormal;			// 히트된 오브젝트의 법선 벡터
	FVector HeightLocation;		// HeightTrace 히트 위치

	void SetOwnerActor(AActor* Actor) { OwnerActor = Actor; }
	AActor* GetOwnerActor() { return OwnerActor; }

	void UpdateClimbSystem();

	/** 액터 전방으로 스피어 트레이스 */
	void ForwardTrace();

	/** 액터 전방 위로 스피어 트레이스 */
	void HeightTrace();

	/** Pelvis 소켓의 위치가 Height Location 과 적정한 범위 안에 위치해있는지 반환한다. */
	bool IsHipToLedge();

	/** 난간 매달리기 */
	void GrabLedge();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ExitLedge();

	FVector MoveToLocation();

	/** GrabLedge 함수의 MoveComponentTo가 끝났을때 호출한다. */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedgeMoveFinished();

	/** 점프 가능인지? 매달리기 위함 */
	bool IsCanJump();

	/** 난간 오르기 */
	void ClimbLedge();

	/** 난간 오르기 끝 */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbingEnd();
};
