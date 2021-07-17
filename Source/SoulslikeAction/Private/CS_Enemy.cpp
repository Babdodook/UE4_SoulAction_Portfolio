// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_Enemy.h"
#include "EnumState.h"
#include "CS_Player.h"
#include "CS_Weapon.h"
#include "CS_TargetingSystem.h"
#include "CS_TargetPointComponent.h"
#include "LJSMathHelpers.h"

#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetComponent.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"



// Sets default values
ACS_Enemy::ACS_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 어그로용
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	TargetPointComponent = CreateDefaultSubobject<UCS_TargetPointComponent>(TEXT("TargetPointComponent"));
	TargetPointComponent->SetupAttachment(RootComponent);

	bOnAttack = false;
	CombatTarget = nullptr;
	bOnHit = false;
	bIsMoveToTarget = false;
	bOnDeathEnd = false;
	WidgetDisplayTime = 3.f;

	AIStopDistance = 25.f;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ACS_Enemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());
	EnemyAnim = Cast<UCS_EnemyAnim>(GetMesh()->GetAnimInstance());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ACS_Enemy::AgroSphere_OnOverlapBegin);

	AttackDelay = 0;
	v = 0;
	h = 0;

	PrevCombatRandNum = 0;

	bOnHitReaction = true;

	Mat_Origin = GetMesh()->GetMaterial(0)->GetMaterial();
}

// Called every frame
void ACS_Enemy::Tick(float DeltaTime)
{
	HideCanvasPanel(DeltaTime);
	UpdateStats(DeltaTime);

	if (!IsAlive()) return;

	Super::Tick(DeltaTime);
	
	// 공격 가능한 타겟 존재할 시에만 실행한다.
	if (CombatTarget == nullptr) return;

	UpdateDelay(DeltaTime);
	CombatMovement();

	CombatBehaviour();
	// 공격 사정거리와 각도 모두 만족할 경우에 공격이 가능하도록 한다.
	// 공격 딜레이가 0이하 일때 실행한다.
	if (IsTargetExistInAttackArea() && IsAttackDelayZero())
	{
		// 공격을 실행한다.
		Attack();
	}
}

// Called to bind functionality to input
void ACS_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Dead) return;
}

// 어그로용 Sphere 오버랩 시작
void ACS_Enemy::AgroSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 타겟 있으면 실행안함
	if (CombatTarget != nullptr) return;

	if (OtherActor)
	{
		if (GetEnemyType() == EEnemyType::ET_TutoAI)
		{
			ACS_Player* Player = Cast<ACS_Player>(OtherActor);
			//CanvasPanel = Player->TutorialTextCanvasPanel;
			CombatTarget = Player;
			return;
		}

		ActiveAIBehaviour(OtherActor);
	}
}

void ACS_Enemy::ActiveAIBehaviour(AActor* Target)
{
	ACS_Player* Player = Cast<ACS_Player>(Target);
	if (Player && !bIsDummy)
	{
		CombatTarget = Player;
		EnemyAnim->SetTargetExist(true);
		MoveToTarget(CombatTarget);

		if (GetEnemyType() == EEnemyType::ET_Boss)
		{
			Player->BossEnemy = this;
			CanvasPanel = Player->BossCanvasPanel;

			bShowHealthBar = true;
		}

		Weapon->SetBladeOnOff(true);
	}
}

void ACS_Enemy::UpdateDelay(float DeltaTime)
{
	if (!IsAttackDelayZero())
	{
		AttackDelay -= DeltaTime;
	}

	if (!IsRandomMoveDelayZero())
	{
		RandomMoveDelay -= DeltaTime;
	}
}

// 타겟으로 이동
void ACS_Enemy::MoveToTarget(class ACS_Player* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		// 달리기 랜덤 설정하기
		SetIsRunRandom();

		bIsMoveToTarget = true;
		bIsStopMovement = false;
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);			// 타겟 지정
		MoveRequest.SetAcceptanceRadius(AIStopDistance);		// 캐릭터 콜라이더와의 거리, 멈추는 거리

		FNavPathSharedPtr NavPath;					// 경로 정보 받아올거, out
		
		AIController->MoveTo(MoveRequest, &NavPath);
	}
}

