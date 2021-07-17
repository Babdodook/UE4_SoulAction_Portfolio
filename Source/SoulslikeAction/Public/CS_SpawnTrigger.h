// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumState.h"
#include "CustomStruct.h"
#include "GameFramework/Actor.h"
#include "CS_SpawnTrigger.generated.h"

UCLASS()
class SOULSLIKEACTION_API ACS_SpawnTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_SpawnTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComponent;

	class ACS_EnemySpawner* Spawner;

	UPROPERTY(EditInstanceOnly)
	int TriggerIndex;

	UPROPERTY(VisibleAnywhere)
	bool bIsOverlapped;

public:
	UPROPERTY(EditAnywhere)
	FSpawnerStruct SpawnerStruct;
public:
	UFUNCTION()
	virtual void Box_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void Box_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	int GetTriggerIndex() { return TriggerIndex; }

	bool GetIsOverlapped() { return bIsOverlapped; }

	void SetSpawner(class ACS_EnemySpawner* spawner) { Spawner = spawner; }
};
