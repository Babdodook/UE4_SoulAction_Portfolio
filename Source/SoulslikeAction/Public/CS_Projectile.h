// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "CS_Projectile.generated.h"

class ACS_EnemySpawner;

UCLASS()
class SOULSLIKEACTION_API ACS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	AActor* TargetActor;

	ACS_EnemySpawner* Spawner;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshCompnent;
public:
	void SetTargetActor(AActor* target) { TargetActor = target; }
	AActor* GetTargetActor() { return TargetActor; }

	void SetSpanwer(ACS_EnemySpawner* _Spawner) { Spawner = _Spawner; }
	ACS_EnemySpawner* GetSpanwer() { return Spawner; }
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LaunchProjectile();
};