void ACS_Enemy::CombatMovement()
{
	if (EnemyMovementStatus != EEnemyMovementStatus::EMS_MoveToTarget) return;

	if (!IsAttackDelayZero())
	{
		if (bIsMoveToTarget)
		{
			bIsMoveToTarget = false;
		}

		RotateToTarget(-1.f);

		// 무작위 이동 딜레이가 0이하일 경우
		if (IsRandomMoveDelayZero())
		{
			// 무작위 이동 딜레이 값을 다시 할당한다.
			SetRandomMoveDelay();
			// 새로운 무작위 방향을 지정한다.
			SetRandomMoveDirection();
		}
		
		SetMoveDirectionLerp(MoveDirectionStatus);
		// 지정된 방향으로 이동한다.
		SetVelocityToDirection(MoveDirection, WalkSpeed);
	}
	else
	{
		// 공격 딜레이가 0이하로, 공격 가능하다면 타겟으로 곧장 이동한다.
		if (!bIsMoveToTarget)
		{
			MoveToTarget(CombatTarget);
		}

		FVector Speed = GetCharacterMovement()->Velocity;
		FVector LSpeed = FVector(Speed.X, Speed.Y, 0.f);

		// 현재 이동 속도가 WalkSpeed보다 느릴때 -> 멈춘거나 다름없다고 간주한다. 타겟으로 회전하도록 한다.
		if (LSpeed.Size() < WalkSpeed)
		{
			RotateToTarget(-1.f);
		}
	}
	
}

void ACS_Enemy::Attack()
{
}

float ACS_Enemy::GetTargetDistance()
{
	// 공격 가능한 타겟이 있을 경우
	if (CombatTarget != nullptr)
	{
		m_targetDistance = GetDistanceTo(CombatTarget);
	}

	return m_targetDistance;
}

void ACS_Enemy::DecrementHealth(float Amount)
{
	Health -= Amount;
}

void ACS_Enemy::IncrementGroggyPoint(float Amount)
{
	GroggyPoint = GroggyPoint + Amount > MaxGroggyPoint ? MaxGroggyPoint : GroggyPoint + Amount;
}

float ACS_Enemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bOnHit || bOnGroggy || 
		EnemyMovementStatus == EEnemyMovementStatus::EMS_Dead) return DamageAmount-1;
	DisplayWidgetWhenTakeDamage(DamageCauser);	

	DecrementHealth(DamageAmount);				// 데미지 적용
	bIsMoveToTarget = false;
	bIsStopMovement = false;
	ChangeMatTimerActive();			// 머터리얼 교체 타이머 작동

	if(EnemyMovementStatus == EEnemyMovementStatus::EMS_CastSpell) return DamageAmount;

	// 그로기 포인트 증가
	IncrementGroggyPoint(1);
	// 타입이 보스고, 그로기 포인트가 최대치다. 그로기 실행
	if (GetEnemyType() == EEnemyType::ET_Boss && GroggyPoint == MaxGroggyPoint)
	{
		Groggy(DamageCauser);
		return DamageAmount;
	}
	
	if (Health <= 0.f)
	{
		Die();

		return DamageAmount;
	}

	bOnHit = true;
	// bOnHitReaction가 false라면 피격 몽타주를 실행하지 않는다
	if (!bOnHitReaction)
	{
		ReSetOnHitTime = 0.3f;
		GetWorldTimerManager().SetTimer(ReSetOnHitTimerHandle, this, &ACS_Enemy::ReSetOnHitTimer, 0.1f, true);
		return DamageAmount;
	}
	ReSetAttack();								// 공격 재설정하기
	SetHitMontage();							// 피격 방향에 따라 애니메이션 세팅
	EnemyAnim->PlayMontage(currentMontage);		// 피격 애니메이션 실행

	return DamageAmount;
}

void ACS_Enemy::RotateToTarget(float RotateSpeed)
{
	// 타겟이 존재하지 않으면 실행하지 않는다.
	if (CombatTarget == nullptr || bOnExecution) return;
	if (RotateSpeed == -1.f)
		RotateSpeed = 5.f;

	FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
	FRotator DesiredRotator = FMath::RInterpTo(GetActorRotation(), TargetRotator, GetWorld()->GetDeltaSeconds(), RotateSpeed);
	DesiredRotator = FRotator(0.f, DesiredRotator.Yaw, 0.f);
	SetActorRotation(DesiredRotator);

}

