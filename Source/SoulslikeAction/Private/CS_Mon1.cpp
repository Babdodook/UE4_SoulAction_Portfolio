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

	// �� ���� �ʱ�ȭ
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

	// ���� ���� ����
	MaxAttackRandomRange = 2;

	// ���� ������ ���� ����
	MaxAttackDelay = 8.f;
	MinAttackDelay = 5.f;

	// ������ �̵� ������ ���� ����
	MaxRandomMoveDelay = 4.f;
	MinRandomMoveDelay = 2.f;

	AttachWeapon();
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// ���� Ŭ���� ��������
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

	// ���� Attach
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

	// CombatActions �迭�� ����ִ� ��쿡�� �����Ѵ�.
	// CombatActions �迭�� ���� ������ Add�Ѵ�.
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

	// bOnAttack�� false�϶� �����Ѵ�.
	// AttackDelay�� 0�����϶� �����Ѵ�.
	if (!bOnAttack)
	{
		bOnAttack = true;
		bOnHit = false;
		bIsMoveToTarget = false;
		bIsStopMovement = false;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Enemy Attacking"));

		// �̵� ���߱�
		AIController->StopMovement();
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bIsRun = false;
		
		// CombatAction �迭�� ���κк��� ��Ÿ�� �ִϸ��̼��� �����ͼ� �����Ѵ�.
		float CountNum = CombatActions.Num();
		currentMontage = CombatActions[CountNum - 1];
		CombatActions.RemoveAt(CountNum - 1);

		// ���� ������ �� �����Ͽ��ٸ�, ���� �����̸� �����Ѵ�.
		if (IsEmptyCombatActions())
			SetAttackDelay();

		// ��Ÿ�� ����
		EnemyAnim->PlayMontage(currentMontage);
		// ��Ÿ�� Section ��ȣ�� ù��° �������� �����Ѵ�.
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
	// ��� ��Ÿ�� ����
	EnemyAnim->StopAllMontages(0.25f);
	// �����ڿ� ���� ���������� ��ġ�Ѵ�
	SetActorLocation(Location);

	// �����ڿ� ���� �����̼����� ��ġ�Ѵ�
	FRotator RotateToPlayer = Causer->GetActorRotation();
	SetActorRotation(RotateToPlayer);

	// ó�� ��Ÿ�ָ� �����Ѵ�
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