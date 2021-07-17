// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_Boss1.h"
#include "CS_Weapon.h"
#include "CS_GhostTrail.h"
#include "CS_Boss1Anim.h"
#include "CS_EnemySpawner.h"
#include "CustomStruct.h"

#include "Engine/DataTable.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Components/WidgetComponent.h"
#include "Components/CanvasPanel.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneComponent.h"

ACS_Boss1::ACS_Boss1()
{
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(GetRootComponent());

	DT_AICombatData = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTable/BossAICombatSetting.BossAICombatSetting'"));
}

void ACS_Boss1::BeginPlay()
{
	Super::BeginPlay();

	// 적 정보 초기화
	MaxHealth = 500.f;
	Health = MaxHealth;
	MaxStamina = 350.f;
	Stamina = MaxStamina;
	MaxGroggyPoint = 15;
	GroggyPoint = 0;
	HealthCover = Health;

	AttackRange = 300.f;
	AttackAngle = 45.f;

	WalkSpeed = 100.f;
	RunSpeed = 400.f;
	SprintSpeed = 500.f;

	// 공격 패턴 개수
	MaxAttackRandomRange = 5;

	// 공격 딜레이 범위 설정
	MaxAttackDelay = 6.f;
	MinAttackDelay = 2.f;

	// 무작위 이동 딜레이 범위 설정
	MaxRandomMoveDelay = 4.f;
	MinRandomMoveDelay = 2.f;

	AttachWeapon();
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	SetEnemyType(EEnemyType::ET_Boss);

	bShowHealthBar = false;

	Boss1Anim = Cast<UCS_Boss1Anim>(EnemyAnim);

	
	if(DT_AICombatData == nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("DT_AICombatData == nullptr")));
	else
	{
		RowNames = DT_AICombatData->GetRowNames();

		for (int i = 0; i < RowNames.Num(); i++)
		{
			FAICombatStruct AICombatData = *(DT_AICombatData->FindRow<FAICombatStruct>(RowNames[i], RowNames[i].ToString()));
			//UE_LOG(LogTemp, Warning, TEXT("%d %f %f"), i, AICombatData.AttackRange, AICombatData.AttackAngle);

			AICombatDataAry.Add(AICombatData);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s %f %f"), *RowNames[i].ToString(), AICombatData.AttackRange, AICombatData.AttackAngle));
		}
	}
	
}

void ACS_Boss1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	UpdateMaxWalkSpeed();

	if (bShowHealthBar)
	{
		VisibleCanvasPanel(DeltaTime);
	}
}

void ACS_Boss1::AttachWeapon()
{
	Weapon = GetWorld()->SpawnActor<ACS_Weapon>(WeaponClass);

	// 무기 Attach
	Weapon->SetOwner(this);
	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("Socket_twinblade");
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(Weapon, GetMesh());
	}
	Weapon->SetActorLocation(GetMesh()->GetSocketLocation("Socket_twinblade"));
	Weapon->SetActorRotation(GetMesh()->GetSocketRotation("Socket_twinblade"));
}

void ACS_Boss1::CombatBehaviour()
{
	if (!IsAttackDelayZero()) return;

	// CombatActions 배열이 비어있는 경우에만 실행한다.
	// CombatActions 배열에 공격 패턴을 Add한다.
	if (IsEmptyCombatActions())
	{
		int32 randnum = GenerateCombatRandNum();

		switch (randnum)
		{
		case 1:
			CombatActions.Add(EnemyAnim->Attack1Montage);
			SetAttackArea(AICombatDataAry[0].AttackRange, AICombatDataAry[0].AttackAngle);

			break;
		case 2:
			CombatActions.Add(EnemyAnim->Attack2Montage);
			SetAttackArea(AICombatDataAry[1].AttackRange, AICombatDataAry[1].AttackAngle);

			break;
		case 3:
			CombatActions.Add(EnemyAnim->Attack3Montage);
			SetAttackArea(AICombatDataAry[2].AttackRange, AICombatDataAry[2].AttackAngle);

			break;
		case 4:
			CombatActions.Add(EnemyAnim->Attack4Montage);
			SetAttackArea(AICombatDataAry[3].AttackRange, AICombatDataAry[3].AttackAngle);

			break;
		case 5:
			CombatActions.Add(EnemyAnim->Attack5Montage);
			SetAttackArea(AICombatDataAry[4].AttackRange, AICombatDataAry[4].AttackAngle);

			break;
		default:
			break;
		}
	}
}