bool ACS_Enemy::IsTargetExistInSight()
{
	// 지정된 타겟이 없다면 false를 반환한다.
	if (CombatTarget == nullptr) return false;

	// 시야 거리와 시야각 모두 만족할 경우에 true를 반환한다. 그렇지 않은 경우 false를 반환한다.
	if (GetTargetDistance() <= SightRange && ULJSMathHelpers::GetAngleToTarget(this, CombatTarget) <= SightAngle) return true;
	else return false;
}

bool ACS_Enemy::IsTargetExistInAttackArea()
{
	if (CombatTarget == nullptr) return false;

	// 공격 거리와 공격각도 모두 만족할 경우에 true를 반환한다. 그렇지 않은 경우 false를 반환한다.
	if (GetTargetDistance() <= AttackRange && ULJSMathHelpers::GetAngleToTarget(this, CombatTarget) <= AttackAngle) return true;
	else return false;
}

void ACS_Enemy::SetVelocityToDirection(FVector Direction, float Value)
{
	FVector DesiredMoveVector = Direction * Value;
	GetCharacterMovement()->Velocity = DesiredMoveVector;
}

bool ACS_Enemy::IsEmptyCombatActions()
{
	if (CombatActions.Num() == 0) return true;
	else return false;
}

void ACS_Enemy::SetAttackDelay()
{
	AttackDelay = FMath::RandRange(MinAttackDelay, MaxAttackDelay);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("AttackDelay: %f"), AttackDelay));
}

bool ACS_Enemy::IsAttackDelayZero()
{
	if (AttackDelay <= 0) return true;
	else return false;
}

void ACS_Enemy::SetRandomMoveDelay()
{
	RandomMoveDelay = FMath::RandRange(MinRandomMoveDelay, MaxRandomMoveDelay);
}

bool ACS_Enemy::IsRandomMoveDelayZero()
{
	if (RandomMoveDelay <= 0) return true;
	else return false;
}

void ACS_Enemy::SetHitMontage()
{
	float HitAngle = ULJSMathHelpers::GetAngleToTargetCross(this, CombatTarget);

	// Front
	if (-45.f <= HitAngle && HitAngle <= 45.f)
	{
		currentMontage = EnemyAnim->HitFrontMontage;
	}
	// Left
	else if (45.f <= HitAngle && HitAngle <= 135.f)
	{
		currentMontage = EnemyAnim->HitLeftMontage;
	}
	// Right
	else if (-135.f <= HitAngle && HitAngle <= -45.f)
	{
		currentMontage = EnemyAnim->HitRightMontage;
	}
	// Back
	else if ((-180.f <= HitAngle && HitAngle <= -135.f) ||
		(135.f <= HitAngle && HitAngle <= 180.f))
	{
		currentMontage = EnemyAnim->HitBackMontage;
	}
}

void ACS_Enemy::ReSetAttack()
{
	CombatActions.Empty();		// 공격 배열 비우기
	bOnAttack = false;			// OnAttack False로 변경
	//SetAttackDelay();			// 공격 딜레이 다시 설정하기
}

void ACS_Enemy::SetMoveDirection(EMoveDirectionStatus Status)
{
	switch (Status)
	{
	case EMoveDirectionStatus::MDS_Forward:
		v = 1;
		h = 0;
		break;
	case EMoveDirectionStatus::MDS_Left:
		v = 0;
		h = -1;
		break;
	case EMoveDirectionStatus::MDS_Right:
		v = 0;
		h = 1;
		break;
	case EMoveDirectionStatus::MDS_Backward:
		v = -1;
		h = 0;
		break;
	case EMoveDirectionStatus::MDS_MAX:
		break;
	default:
		return;
	}

	MoveDirection = GetActorForwardVector() * v + GetActorRightVector() * h;
}

