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

	TArray<ACS_Enemy*> DetectedEnemy;	// ���� ������ ����
	TArray<ACS_Enemy*> EnemyInScreen;	// ��ũ���� �����ϴ� ����
	TArray<ACS_Enemy*> EnemyInSight;	// �÷��̾� �þ߿� �����ϴ� ����
	ACS_Enemy* TargetEnemy;

	bool bIsLockOnTarget;

	FRotator DesiredRotator;

	UPROPERTY(EditAnywhere, Category = "System")
	bool bIsDrawDebug;
public:

	/** �� ���� �� Ÿ���� */
	void SearchTarget();

	/** �� ĳ���� ���� */
	void DetectEnemyObjects();

	/** ��ũ���� �ִ��� üũ */
	bool IsEnemyInScreen(AActor* Enemy);

	/** ���� �÷��̾� ���̿� ���� �ִ��� üũ */
	bool IsWallExist(AActor* Enemy);

	/** Ÿ������ ���� */
	void SetTargetEnemy(ACS_Enemy* Target) { TargetEnemy = Target; }

	UFUNCTION(BlueprintCallable)
	ACS_Enemy* GetTargetEnemy() const { return TargetEnemy; }

	/** Ÿ������ ī�޶�� �÷��̾� ȸ�� ���� */
	void LockOnTarget(ACS_Enemy* Target);

	/** Ÿ������ ȸ�� ���� */
	void UnLockTarget();

	/** Ÿ������ ��Ʈ�ѷ� ȸ�� */
	void RotateToTarget();

	UFUNCTION(BlueprintCallable, Category = "TargetSystem")
	bool GetIsLockOn() { return bIsLockOnTarget; }

	/** DrawȮ�ο� */
	UFUNCTION(BlueprintCallable)
	void SetDrawDebug(bool Value) { bIsDrawDebug = Value; }

	UFUNCTION(BlueprintCallable)
	bool GetDrawDebug() { return bIsDrawDebug; }
};
