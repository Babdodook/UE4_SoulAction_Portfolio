// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumState.h"
#include "CustomStruct.h"
#include "CS_EnemySpawner.generated.h"

class ACS_Player;
class ACS_Enemy;
class ACS_Boss1;
class ACS_Mon1;
class ACS_TutorialAI;
class ACS_SpawnLocator;
class ACS_SpawnTrigger;
class ACS_Projectile;

UCLASS()
class SOULSLIKEACTION_API ACS_EnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_EnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Player> PlayerClass;
	ACS_Player* Player;

	// ������ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Boss1> Boss1Class;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Mon1> Mon1Class;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_TutorialAI> TutorialAIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Projectile> ProjectileClass;

	// ���� �Ϸ��� ��
	TArray<ACS_Enemy*> InWorld_Boss1Ary;
	TArray<ACS_Enemy*> InWorld_Mon1Ary;
	TArray<ACS_Enemy*> InWorld_TutorialAIAry;

	// �������� ���� ��
	TArray<ACS_Enemy*> OutWorld_Boss1Ary;
	TArray<ACS_Enemy*> OutWorld_Mon1Ary;
	TArray<ACS_Enemy*> OutWorld_TutorialAIAry;

	// ���� �Ϸ��� �߻�ü
	TArray<ACS_Projectile*> InWorld_ProjectileAry;
	// �������� ���� �߻�ü
	TArray<ACS_Projectile*> OutWorld_ProjectileAry;

	// ���� �������� Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_SpawnLocator> SpawnLocatorClass;
	TArray<AActor*> SpawnLocatorAry;

	// ���� �����صξ���� ��������
	AActor* SpawnOriginLocator;

	// ���� Ʈ���� Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_SpawnTrigger> SpawnTriggerClass;
	TArray<AActor*> SpawnTriggerAry;

	// ���� Ÿ�̸�
	FTimerHandle SpawnTimerHandle;

	// ���� ������ Ʈ������ ������ ���� �������͵�
	TArray<AActor*> CurrentLocators;

	// ���� ������ ��, ī��Ʈ, ������ ����
	FSpawnerStruct CurrentSpawnerStruct;

	int PrevLocatorIndex;
private:
	/** ���ο� �� ���� �����ϱ� */
	void CreateNewEnemyActor(TSubclassOf<ACS_Enemy> EnemyClassType);

	/** ���� �� Spawn Locator ã�� �迭�� �ֱ� */
	void FindAllSpawnLocators();

	/** ���� �� Spawn Trigger ã�� �迭�� �ֱ� */
	void FindAllSpawnTriggers();

	/** ������ Ʈ���������� �� ���� �����ϱ� */
	void SpawnEnemyActor(FTransform SpawnTransform);

	void SpawnEnemyActor_Direct(ESpawnLocatorStatus SpawnLocatorStatus, TSubclassOf<ACS_Enemy> EnemyClassType);

	void CreateNewProjectile();

	void SpawnTimer();

	FTransform GetSpawnTransformRandom();
	
public:
	void SpawnTimerActive(FSpawnerStruct SpawnerStruct);

	void SpawnProjectile(FTransform SpawnTransform);
};