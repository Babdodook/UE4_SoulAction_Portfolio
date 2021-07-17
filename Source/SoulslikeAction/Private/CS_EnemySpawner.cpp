// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_EnemySpawner.h"
#include "CS_Enemy.h"
#include "CS_SpawnLocator.h"
#include "CS_SpawnTrigger.h"
#include "EnumState.h"
#include "CS_Boss1.h"
#include "CS_Mon1.h"
#include "CS_Player.h"
#include "CS_TutorialAI.h"
#include "CS_Projectile.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ACS_EnemySpawner::ACS_EnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACS_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<ACS_Player>(UGameplayStatics::GetActorOfClass(GetWorld(), PlayerClass));
	FindAllSpawnLocators();
	FindAllSpawnTriggers();

	PrevLocatorIndex = -1;

	// 적 액터 생성
	for (int i = 0; i < 30; i++)
	{
		CreateNewEnemyActor(Mon1Class);
	}
	CreateNewEnemyActor(Boss1Class);
	CreateNewEnemyActor(TutorialAIClass);

	//// 발사체 생성
	//for (int i = 0; i < 10; i++)
	//{
	//	CreateNewProjectile();
	//}

	SpawnEnemyActor_Direct(ESpawnLocatorStatus::SLS_BossPoint_1, Boss1Class);
}

void ACS_EnemySpawner::CreateNewEnemyActor(TSubclassOf<ACS_Enemy> EnemyClassType)
{
	ACS_Enemy* Enemy = GetWorld()->SpawnActor<ACS_Enemy>(EnemyClassType);
	//Enemy->SetFolderPath("/Level1/Enemys");

	Enemy->SetSpawner(this);
	Enemy->SetActorTransform(SpawnOriginLocator->GetActorTransform());
	
	if (EnemyClassType == Boss1Class)
	{
		OutWorld_Boss1Ary.Add(Enemy);
	}
	else if (EnemyClassType == Mon1Class)
	{
		OutWorld_Mon1Ary.Add(Enemy);
	}
	else if (EnemyClassType == TutorialAIClass)
	{
		OutWorld_TutorialAIAry.Add(Enemy);
	}
}

void ACS_EnemySpawner::FindAllSpawnLocators()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnLocatorClass, SpawnLocatorAry);

	// SpawnOriginLocator 를 찾아서 지정한다
	for (AActor* actor : SpawnLocatorAry)
	{
		ACS_SpawnLocator* locator = Cast<ACS_SpawnLocator>(actor);
		if (locator != nullptr)
		{
			if (locator->SpawnLocatorStatus == ESpawnLocatorStatus::SLS_OriginPoint)
			{
				SpawnOriginLocator = locator;
				break;
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("%s"), *actor->GetName());
	}
}

void ACS_EnemySpawner::FindAllSpawnTriggers()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnTriggerClass, SpawnTriggerAry);

	for (AActor* actor : SpawnTriggerAry)
	{
		ACS_SpawnTrigger* trigger = Cast<ACS_SpawnTrigger>(actor);
		if (trigger != nullptr)
		{
			trigger->SetSpawner(this);
			UE_LOG(LogTemp, Log, TEXT("%s %d"), *actor->GetName(), trigger->GetTriggerIndex());
		}
	}
}

void ACS_EnemySpawner::SpawnEnemyActor(FTransform SpawnTransform)
{
	ACS_Enemy* enemyActor = nullptr;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SpawnEnemyActor")));

	if (CurrentSpawnerStruct.EnemyClassType == Boss1Class)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Boss1Class")));

		InWorld_Boss1Ary.AddUnique(OutWorld_Boss1Ary[0]);
		enemyActor = InWorld_Boss1Ary.Last();

		OutWorld_Boss1Ary.Remove(enemyActor);
	}
	else if (CurrentSpawnerStruct.EnemyClassType == Mon1Class)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Mon1Class")));

		InWorld_Mon1Ary.AddUnique(OutWorld_Mon1Ary[0]);
		enemyActor = InWorld_Mon1Ary.Last();

		OutWorld_Mon1Ary.Remove(enemyActor);
	}
	else if (CurrentSpawnerStruct.EnemyClassType == TutorialAIClass)
	{
		InWorld_TutorialAIAry.AddUnique(OutWorld_TutorialAIAry[0]);
		enemyActor = InWorld_TutorialAIAry.Last();

		OutWorld_TutorialAIAry.Remove(enemyActor);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s"), *enemyActor->GetName()));
	enemyActor->SetActorTransform(SpawnTransform);

	if (CurrentSpawnerStruct.bAutoTarget)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("bAutoTarget")));
		enemyActor->ActiveAIBehaviour(Player);
	}
}

