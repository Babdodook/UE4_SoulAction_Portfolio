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
    FVector startLoc = GetActorLocation();      // �߻� ����
    FVector targetLoc = TargetActor->GetActorLocation();  // Ÿ�� ����.
    float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
    FVector outVelocity = FVector::ZeroVector;   // ��� Velocity
    if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, outVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue))
    {
        FPredictProjectilePathParams predictParams(20.0f, startLoc, outVelocity, 15.0f);   // 20: tracing ������ ������Ÿ�� ũ��, 15: �ù����̼ǵǴ� Max �ð�(��)
        predictParams.DrawDebugTime = 15.0f;     //����� ���� �������� �ð� (��)
        predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime �� �����ϸ� EDrawDebugTrace::Type::ForDuration �ʿ�.
        predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
        FPredictProjectilePathResult result;
        UGameplayStatics::PredictProjectilePath(this, predictParams, result);

        StaticMeshCompnent->AddImpulse(outVelocity);
        //GetMesh()->AddImpulse(outVelocity);
        //objectToSend->AddImpluse(outVelocity); // objectToSend�� �߻�ü
    }
}