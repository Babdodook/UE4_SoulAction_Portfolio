// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_Player.h"
#include "CS_Mon1.h"
#include "CS_Weapon.h"
#include "LJSMathHelpers.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACS_Mon1::ACS_Mon1()
{
	AIHealthBarComponenet = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIHealthBarComponenet"));
	AIHealthBarComponenet->SetupAttachment(RootComponent);

	// 적 정보 초기화
	MaxHealth = 100.f;
	Health = MaxHealth;
	MaxStamina = 350.f;
	Stamina = MaxStamina;
	MaxGroggyPoint = 10;
	GroggyPoint = MaxGroggyPoint;
	HealthCover = Health;
}

// Called when the game starts or when spawned
void ACS_Mon1::BeginPlay()
{
	Super::BeginPlay();

	SightRange = 500.f;
	SightAngle = 60.f;

	AttackRange = 300.f;
	AttackAngle = 45.f;

	WalkSpeed = 100.f;
	RunSpeed = 400.f;
	SprintSpeed = 500.f;

	// 공격 패턴 개수
	MaxAttackRandomRange = 2;

	// 공격 딜레이 범위 설정
	MaxAttackDelay = 8.f;
	MinAttackDelay = 5.f;

	// 무작위 이동 딜레이 범위 설정
	MaxRandomMoveDelay = 4.f;
	MinRandomMoveDelay = 2.f;

	AttachWeapon();
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// 위젯 클래스 가져오기
	WidgetClass = Cast<UUserWidget>(AIHealthBarComponenet->GetUserWidgetObject());
	WidgetClass->SetVisibility(ESlateVisibility::Hidden);

	SetEnemyType(EEnemyType::ET_Normal);
}

// Called every frame
void ACS_Mon1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) return;

	UpdateMaxWalkSpeed();
}

void ACS_Mon1::AttachWeapon()
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

	//UE_LOG(LogTemp, Warning, TEXT("Run AttachWeapon()"));
}

void ACS_Mon1::CombatBehaviour()
{
	if (!IsAttackDelayZero()) return;

	// CombatActions 배열이 비어있는 경우에만 실행한다.
	// CombatActions 배열에 공격 패턴을 Add한다.
	if (IsEmptyCombatActions())
	{
		int32 randnum = GenerateCombatRandNum();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("CombatBehaviour :: Now Action is %d"), randnum));

		switch (randnum)
		{
		case 1:
			CombatActions.Add(EnemyAnim->Attack1Montage);
			break;
		case 2:
			CombatActions.Add(EnemyAnim->Attack1Montage);
			CombatActions.Add(EnemyAnim->Attack2Montage);
			break;
		default:
			break;
		}
	}
}

void ACS_Mon1::Attack()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

	// bOnAttack이 false일때 실행한다.
	// AttackDelay가 0이하일때 실행한다.
	if (!bOnAttack)
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

void ACS_Mon1::Die()
{
	Super::Die();

	/*const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("Socket_twinblade");
	if (WeaponSocket)
	{
		Weapon->DetachFromActor
		WeaponSocket->AttachActor(Weapon, GetMesh());
	}*/
}

void ACS_Mon1::Executed(AActor* Causer, FVector Location)
{
	bOnExecution = true;
	// 모든 몽타주 중지
	EnemyAnim->StopAllMontages(0.25f);
	// 공격자에 맞춘 포지션으로 위치한다
	SetActorLocation(Location);

	// 공격자에 맞춘 로테이션으로 위치한다
	FRotator RotateToPlayer = Causer->GetActorRotation();
	SetActorRotation(RotateToPlayer);

	// 처형 몽타주를 실행한다
	EnemyAnim->PlayMontage(EnemyAnim->ExecutedMontage);
}

void ACS_Mon1::ExecutedTakeDamage(float DamageAmount)
{
	DecrementHealth(DamageAmount);
	ChangeMatTimerActive();
}

void ACS_Mon1::ExecutedEnd()
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
		//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	}

	bOnExecution = false;
}

bool ACS_Mon1::IsExecuteReady()
{
	float TargetAngle = ULJSMathHelpers::GetAngleToTargetCross(this, CombatTarget);

	if ((-180.f <= TargetAngle && TargetAngle <= -135.f) ||
		(135.f <= TargetAngle && TargetAngle <= 180.f))
	{
		return true;
	}
	else
		return false;
}