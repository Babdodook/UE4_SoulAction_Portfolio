// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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
	class ACS_Player* playerClass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	// 트레이스 캡슐 반지름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float ForwardTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HeightTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float SideTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float EdgeTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float TurnTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float UpTraceRadius;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float TraceCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HipToLedgeMin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float HipToLedgeMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	float SideMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bIsClimbingLedge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bIsHanging;
	
	bool bForwardHit;
	bool bHeightHit;
	bool bLeftHit;
	bool bRightHit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanMoveLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanMoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanJumpLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanJumpRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanTurnLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanTurnRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bIsTurning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bCanJumpUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bIsJumpingUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb System")
	bool bIsTurnBack;

	FVector WallLocation;		// ForwardTrace 히트 위치
	FVector WallNormal;			// 히트된 오브젝트의 법선 벡터
	FVector HeightLocation;		// HeightTrace 히트 위치
	float WallZSize;
	float WallZLocation;

	// 난간으로 이동할 트랜스폼
	FVector TargetRelativeLocation;
	FRotator TargetRelativeRotation;

	void SetOwnerActor(AActor* Actor) { OwnerActor = Actor; }
	AActor* GetOwnerActor() { return OwnerActor; }

	void UpdateClimbSystem();

	/** 액터 전방으로 스피어 트레이스 */
	void ForwardTrace();

	/** 액터 전방 위로 스피어 트레이스 */
	void HeightTrace();

	/** 좌우 이동 트레이스 */
	void SideMoveTrace(class UArrowComponent* Arrow, bool& bCanMove, bool& bCanJump, bool& bCanTurn);

	/** 좌우 점프 트레이스 */
	void EdgeTrace(class UArrowComponent* Arrow, bool bCanMove, bool &bCanJump);

	/** 좌우 모서리 이동 트레이스 */
	void TurnTrace(class UArrowComponent* Arrow, bool bCanMove, bool& bCanTurn);

	void JumpUpTrace();

	/** 소켓의 위치가 난간에 매달릴수 있는 높이인지 반환한다. */
	bool IsHipToLedge();

	/** 난간 매달리기 */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedge();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ExitLedge();

	FVector MoveToLocation();

	/** GrabLedge 함수의 MoveComponentTo가 끝났을때 호출한다. */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedgeMoveFinished();

	/** 난간 오르기 */
	void ClimbLedge();

	/** 난간 오르기 끝 */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbUpEnd();

	void SideMovement();

	void ClimbJumpSide();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbJumpSideEnd();

	void ClimbTurn();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbTurnEnd();

	void ClimbJumpUp();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ClimbJumpUpEnd();
};
