// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_SpawnTrigger.h"
#include "CS_Player.h"
#include "CS_EnemySpawner.h"
#include "EnumState.h"
#include "CustomStruct.h"

#include "Components/BoxComponent.h"

// Sets default values
ACS_SpawnTrigger::ACS_SpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//BoxComponent->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ACS_SpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACS_SpawnTrigger::Box_OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACS_SpawnTrigger::Box_OnOverlapEnd);

	bIsOverlapped = false;
}

// Called every frame
void ACS_SpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACS_SpawnTrigger::Box_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Overlapped")));

	ACS_Player* player = Cast<ACS_Player>(OtherActor);
	if (player && !bIsOverlapped)
	{
		bIsOverlapped = true;
		Spawner->SpawnTimerActive(SpawnerStruct);
	}
}

void ACS_SpawnTrigger::Box_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}