void ACS_Enemy::SetMoveDirectionLerp(EMoveDirectionStatus Status)
{
	switch (Status)
	{
	case EMoveDirectionStatus::MDS_Forward:
		v = FMath::Lerp<float, float>(v, 1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, 0.f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_ForwardLeft:
		v = FMath::Lerp<float, float>(v, 1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, -1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_ForwardRight:
		v = FMath::Lerp<float, float>(v, 1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, 1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_Left:
		v = FMath::Lerp<float, float>(v, 0.f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, -1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_Right:
		v = FMath::Lerp<float, float>(v, 0.f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, 1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_Backward:
		v = FMath::Lerp<float, float>(v, -1.1f, 10.f * GetWorld()->GetDeltaSeconds());
		h = FMath::Lerp<float, float>(h, 0.f, 10.f * GetWorld()->GetDeltaSeconds());
		break;
	case EMoveDirectionStatus::MDS_MAX:
		break;
	default:
		return;
	}

	MoveDirection = GetActorForwardVector() * v + GetActorRightVector() * h;
	MoveDirection.Normalize();
}

void ACS_Enemy::SetRandomMoveDirection()
{
	int randnum = FMath::RandRange(1, 6);

	switch (randnum)
	{
	case 1:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_Forward;
		break;
	case 2:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_ForwardLeft;
		break;
	case 3:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_ForwardRight;
		break;
	case 4:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_Left;
		break;
	case 5:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_Right;
		break;
	case 6:
		MoveDirectionStatus = EMoveDirectionStatus::MDS_Backward;
		break;
	}
}

void ACS_Enemy::SetIsRunRandom()
{
	int randnum = FMath::RandRange(0, 1);
	
	if (randnum == 0) bIsRun = true;
	else bIsRun = false;
}

void ACS_Enemy::UpdateMaxWalkSpeed()
{
	if (CombatTarget == nullptr) return;

	if (bIsRun && GetCharacterMovement()->MaxWalkSpeed <= RunSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp<float, float>(GetCharacterMovement()->MaxWalkSpeed, RunSpeed, 5 * GetWorld()->GetDeltaSeconds());
	}
	else if (!bIsRun && GetCharacterMovement()->MaxWalkSpeed >= WalkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp<float, float>(GetCharacterMovement()->MaxWalkSpeed, WalkSpeed, 5 * GetWorld()->GetDeltaSeconds());
	}
}

void ACS_Enemy::Die()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	Weapon->SetBladeOnOff(false);
	Weapon->SetVisibility(true);
	// 래그돌 적용
	Ragdoll();

	// 일반 적인 타격으로 인한 죽음
	if (!bIsExecuted)
	{
		// 임펄스
		FVector ImpurseVec = CombatTarget->GetActorForwardVector() * 700.f * GetMesh()->GetMass();
		FName BoneName = GetMesh()->GetBoneName(1);
		GetMesh()->AddImpulse(ImpurseVec, BoneName);
	}
}

void ACS_Enemy::Ragdoll()
{
	EnemyAnim->StopAllMontages(0.1f);
	bOnDeathEnd = true;

	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool ACS_Enemy::IsAlive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void ACS_Enemy::DeathEnd()
{
	EnemyAnim->StopAllMontages(0.1f);
	bOnDeathEnd = true;
	
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//UPrimitiveComponent::AddImpulse(CombatTarget->GetActorForwardVector() * 500.f, )
	FVector ImpurseVec = CombatTarget->GetActorForwardVector() * 700.f * GetMesh()->GetMass();
	FName BoneName = GetMesh()->GetBoneName(1);
	GetMesh()->AddImpulse(ImpurseVec, BoneName);
	//GetMesh()->AddImpulseAtLocation(ImpurseVec, GetActorLocation(), BoneName);
}

void ACS_Enemy::SetWidgetVisible(bool Value)
{
	if(Value) WidgetClass->SetVisibility(ESlateVisibility::Visible);
	else WidgetClass->SetVisibility(ESlateVisibility::Hidden);
}

void ACS_Enemy::UpdateStats(float DeltaTime)
{
	if (Health < HealthCover)
	{
		HealthCover = FMath::Lerp<float, float>(HealthCover, Health, 2.5f * DeltaTime);
	}
}

void ACS_Enemy::HideCanvasPanel(float DeltaTime)
{
	if (IsAlive()) return;

	if (CanvasPanel->GetRenderOpacity() > 0.f)
	{
		CanvasPanel->SetRenderOpacity(FMath::Lerp<float, float>(CanvasPanel->GetRenderOpacity(), 0.f, 2.f * DeltaTime));
	}
}

void ACS_Enemy::VisibleCanvasPanel(float DeltaTime)
{
	if (CanvasPanel->GetRenderOpacity() < 1.f)
	{
		CanvasPanel->SetRenderOpacity(FMath::Lerp<float, float>(CanvasPanel->GetRenderOpacity(), 1.1f, 2.f * DeltaTime));
		if (CanvasPanel->GetRenderOpacity() >= 1.f)
			bShowHealthBar = false;
	}
}

void ACS_Enemy::WidgetDisplayTimer()
{
	if (!IsAlive() || this == DamageCausers->TargetSystemComponent->GetTargetEnemy())
	{
		GetWorldTimerManager().ClearTimer(WidgetDisplayTimerHandle);
		return;
	}

	--WidgetDisplayTime;
	if (WidgetDisplayTime <= 0.f)
	{
		SetWidgetVisible(false);
		GetWorldTimerManager().ClearTimer(WidgetDisplayTimerHandle);
	}
}

void ACS_Enemy::DisplayWidgetWhenTakeDamage(AActor* DamageCauser)
{
	// 몬스터 타입이 보스일 경우 실행하지 않는다.
	if (GetEnemyType() == EEnemyType::ET_Boss) return;

	ACS_Player* player = Cast<ACS_Player>(DamageCauser);
	DamageCausers = player;

	// 타겟팅된 적이 이 액터라면 위젯을 표시하지 않는다.
	if (this == player->TargetSystemComponent->GetTargetEnemy())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Yes That is Me"));
		return;
	}
	// 타겟팅되지 않았다면 위젯 디스플레이 타이머를 실행한다.
	else
	{
		// 타이머 작동 중이라면 핸들을 clear하고 다시 작동한다.
		if (GetWorldTimerManager().IsTimerActive(WidgetDisplayTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(WidgetDisplayTimerHandle);
		}

		SetWidgetVisible(true);
		WidgetDisplayTime = 3.f;
		GetWorldTimerManager().SetTimer(WidgetDisplayTimerHandle, this, &ACS_Enemy::WidgetDisplayTimer, 1.0f, true);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Not Me"));
	}
}

void ACS_Enemy::SetAttackArea(float Range, float Angle)
{
	AttackRange = Range;
	AttackAngle = Angle;
}

int ACS_Enemy::GenerateCombatRandNum()
{
	while (1)
	{
		int32 randnum = FMath::RandRange(1, MaxAttackRandomRange);

		if (randnum == PrevCombatRandNum)
			continue;
		else
		{
			PrevCombatRandNum = randnum;
			return randnum;
		}
	}
}

void ACS_Enemy::ReSetOnHitTimer()
{
	ReSetOnHitTime -= 0.1f;
	if (ReSetOnHitTime <= 0.f)
	{
		bOnHit = false;
		GetWorldTimerManager().ClearTimer(ReSetOnHitTimerHandle);
	}
}

void ACS_Enemy::SetMeshMaterial(UMaterial* Mat)
{
	GetMesh()->SetMaterial(0, Mat);
}

void ACS_Enemy::ChangeMatTimer()
{
	ChangeMatTime -= 0.1f;

	if (ChangeMatTime <= 0.f)
	{
		GetMesh()->SetMaterial(0, Mat_Origin);
		GetWorldTimerManager().ClearTimer(ChangeMatTimerHandle);
	}
}

void ACS_Enemy::ChangeMatTimerActive()
{
	if (!GetWorldTimerManager().IsTimerActive(ChangeMatTimerHandle))
	{
		ChangeMatTime = 0.2f;
		GetMesh()->SetMaterial(0, Mat_GridInst);
		GetWorldTimerManager().SetTimer(ChangeMatTimerHandle, this, &ACS_Enemy::ChangeMatTimer, 0.1f, true);
	}
}