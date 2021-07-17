// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumState.h"

#include "CS_SpawnLocator.generated.h"

UCLASS()
class SOULSLIKEACTION_API ACS_SpawnLocator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_SpawnLocator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Locator Type")
	ESpawnLocatorStatus SpawnLocatorStatus;
};