void ACS_Boss1::Attack()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

	// bOnAttack이 false일때 실행한다.
	// AttackDelay가 0이하일때 실행한다.
	if (!bOnAttack && !bOnGroggy)
	{
		bOnAttack = true;
		bOnHit = false;
		bIsMoveToTarget = false;
		bIsStopMovement = false;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Enemy Attacking"));

		// 이동 멈추기
		AIController->StopMovement();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bIsRun = false;

		// CombatAction 배열의 끝부분부터 몽타주 애니메이션을 꺼내와서 실행한다.
		float CountNum = CombatActions.Num();
		currentMontage = CombatActions[CountNum - 1];
		CombatActions.RemoveAt(CountNum - 1);

		// 공격 패턴을 다 실행하였다면, 공격 딜레이를 지정한다.
		if (IsEmptyCombatActions())
			SetAttackDelay();

		// 몽타주 실행
		EnemyAnim->PlayMontage(currentMontage);
		// 몽타주 Section 번호를 첫번째 섹션으로 지정한다.
		EnemyAnim->SectionNumber = 1;
	}
}

void ACS_Boss1::Die()
{
	Super::Die();
}

void ACS_Boss1::SpawnGhostTrail(float SpawnDelay)
{
	// 타이머 실행중이라면 실행하지 않는다.
	if (GetWorldTimerManager().IsTimerActive(SpawnGhostTrailTimerHandle)) return;

	// 스폰 딜레이를 지정한다.
	SpawnGhostTrailTime = SpawnDelay;
	GetWorldTimerManager().SetTimer(SpawnGhostTrailTimerHandle, this, &ACS_Boss1::SpawnGhostTrailTimer, GetWorld()->GetDeltaSeconds(), true);

	// 고스트 트레일 액터를 스폰한다.
	ACS_GhostTrail* GhostTrail = GetWorld()->SpawnActor<ACS_GhostTrail>(GhostTrailClass, GetMesh()->GetComponentTransform());
	// 현재 Boss1의 메쉬를 포즈로 지정한다.
	GhostTrail->SetPose(GetMesh());
}

void ACS_Boss1::SpawnGhostTrailTimer()
{
	SpawnGhostTrailTime -= GetWorld()->GetDeltaSeconds();
	if (SpawnGhostTrailTime <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(SpawnGhostTrailTimerHandle);
	}
}

void ACS_Boss1::Executed(AActor* Causer, FVector Location)
{
	// 모든 몽타주 중지
	EnemyAnim->StopAllMontages(0.25f);
	// 공격자에 맞춘 포지션으로 위치한다
	SetActorLocation(Location);

	// 공격자에 맞춘 로테이션으로 위치한다
	FRotator RotateToPlayer = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation());
	SetActorRotation(RotateToPlayer);

	// 처형 몽타주를 실행한다
	EnemyAnim->PlayMontage(EnemyAnim->ExecutedMontage);
}

void ACS_Boss1::ExecutedTakeDamage(float DamageAmount)
{
	DecrementHealth(DamageAmount);
	ChangeMatTimerActive();
}

void ACS_Boss1::ExecutedEnd() 
{
	if (Health <= 0.f)
	{
		bIsExecuted = true;
		EnemyAnim->SetAnimSpeed(0.f);
		Die();
	}
	else
	{
		GroggyPoint = 0;
		EnemyAnim->SectionNumber = 1;
		EnemyAnim->NextSection();
		ReSetAttack();
		bIsMoveToTarget = false;
		bIsStopMovement = false;
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	}
}

void ACS_Boss1::Groggy(AActor* Causer)
{
	// 그로기 상태로 변경한다
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Groggy);
	// AI 상태변수 설정
	bOnGroggy = true;
	bIsCanExecute = true;

	// 공격자에 맞춘 로테이션으로 위치한다
	FRotator RotateToPlayer = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation());
	SetActorRotation(RotateToPlayer);
	// 그로기 몽타주 실행
	EnemyAnim->PlayMontage(EnemyAnim->GroggyMontage);
}

bool ACS_Boss1::IsExecuteReady()
{
	return bIsCanExecute;
}

void ACS_Boss1::CastSpell()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_CastSpell);

	// 캐스팅 몽타주 실행
	EnemyAnim->PlayMontage(Boss1Anim->CastMontage);
}