void ACS_EnemySpawner::SpawnTimerActive(FSpawnerStruct SpawnerStruct)
{
	CurrentSpawnerStruct = SpawnerStruct;
	CurrentLocators.Empty();
	PrevLocatorIndex = -1;

	for (AActor* actor : SpawnLocatorAry)
	{
		ACS_SpawnLocator* locator = Cast<ACS_SpawnLocator>(actor);
		if (locator != nullptr)
		{
			if(locator->SpawnLocatorStatus == CurrentSpawnerStruct.SpawnLocatorStatus)
				CurrentLocators.AddUnique(locator);
		}
	}

	if (!GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ACS_EnemySpawner::SpawnTimer, CurrentSpawnerStruct.SpawnDelay, true);
	}
}

void ACS_EnemySpawner::SpawnTimer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SpawnTimer")));

	CurrentSpawnerStruct.SpawnCount--;
	SpawnEnemyActor(GetSpawnTransformRandom());

	if (CurrentSpawnerStruct.SpawnCount <= 0)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

FTransform ACS_EnemySpawner::GetSpawnTransformRandom()
{
	if (CurrentLocators.Num() == 1)
	{
		return CurrentLocators[0]->GetActorTransform();
	}

	while (1)
	{
		int32 randnum = FMath::RandRange(1, CurrentLocators.Num());

		if (randnum == PrevLocatorIndex)
			continue;
		else
		{
			PrevLocatorIndex = randnum;
			return CurrentLocators[randnum-1]->GetActorTransform();
		}
	}
}

void ACS_EnemySpawner::CreateNewProjectile()
{
	ACS_Projectile* Projectile = GetWorld()->SpawnActor<ACS_Projectile>(ProjectileClass);

	Projectile->SetTargetActor(Player);
	Projectile->SetSpanwer(this);
	Projectile->SetActorTransform(SpawnOriginLocator->GetActorTransform());

	OutWorld_ProjectileAry.Add(Projectile);
}

void ACS_EnemySpawner::SpawnProjectile(FTransform SpawnTransform)
{
	InWorld_ProjectileAry.AddUnique(OutWorld_ProjectileAry[0]);
	ACS_Projectile* projectile = InWorld_ProjectileAry.Last();

	OutWorld_ProjectileAry.Remove(projectile);

	projectile->SetActorTransform(SpawnTransform);
	projectile->LaunchProjectile();
}

void ACS_EnemySpawner::SpawnEnemyActor_Direct(ESpawnLocatorStatus SpawnLocatorStatus, TSubclassOf<ACS_Enemy> EnemyClassType)
{
	ACS_Enemy* enemyActor = nullptr;

	if (EnemyClassType == Boss1Class)
	{
		InWorld_Boss1Ary.AddUnique(OutWorld_Boss1Ary[0]);
		enemyActor = InWorld_Boss1Ary.Last();

		OutWorld_Boss1Ary.Remove(enemyActor);
	}
	else if (EnemyClassType == Mon1Class)
	{
		InWorld_Mon1Ary.AddUnique(OutWorld_Mon1Ary[0]);
		enemyActor = InWorld_Mon1Ary.Last();

		OutWorld_Mon1Ary.Remove(enemyActor);
	}
	else if (EnemyClassType == TutorialAIClass)
	{
		InWorld_TutorialAIAry.AddUnique(OutWorld_TutorialAIAry[0]);
		enemyActor = InWorld_TutorialAIAry.Last();

		OutWorld_TutorialAIAry.Remove(enemyActor);
	}

	FTransform SpawnTransform = FTransform::Identity;
	for (auto& locator : SpawnLocatorAry)
	{
		ACS_SpawnLocator* _locator = Cast<ACS_SpawnLocator>(locator);
		if (_locator->SpawnLocatorStatus == SpawnLocatorStatus)
		{
			SpawnTransform = _locator->GetActorTransform();

			break;
		}
	}

	enemyActor->SetActorTransform(SpawnTransform);
}