#pragma once

#include "CoreMinimal.h"
#include "EnumState.h"
#include "Engine/DataTable.h"
#include "CustomStruct.generated.h"

USTRUCT(Atomic)
struct FSpawnerStruct
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	ESpawnLocatorStatus SpawnLocatorStatus;

	UPROPERTY(EditAnywhere)
	float SpawnDelay;

	UPROPERTY(EditAnywhere)
	int SpawnCount;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACS_Enemy> EnemyClassType;

	UPROPERTY(EditAnywhere)
	bool bAutoTarget;
};

USTRUCT(Atomic, BlueprintType)
struct FDamageTypeStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	EDamageType DamageType;

	UPROPERTY(EditAnywhere)
	float DamageAmount;
};

USTRUCT(Atomic, BlueprintType)
struct FAICombatStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float AttackRange;

	UPROPERTY(EditAnywhere)
	float AttackAngle;
};