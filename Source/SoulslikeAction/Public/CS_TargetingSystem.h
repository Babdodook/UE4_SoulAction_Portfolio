// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CS_TargetingSystem.generated.h"

class ACS_Player;
class UCS_PlayerAnim;
class ACS_Enemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKEACTION_API UCS_TargetingSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCS_TargetingSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	ACS_Player* Player;
	UCS_PlayerAnim* PlayerAnim;

	TArray<ACS_Enemy*> DetectedEnemy;	// 최초 감지된 적들
	TArray<ACS_Enemy*> EnemyInScreen;	// 스크린에 존재하는 적들
	TArray<ACS_Enemy*> EnemyInSight;	// 플레이어 시야에 존재하는 적들
	ACS_Enemy* TargetEnemy;

	bool bIsLockOnTarget;

	FRotator DesiredRotator;

	UPROPERTY(EditAnywhere, Category = "System")
	bool bIsDrawDebug;
public:

	/** 적 검출 및 타겟팅 */
	void SearchTarget();

	/** 적 캐릭터 감지 */
	void DetectEnemyObjects();

	/** 스크린에 있는지 체크 */
	bool IsEnemyInScreen(AActor* Enemy);

	/** 적과 플레이어 사이에 벽이 있는지 체크 */
	bool IsWallExist(AActor* Enemy);

	/** 타겟으로 지정 */
	void SetTargetEnemy(ACS_Enemy* Target) { TargetEnemy = Target; }

	UFUNCTION(BlueprintCallable)
	ACS_Enemy* GetTargetEnemy() const { return TargetEnemy; }

	/** 타겟으로 카메라와 플레이어 회전 고정 */
	void LockOnTarget(ACS_Enemy* Target);

	/** 타겟으로 회전 해제 */
	void UnLockTarget();

	/** 타겟으로 컨트롤러 회전 */
	void RotateToTarget();

	UFUNCTION(BlueprintCallable, Category = "TargetSystem")
	bool GetIsLockOn() { return bIsLockOnTarget; }

	/** Draw확인용 */
	UFUNCTION(BlueprintCallable)
	void SetDrawDebug(bool Value) { bIsDrawDebug = Value; }

	UFUNCTION(BlueprintCallable)
	bool GetDrawDebug() { return bIsDrawDebug; }
};
