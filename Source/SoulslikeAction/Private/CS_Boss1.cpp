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

	// �� ���� �ʱ�ȭ
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

	// ���� ���� ����
	MaxAttackRandomRange = 5;

	// ���� ������ ���� ����
	MaxAttackDelay = 6.f;
	MinAttackDelay = 2.f;

	// ������ �̵� ������ ���� ����
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

	// ���� Attach
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

	// CombatActions �迭�� ����ִ� ��쿡�� �����Ѵ�.
	// CombatActions �迭�� ���� ������ Add�Ѵ�.
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

	// bOnAttack�� false�϶� �����Ѵ�.
	// AttackDelay�� 0�����϶� �����Ѵ�.
	if (!bOnAttack && !bOnGroggy)
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

void ACS_Boss1::Die()
{
	Super::Die();
}

void ACS_Boss1::SpawnGhostTrail(float SpawnDelay)
{
	// Ÿ�̸� �������̶�� �������� �ʴ´�.
	if (GetWorldTimerManager().IsTimerActive(SpawnGhostTrailTimerHandle)) return;

	// ���� �����̸� �����Ѵ�.
	SpawnGhostTrailTime = SpawnDelay;
	GetWorldTimerManager().SetTimer(SpawnGhostTrailTimerHandle, this, &ACS_Boss1::SpawnGhostTrailTimer, GetWorld()->GetDeltaSeconds(), true);

	// ��Ʈ Ʈ���� ���͸� �����Ѵ�.
	ACS_GhostTrail* GhostTrail = GetWorld()->SpawnActor<ACS_GhostTrail>(GhostTrailClass, GetMesh()->GetComponentTransform());
	// ���� Boss1�� �޽��� ����� �����Ѵ�.
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
	// ��� ��Ÿ�� ����
	EnemyAnim->StopAllMontages(0.25f);
	// �����ڿ� ���� ���������� ��ġ�Ѵ�
	SetActorLocation(Location);

	// �����ڿ� ���� �����̼����� ��ġ�Ѵ�
	FRotator RotateToPlayer = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation());
	SetActorRotation(RotateToPlayer);

	// ó�� ��Ÿ�ָ� �����Ѵ�
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
	// �׷α� ���·� �����Ѵ�
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Groggy);
	// AI ���º��� ����
	bOnGroggy = true;
	bIsCanExecute = true;

	// �����ڿ� ���� �����̼����� ��ġ�Ѵ�
	FRotator RotateToPlayer = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation());
	SetActorRotation(RotateToPlayer);
	// �׷α� ��Ÿ�� ����
	EnemyAnim->PlayMontage(EnemyAnim->GroggyMontage);
}

bool ACS_Boss1::IsExecuteReady()
{
	return bIsCanExecute;
}

void ACS_Boss1::CastSpell()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_CastSpell);

	// ĳ���� ��Ÿ�� ����
	EnemyAnim->PlayMontage(Boss1Anim->CastMontage);
}