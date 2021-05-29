// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_Enemy.h"
#include "CS_Boss1.generated.h"

class ACS_GhostTrail;

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API ACS_Boss1 : public ACS_Enemy
{
	GENERATED_BODY()

public:

	ACS_Boss1();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_GhostTrail> GhostTrailClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SpawnGhostTrail(float SpawnDelay);

	FTimerHandle SpawnGhostTrailTimerHandle;
	float SpawnGhostTrailTime;
	void SpawnGhostTrailTimer();

	virtual void Executed(AActor* Causer, FVector Location) override;

	virtual void ExecutedTakeDamage(float DamageAmount) override;

	virtual void ExecutedEnd() override;

	virtual void Groggy(AActor* Causer) override;

	virtual bool IsExecuteReady() override;

	// Functions
protected:
	virtual void AttachWeapon() override;

	virtual void CombatBehaviour() override;

	virtual void Attack() override;

	virtual void Die() override;
};
