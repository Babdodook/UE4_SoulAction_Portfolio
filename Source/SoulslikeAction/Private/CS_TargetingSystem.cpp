// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_TargetingSystem.h"
#include "CS_Player.h"
#include "CS_PlayerAnim.h"
#include "CS_Enemy.h"
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
}

void UCS_TargetingSystem::SearchTarget()
{
	// 이미 타겟지정된 적 존재
	if (GetTargetEnemy() != nullptr)
	{
		// 타겟 해제
		UnLockTarget();
		return;
	}

	// 적 감지
	DetectEnemyObjects();

	// 감지된 적 없음. return
	if (DetectedEnemy.Num() == 0) return;

	// 스크린에 있는지 확인
	for (auto& enemy : DetectedEnemy)
	{
		if (IsEnemyInScreen(enemy))
		{
			EnemyInScreen.Add(enemy);
			//UE_LOG(LogTemp, Warning, TEXT("%s is in Screen"), *enemy->GetName());
		}
	}

	// 스크린에 존재하는 적 없음. return
	if (EnemyInScreen.Num() == 0) return;

	// 스크린에 존재하는 적과 플레이어 사이에 벽이 있는지 확인
	for (auto& enemy : DetectedEnemy)
	{
		if (IsWallExist(enemy))
		{
			EnemyInSight.Add(enemy);
			//UE_LOG(LogTemp, Warning, TEXT("%s is in Screen"), *enemy->GetName());
		}
	}

	// 플레이어 시야에 존재하는 적 없음. return
	if (EnemyInSight.Num() == 0) return;

	// 플레이어와 가장 가까운 적을 찾아냄
	ACS_Enemy* minDistanceEnemy = EnemyInSight[0];
	for (auto& enemy : EnemyInSight)
	{
		if (FVector::Dist(Player->GetActorLocation(), minDistanceEnemy->GetActorLocation()) >= FVector::Dist(Player->GetActorLocation(), enemy->GetActorLocation()))
		{
			minDistanceEnemy = enemy;
		}
	}

	// 가장 가까운 적 타겟으로 지정
	LockOnTarget(minDistanceEnemy);
	//UE_LOG(LogTemp, Warning, TEXT("Now %s is Target"), *TargetEnemy->GetName());
}

void UCS_TargetingSystem::DetectEnemyObjects()
{
	// 모든 배열 초기화
	DetectedEnemy.Empty();
	EnemyInScreen.Empty();
	EnemyInSight.Empty();

	TArray<FHitResult> OutHits;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// 충돌 확인용 그리기
	FCollisionShape DebugShape = FCollisionShape::MakeSphere(600.f);
	if(bIsDrawDebug)
		DrawDebugSphere(GetWorld(), Player->GetActorLocation(), DebugShape.GetSphereRadius(), 10, FColor::Blue, false, 1.5f);

	// 스윕 멀티 트레이스
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Player->GetActorLocation(), Player->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, DebugShape, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			ACS_Enemy* enemy = Cast<ACS_Enemy>(Hit.GetActor());
			if (enemy && enemy->IsAlive())
			{
				// 중복된 것 Add하지 않음
				DetectedEnemy.AddUnique(enemy);
			}
		}
	}
}

bool UCS_TargetingSystem::IsEnemyInScreen(AActor* Enemy)
{
	// APlayerController 캐스팅
	const APlayerController* const PlayerController = Cast<const APlayerController>(Player->GetController());

	// 월드 좌표 -> 스크린 좌표 변환
	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenLocation);

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;
	PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

	int32 ScreenX = ScreenLocation.X;
	int32 ScreenY = ScreenLocation.Y;

	if ((0 < ScreenX && ScreenX < ScreenWidth) && (0 < ScreenY && ScreenY < ScreenHeight))
		return true;
	else
		return false;
}

bool UCS_TargetingSystem::IsWallExist(AActor* Enemy)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// 트레이스 시작, 끝 포지션
	// 플레이어 위치 -> 적 위치
	FVector StartPoint = Player->GetActorLocation();
	FVector EndPoint = Enemy->GetActorLocation();

	float distance = FVector::Dist(StartPoint, EndPoint);

	// 플레이어 -> 적 라인 트레이스
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

	// 일반 몬스터일 경우에만 실행한다.
	if (GetTargetEnemy()->GetEnemyType() == EEnemyType::ET_Normal)
	{
		// 적 체력바 켜기
		GetTargetEnemy()->SetWidgetVisible(true);
	}

	// 장비 착용하고 있지않다면 bIsEquip true
	if (!PlayerAnim->bIsEquip)
		PlayerAnim->bIsEquip = true;
}

void UCS_TargetingSystem::UnLockTarget()
{
	// 적이 살아있는 경우에는 체력바 감추기, 죽은 경우에는 Lerp를 통해 없어지도록 한다.
	if (TargetEnemy->IsAlive() && GetTargetEnemy()->GetEnemyType() == EEnemyType::ET_Normal)
	{
		// 적 체력바 감추기
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
	// 락온 상태가 아니라면 실행하지 않는다
	// 락온 상태일때만 타겟방향으로 회전한다
	if (!bIsLockOnTarget) return;
	if (!TargetEnemy->IsAlive()) 
	{
		UnLockTarget();
		return;
	}

	//FRotator playerRotator = Player->GetActorRotation();
	//FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), TargetEnemy->GetActorLocation());
	//DesiredRotator = FMath::RInterpTo(playerRotator, FRotator{ playerRotator.Pitch, LookAtRotator.Yaw, playerRotator.Roll }, GetWorld()->GetDeltaSeconds(), 10.f);

	

	//FRotator ContorllerRotator = Player->GetController()->GetControlRotation();
	FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(Player->SpringArm->GetComponentLocation(), TargetEnemy->GetActorLocation());
	//FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(Player->FollowCamera->GetComponentLocation() , TargetEnemy->GetActorLocation());
	//FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), TargetEnemy->GetActorLocation());


	DesiredRotator = FMath::RInterpTo(DesiredRotator, TargetRotator, GetWorld()->GetDeltaSeconds(), 10.f);
	//Player->GetInstigator()->GetController()->SetControlRotation(DesiredRotator);
	Player->GetController()->SetControlRotation(DesiredRotator);

	/*const FRotator LookedAtCameraRotation = FMath::RInterpConstantTo(Player->GetControlRotation(),
		LookAtRotator + FRotator{ -20.f, 0.f, 0.f },
		GetWorld()->GetDeltaSeconds(),
		300.f);

	Player->GetInstigator()->GetController()->SetControlRotation(LookedAtCameraRotation);


	const FRotator PlayerRotation = Player->GetActorRotation();

	Player->SetActorRotation(FMath::RInterpConstantTo(PlayerRotation,
		FRotator{
			PlayerRotation.Pitch,
			LookAtRotator.Yaw,
			PlayerRotation.Roll
		},
		GetWorld()->GetDeltaSeconds(),
		300.f));*/

}