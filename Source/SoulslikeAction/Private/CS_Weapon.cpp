// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "CS_Player.h"
#include "CS_Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACS_Weapon::ACS_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(Root);

	WeaponMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh2"));
	WeaponMesh2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh2->SetupAttachment(Root);

	WeaponHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponHandle"));
	WeaponHandle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponHandle->SetupAttachment(Root);

	WeaponHandleCopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponHandleCopy"));
	WeaponHandleCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponHandleCopy->SetupAttachment(Root);

	WeaponBlade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBlade"));
	WeaponBlade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBlade->SetupAttachment(Root);

	WeaponBladeCopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBladeCopy"));
	WeaponBladeCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBladeCopy->SetupAttachment(Root);

	bIsAttack = false;
}

void ACS_Weapon::BeginPlay()
{
	Super::BeginPlay();
}

void ACS_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACS_Weapon::Equip(ACS_Player* Character)
{
	if (Character)
	{
		const USkeletalMeshSocket* R_HandSocket = Character->GetMesh()->GetSocketByName("Socket_Weapon_R");
		if (R_HandSocket)
		{
			R_HandSocket->AttachActor(this, Character->GetMesh());
		}
	}
}

void ACS_Weapon::PlayerWeaponCollisionCheck()
{
	ACS_Player* pOwner = Cast<ACS_Player>(GetOwner());
	if (pOwner == nullptr) return;

	// ��Ʈ ��� �����
	TArray<FHitResult> OutHits;
	FHitResult lineHits;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());		// �±�, �ܼ�(false) / ����(true) Ʈ���̽�, ������ ���
	
	// Ʈ���̽� ����, �� ������
	FVector StartPoint = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketLocation("Socket_collision_r");
	FVector EndPoint = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketLocation("Socket_collision_l");
	
	float halfLength = (EndPoint - StartPoint).Size() * 0.5f;
	FVector Center = StartPoint + StartPoint.GetUnsafeNormal() * halfLength;
	FQuat CapsuleRot = FRotationMatrix::MakeFromY((StartPoint - EndPoint).GetSafeNormal()).ToQuat();
	FColor DrawColor = FColor::Red;
	float DebugLifeTime = 2.f;
	// �浹 Ȯ�ο� �׸���
	FCollisionShape DebugShape = FCollisionShape::MakeCapsule(FVector(15.f, 5.f, halfLength));
	//DrawDebugCapsule(GetWorld(), Center, DebugShape.GetCapsuleHalfHeight() ,DebugShape.GetCapsuleRadius(), CapsuleRot, DrawColor, false, DebugLifeTime);
	//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Yellow, false, 3.f);

	// ���� Ʈ���̽�
	bool isLineHits = GetWorld()->LineTraceSingleByChannel(lineHits, StartPoint, EndPoint, ECC_WorldDynamic, TraceParams);

	if (isLineHits)
	{
		ACS_Enemy* enemy = Cast<ACS_Enemy>(lineHits.GetActor());
		if (enemy != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ApplyDamage to Enemy Actor"));
			if (DamageAmount == UGameplayStatics::ApplyDamage(enemy, DamageAmount, NULL, GetOwner(), NULL))
			{
				pOwner->PlayCamShake();
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("hit result: %s"), *lineHits.Actor->GetName());
	}

	// ���� ��Ƽ
	bool isHit = false;
	//isHit = GetWorld()->SweepMultiByChannel(OutHits, StartPoint, EndPoint, FQuat::Identity, ECC_WorldDynamic, DebugShape, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			if (GEngine)
			{
				// screen log information on what was hit
				UE_LOG(LogTemp, Warning, TEXT("hit result: %s"), *Hit.Actor->GetName());
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *Hit.Actor->GetName()));
				// uncommnet to see more info on sweeped actor
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("All Hit Information: %s"), *Hit.ToString()));
			}
		}
	}
}

void ACS_Weapon::EnemyWeaponCollisionCheck()
{
	ACS_Enemy* eOwner = Cast<ACS_Enemy>(GetOwner());
	if (eOwner == nullptr) return;

	// ��Ʈ ��� �����
	TArray<FHitResult> OutHits;
	FHitResult lineHits;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());		// �±�, �ܼ�(false) / ����(true) Ʈ���̽�, ������ ���

	// Ʈ���̽� ����, �� ������
	FVector StartPoint = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketLocation("Socket_collision_middle");
	FVector EndPoint = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketLocation("Socket_collision_l");

	float halfLength = (EndPoint - StartPoint).Size() * 0.5f;
	FVector Center = StartPoint + StartPoint.GetUnsafeNormal() * halfLength;
	FQuat CapsuleRot = FRotationMatrix::MakeFromY((StartPoint - EndPoint).GetSafeNormal()).ToQuat();
	FColor DrawColor = FColor::Red;
	float DebugLifeTime = 2.f;
	// �浹 Ȯ�ο� �׸���
	FCollisionShape DebugShape = FCollisionShape::MakeCapsule(FVector(15.f, 5.f, halfLength));
	//DrawDebugCapsule(GetWorld(), Center, DebugShape.GetCapsuleHalfHeight() ,DebugShape.GetCapsuleRadius(), CapsuleRot, DrawColor, false, DebugLifeTime);
	//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Yellow, false, 3.f);

	// ���� Ʈ���̽�
	bool isLineHits = GetWorld()->LineTraceSingleByChannel(lineHits, StartPoint, EndPoint, ECC_WorldDynamic, TraceParams);

	if (isLineHits)
	{
		ACS_Player* player = Cast<ACS_Player>(lineHits.GetActor());
		if (player != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ApplyDamage to Enemy Actor"));
			UGameplayStatics::ApplyDamage(player, DamageAmount, NULL, GetOwner(), NULL);
		}

		//UE_LOG(LogTemp, Warning, TEXT("hit result: %s"), *lineHits.Actor->GetName());
	}

	// ���� ��Ƽ
	bool isHit = false;
	//isHit = GetWorld()->SweepMultiByChannel(OutHits, StartPoint, EndPoint, FQuat::Identity, ECC_WorldDynamic, DebugShape, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			if (GEngine)
			{
				// screen log information on what was hit
				UE_LOG(LogTemp, Warning, TEXT("hit result: %s"), *Hit.Actor->GetName());
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *Hit.Actor->GetName()));
				// uncommnet to see more info on sweeped actor
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("All Hit Information: %s"), *Hit.ToString()));
			}
		}
	}
}