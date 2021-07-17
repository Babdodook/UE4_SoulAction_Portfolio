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

	// Ʈ���̽� ĸ�� ������
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

	FVector WallLocation;		// ForwardTrace ��Ʈ ��ġ
	FVector WallNormal;			// ��Ʈ�� ������Ʈ�� ���� ����
	FVector HeightLocation;		// HeightTrace ��Ʈ ��ġ
	float WallZSize;
	float WallZLocation;

	// �������� �̵��� Ʈ������
	FVector TargetRelativeLocation;
	FRotator TargetRelativeRotation;

	void SetOwnerActor(AActor* Actor) { OwnerActor = Actor; }
	AActor* GetOwnerActor() { return OwnerActor; }

	void UpdateClimbSystem();

	/** ���� �������� ���Ǿ� Ʈ���̽� */
	void ForwardTrace();

	/** ���� ���� ���� ���Ǿ� Ʈ���̽� */
	void HeightTrace();

	/** �¿� �̵� Ʈ���̽� */
	void SideMoveTrace(class UArrowComponent* Arrow, bool& bCanMove, bool& bCanJump, bool& bCanTurn);

	/** �¿� ���� Ʈ���̽� */
	void EdgeTrace(class UArrowComponent* Arrow, bool bCanMove, bool &bCanJump);

	/** �¿� �𼭸� �̵� Ʈ���̽� */
	void TurnTrace(class UArrowComponent* Arrow, bool bCanMove, bool& bCanTurn);

	void JumpUpTrace();

	/** ������ ��ġ�� ������ �Ŵ޸��� �ִ� �������� ��ȯ�Ѵ�. */
	bool IsHipToLedge();

	/** ���� �Ŵ޸��� */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedge();

	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void ExitLedge();

	FVector MoveToLocation();

	/** GrabLedge �Լ��� MoveComponentTo�� �������� ȣ���Ѵ�. */
	UFUNCTION(BlueprintCallable, Category = "Climb System")
	void GrabLedgeMoveFinished();

	/** ���� ������ */
	void ClimbLedge();

	/** ���� ������ �� */
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
