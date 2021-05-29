// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CS_GhostTrail.generated.h"

UCLASS()
class SOULSLIKEACTION_API ACS_GhostTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_GhostTrail();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPoseableMeshComponent* PoseableMesh;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* Mat_GhostTrail;

	UMaterialInstanceDynamic* Mat_GhostTrail_Dynamic;

	void SetPose(USkeletalMeshComponent* Mesh);

	FTimerHandle TrailDissapearTimeHandle;
	float TrailDissapearTime;
	void TrailDissapearTimer();

	float Opacity = 2.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
