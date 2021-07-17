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

	// 스폰할 몬스터 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Boss1> Boss1Class;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Mon1> Mon1Class;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_TutorialAI> TutorialAIClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Projectile> ProjectileClass;

	// 스폰 완료한 적
	TArray<ACS_Enemy*> InWorld_Boss1Ary;
	TArray<ACS_Enemy*> InWorld_Mon1Ary;
	TArray<ACS_Enemy*> InWorld_TutorialAIAry;

	// 스폰되지 않은 적
	TArray<ACS_Enemy*> OutWorld_Boss1Ary;
	TArray<ACS_Enemy*> OutWorld_Mon1Ary;
	TArray<ACS_Enemy*> OutWorld_TutorialAIAry;

	// 스폰 완료한 발사체
	TArray<ACS_Projectile*> InWorld_ProjectileAry;
	// 스폰되지 않은 발사체
	TArray<ACS_Projectile*> OutWorld_ProjectileAry;

	// 스폰 로케이터 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_SpawnLocator> SpawnLocatorClass;
	TArray<AActor*> SpawnLocatorAry;

	// 적을 생성해두어야할 로케이터
	AActor* SpawnOriginLocator;

	// 스폰 트리거 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_SpawnTrigger> SpawnTriggerClass;
	TArray<AActor*> SpawnTriggerAry;

	// 스폰 타이머
	FTimerHandle SpawnTimerHandle;

	// 현재 스폰할 트랜스폼 정보를 가진 로케이터들
	TArray<AActor*> CurrentLocators;

	// 현재 스폰할 적, 카운트, 딜레이 정보
	FSpawnerStruct CurrentSpawnerStruct;

	int PrevLocatorIndex;
private:
	/** 새로운 적 액터 생성하기 */
	void CreateNewEnemyActor(TSubclassOf<ACS_Enemy> EnemyClassType);

	/** 월드 상에 Spawn Locator 찾아 배열에 넣기 */
	void FindAllSpawnLocators();

	/** 월드 상에 Spawn Trigger 찾아 배열에 넣기 */
	void FindAllSpawnTriggers();

	/** 지정한 트랜스폼으로 적 액터 스폰하기 */
	void SpawnEnemyActor(FTransform SpawnTransform);

	void SpawnEnemyActor_Direct(ESpawnLocatorStatus SpawnLocatorStatus, TSubclassOf<ACS_Enemy> EnemyClassType);

	void CreateNewProjectile();

	void SpawnTimer();

	FTransform GetSpawnTransformRandom();
	
public:
	void SpawnTimerActive(FSpawnerStruct SpawnerStruct);

	void SpawnProjectile(FTransform SpawnTransform);
};