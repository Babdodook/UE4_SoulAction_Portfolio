// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_TargetingSystem.h"
#include "CS_Player.h"
#include "CS_PlayerAnim.h"
#include "CS_Enemy.h"
#include "CS_Weapon.h"
#include "EnumState.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UCS_TargetingSystem::UCS_TargetingSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UCS_TargetingSystem::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ACS_Player>(GetOwner());
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	PlayerAnim = Cast<UCS_PlayerAnim>(OwnerCharacter->GetMesh()->GetAnimInstance());
	bIsLockOnTarget = false;
	PlayerAnim->bIsLockOn = false;
	bIsDrawDebug = false;
	TargetEnemy = nullptr;	

	SetTargetSearchType(ETargetSearchType::TST_None);

	SetScreenSize(ScreenWidth, ScreenHeight);
}

void UCS_TargetingSystem::SearchTarget()
{
	// �̹� Ÿ�������� �� ����
	if (GetTargetEnemy() != nullptr && GetTargetSearchType() == ETargetSearchType::TST_None)
	{
		// Ÿ�� ����
		UnLockTarget();
		return;
	}


	// �� ����
	DetectEnemyObjects();
	// ������ �� ����. return
	if (DetectedEnemy.Num() == 0) return;


	SetScreenSize(ScreenWidth, ScreenHeight);
	// ��ũ���� �ִ��� Ȯ��
	for (auto& enemy : DetectedEnemy)
	{
		if (IsEnemyInScreen(enemy))
		{
			EnemyInScreen.Add(enemy);
			//UE_LOG(LogTemp, Warning, TEXT("%s is in Screen"), *enemy->GetName());
		}
	}
	// ��ũ���� �����ϴ� �� ����. return
	if (EnemyInScreen.Num() == 0) return;


	// ��ũ���� �����ϴ� ���� �÷��̾� ���̿� ���� �ִ��� Ȯ��
	for (auto& enemy : EnemyInScreen)
	{
		if (IsWallExist(enemy))
		{
			EnemyInSight.Add(enemy);
			//UE_LOG(LogTemp, Warning, TEXT("%s is in Screen"), *enemy->GetName());
		}
	}
	// �÷��̾� �þ߿� �����ϴ� �� ����. return
	if (EnemyInSight.Num() == 0) return;


	switch (GetTargetSearchType())
	{
	case ETargetSearchType::TST_None:
		{
			// �÷��̾�� ���� ����� ���� ã�Ƴ�
			ACS_Enemy* minDistanceEnemy = EnemyInSight[0];
			for (auto& enemy : EnemyInSight)
			{
				if (FVector::Dist(Player->GetActorLocation(), minDistanceEnemy->GetActorLocation()) >= FVector::Dist(Player->GetActorLocation(), enemy->GetActorLocation()))
				{
					minDistanceEnemy = enemy;
				}
			}

			// ���� ����� �� Ÿ������ ����
			LockOnTarget(minDistanceEnemy);
		}

		break;
	case ETargetSearchType::TST_WheelUp:
	case ETargetSearchType::TST_WheelDown:

		ACS_Enemy* minDistanceEnemy = ChangeTargetEnemy();

		if (minDistanceEnemy != nullptr)
		{
			UnLockTarget();
			LockOnTarget(minDistanceEnemy);
		}

		break;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Now %s is Target"), *TargetEnemy->GetName());
}

void UCS_TargetingSystem::DetectEnemyObjects()
{
	// ��� �迭 �ʱ�ȭ
	DetectedEnemy.Empty();
	EnemyInScreen.Empty();
	EnemyInSight.Empty();

	TArray<FHitResult> OutHits;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// �浹 Ȯ�ο� �׸���
	FCollisionShape DebugShape = FCollisionShape::MakeSphere(600.f);
	if(bIsDrawDebug)
		DrawDebugSphere(GetWorld(), Player->GetActorLocation(), DebugShape.GetSphereRadius(), 10, FColor::Blue, false, 1.5f);

	// ���� ��Ƽ Ʈ���̽�
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Player->GetActorLocation(), Player->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, DebugShape, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			ACS_Enemy* enemy = Cast<ACS_Enemy>(Hit.GetActor());
			if (enemy && enemy->IsAlive())
			{
				// �ߺ��� �� Add���� ����
				DetectedEnemy.AddUnique(enemy);
			}
		}
	}
}

bool UCS_TargetingSystem::IsEnemyInScreen(AActor* Enemy)
{
	// APlayerController ĳ����
	const APlayerController* const PlayerController = Cast<const APlayerController>(Player->GetController());

	// ���� ��ǥ -> ��ũ�� ��ǥ ��ȯ
	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenLocation);

	int32 ScreenX = ScreenLocation.X;
	int32 ScreenY = ScreenLocation.Y;

	// Ÿ���� ��ũ����ǥ���� ��ũ�� ũ��� ���� ���� ū ���� �����ٸ�, ȭ�� �ۿ� �ִ� ������ �����Ѵ�.
	if ((0 < ScreenX && ScreenX < ScreenWidth) && (0 < ScreenY && ScreenY < ScreenHeight))
		return true;
	else
		return false;
}

