// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_Projectile.h"

#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ACS_Projectile::ACS_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    StaticMeshCompnent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshCompnent"));
    StaticMeshCompnent->SetupAttachment(GetRootComponent());


}

// Called when the game starts or when spawned
void ACS_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
    
}

// Called every frame
void ACS_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACS_Projectile::LaunchProjectile()
{
    FVector startLoc = GetActorLocation();      // 발사 지점
    FVector targetLoc = TargetActor->GetActorLocation();  // 타겟 지점.
    float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
    FVector outVelocity = FVector::ZeroVector;   // 결과 Velocity
    if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
    {
        FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 15.0f);   // 20: tracing 보여질 프로젝타일 크기, 15: 시물레이션되는 Max 시간(초)
        predictParams.DrawDebugTime = 15.0f;     //디버그 라인 보여지는 시간 (초)
        predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime 을 지정하면 EDrawDebugTrace::Type::ForDuration 필요.
        predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
        FPredictProjectilePathResult result;
        UGameplayStatics::PredictProjectilePath(this, predictParams, result);

        StaticMeshCompnent->AddImpulse(outVelocity);
        //GetMesh()->AddImpulse(outVelocity);
        //objectToSend->AddImpluse(outVelocity); // objectToSend는 발사체
    }
}