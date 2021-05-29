// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_GhostTrail.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
ACS_GhostTrail::ACS_GhostTrail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
	PoseableMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ACS_GhostTrail::BeginPlay()
{
	Super::BeginPlay();
	Mat_GhostTrail_Dynamic = PoseableMesh->CreateDynamicMaterialInstance(3, Mat_GhostTrail);

	for (int i = 0; i < PoseableMesh->GetNumMaterials(); i++)
	{
		PoseableMesh->SetMaterial(i, Mat_GhostTrail_Dynamic);
	}

	GetWorldTimerManager().SetTimer(TrailDissapearTimeHandle, this, &ACS_GhostTrail::TrailDissapearTimer, GetWorld()->GetDeltaSeconds(), true, 0.2f);
}

void ACS_GhostTrail::SetPose(USkeletalMeshComponent* Mesh)
{
	PoseableMesh->CopyPoseFromSkeletalComponent(Mesh);
}

void ACS_GhostTrail::TrailDissapearTimer()
{
	Opacity = FMath::Lerp<float, float>(Opacity, -0.5f, 5.f * GetWorld()->GetDeltaSeconds());

	for (int i = 0; i < PoseableMesh->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(PoseableMesh->GetMaterial(i));
		mat->SetScalarParameterValue("Opacity", Opacity);
	}

	if (Opacity <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(TrailDissapearTimeHandle);
		Destroy();
	}
}