bool UCS_TargetingSystem::IsWallExist(AActor* Enemy)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// Ʈ���̽� ����, �� ������
	// �÷��̾� ��ġ -> �� ��ġ
	FVector StartPoint = Player->GetActorLocation();
	FVector EndPoint = Enemy->GetActorLocation();

	float distance = FVector::Dist(StartPoint, EndPoint);

	// �÷��̾� -> �� ���� Ʈ���̽�
	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_GameTraceChannel3, TraceParams);
	if (bIsDrawDebug)
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, 3.f);

	if (isHit)
	{
		ACS_Enemy* HitEnemy = Cast<ACS_Enemy>(HitResult.GetActor());
		if (HitEnemy != nullptr && HitEnemy == Enemy)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Success : %s"), *HitEnemy->GetName());
			return true;
		}
	}

	return false;
}

void UCS_TargetingSystem::LockOnTarget(ACS_Enemy* Target)
{
	SetTargetEnemy(Target);
	bIsLockOnTarget = true;
	PlayerAnim->bIsLockOn = true;
	//Player->bUseControllerRotationYaw = true;
	Player->GetCharacterMovement()->bOrientRotationToMovement = false;
	Player->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	DesiredRotator = Player->GetController()->GetControlRotation();

	// �Ϲ� ������ ��쿡�� �����Ѵ�.
	if (GetTargetEnemy()->GetEnemyType() == EEnemyType::ET_Normal ||
		GetTargetEnemy()->GetEnemyType() == EEnemyType::ET_TutoAI)
	{
		// �� ü�¹� �ѱ�
		GetTargetEnemy()->SetWidgetVisible(true);
	}

	// ��� �����ϰ� �����ʴٸ� bIsEquip true
	if (!PlayerAnim->bIsEquip)
	{
		PlayerAnim->bIsEquip = true;
		Player->Weapon->SetBladeOnOff(true);
	}
}

void UCS_TargetingSystem::UnLockTarget()
{
	// ���� ����ִ� ��쿡�� ü�¹� ���߱�, ���� ��쿡�� Lerp�� ���� ���������� �Ѵ�.
	if (TargetEnemy->IsAlive() && GetTargetEnemy()->GetEnemyType() == EEnemyType::ET_Normal)
	{
		// �� ü�¹� ���߱�
		GetTargetEnemy()->SetWidgetVisible(false);
	}

	SetTargetEnemy(nullptr);
	bIsLockOnTarget = false;
	PlayerAnim->bIsLockOn = false;
	//Player->bUseControllerRotationYaw = false;
	Player->GetCharacterMovement()->bOrientRotationToMovement = true;
	Player->GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void UCS_TargetingSystem::RotateToTarget()
{
	// ���� ���°� �ƴ϶�� �������� �ʴ´�
	// ���� �����϶��� Ÿ�ٹ������� ȸ���Ѵ�
	if (!bIsLockOnTarget) return;
	if (!TargetEnemy->IsAlive()) 
	{
		UnLockTarget();
		return;
	}

	// New Rotate
	FRotator TargetRot;
	FRotator ControllerRot;
	if (!bUseSpringArmRotation)
	{
		TargetRot = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), TargetEnemy->GetActorLocation());
		ControllerRot = UKismetMathLibrary::MakeRotator(TargetRot.Roll, TargetRot.Pitch - 15.f, TargetRot.Yaw);
	}
	else
	{
		ControllerRot = UKismetMathLibrary::FindLookAtRotation(Player->SpringArm->GetComponentLocation(), TargetEnemy->GetActorLocation());
	}

	DesiredRotator = FMath::RInterpTo(DesiredRotator, ControllerRot, GetWorld()->GetDeltaSeconds(), 10.f);
	Player->GetController()->SetControlRotation(DesiredRotator);

}

void UCS_TargetingSystem::SetScreenSize(int& width, int& height)
{
	width = GSystemResolution.ResX;
	height = GSystemResolution.ResY;
}

ACS_Enemy* UCS_TargetingSystem::ChangeTargetEnemy()
{
	const APlayerController* const PlayerController = Cast<const APlayerController>(Player->GetController());

	int minDistance = 0;
	ACS_Enemy* minDistanceEnemy = nullptr;

	int ScreenHalfWidth = ScreenWidth / 2;

	for (auto& enemy : EnemyInSight)
	{
		if (GetTargetEnemy() == enemy)
			continue;

		// ���� ��ǥ -> ��ũ�� ��ǥ ��ȯ
		FVector2D ScreenLocation;
		PlayerController->ProjectWorldLocationToScreen(enemy->GetActorLocation(), ScreenLocation);

		int32 ScreenX = ScreenLocation.X;
		int32 ScreenY = ScreenLocation.Y;

		if (0 < ScreenX && (GetTargetSearchType() == ETargetSearchType::TST_WheelUp ? ScreenX < ScreenHalfWidth : ScreenX > ScreenHalfWidth) &&
			(0 < ScreenY && ScreenY < ScreenHeight))
		{
			if (!minDistance || (GetTargetSearchType() == ETargetSearchType::TST_WheelUp ? ScreenX > minDistance : ScreenX < minDistance))
			{
				minDistance = ScreenX;
				minDistanceEnemy = enemy;
			}
		}
	}

	return minDistanceEnemy;
}