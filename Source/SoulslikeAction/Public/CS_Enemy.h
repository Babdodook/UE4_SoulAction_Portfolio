// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_EnemyAnim.h"
#include "CustomStruct.h"
#include "EnumState.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "CS_Enemy.generated.h"

class ACS_Weapon;
class ACS_EnemySpawner;

UCLASS()
class SOULSLIKEACTION_API ACS_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACS_Enemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Weapon
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Weapon> WeaponClass;

	UPROPERTY()
	ACS_Weapon* Weapon;

protected:
	/** ���� ����, ���̱� */
	virtual void AttachWeapon() {}

public:
	UFUNCTION(BlueprintCallable)
	ACS_Weapon* GetWeapon() { return Weapon; }
#pragma endregion


#pragma region AI
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyType EnemyType;

	// ��׷ο�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

public:
	UFUNCTION(BlueprintCallable)
	void SetEnemyType(EEnemyType Type) { EnemyType = Type; }

	UFUNCTION(BlueprintCallable)
	EEnemyType GetEnemyType() { return EnemyType; }

	/** ��׷� �� ������ ����*/
	UFUNCTION()
	virtual void AgroSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActiveAIBehaviour(AActor* Target);
#pragma endregion


#pragma region Combat
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* currentMontage;

	/** �þ� �Ÿ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SightRange;

	/** �þ� �� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SightAngle;

	/** ���� ��Ÿ� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	/** ���� ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDelay;

	float MaxAttackDelay;
	float MinAttackDelay;

	/** ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHit;

	/** �ǰݹ��� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector HitDirection;

	/** �������� �ߺ� Ȯ�ο�, ���� ���� ��ȣ�� �����صд�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int PrevCombatRandNum;

	/** �ǰ� ��Ÿ�ָ� �����ϴ��� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHitReaction;

	bool bOnDeathEnd;

	// �÷��̾ ������ ó������ �׾����� ����, �Ͼ�� �״� ��Ÿ�ָ� �������� �ʴ´�
	bool bIsExecuted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	ACS_Player* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float m_targetDistance;

	/** �׷α� ���·� ���Խ� true, Ȱ��ȭ �� Attack() ���� ����*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnGroggy;

	/** �÷��̾ ó�� �׼� ���� üũ������ ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsCanExecute;

	bool bOnExecution;

protected:
	/** ���� ���������� �����Ѵ�. */
	virtual void CombatBehaviour() {}

	/** ���� */
	UFUNCTION(BlueprintCallable)
	virtual void Attack();

public:
	/** Ÿ������ �̵� */
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class ACS_Player* Target);

	/** ���� �� ĳ���� �̵� */
	void CombatMovement();

	/** ������ ���� */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/**	Ÿ�ٰ��� �Ÿ� ��ȯ */
	float GetTargetDistance();

	/** Ÿ������ ȸ���Ѵ�. */
	UFUNCTION(BlueprintCallable)
	void RotateToTarget(float RotateSpeed);

	/** Ÿ���� �þ߰��� �Ÿ��ȿ� �ִ���? */
	bool IsTargetExistInSight();

	/** Ÿ���� ���� �����Ÿ��� ���� �����ȿ� �ִ���? */
	bool IsTargetExistInAttackArea();

	/** CombatAction �迭�� ����ִ��� ����� ��ȯ�Ѵ�. */
	UFUNCTION(BlueprintCallable)
	bool IsEmptyCombatActions();

	/** ���� �����̸� �����Ѵ�. */
	void SetAttackDelay();

	/** ���� �����̰� 0���϶�� true�� ��ȯ�Ѵ�. */
	bool IsAttackDelayZero();

	/** ������ �̵� �����̸� �����Ѵ�. */
	void SetRandomMoveDelay();

	/** ������ �̵� �����̰� 0���϶�� true�� ��ȯ�Ѵ�. */
	bool IsRandomMoveDelayZero();

	/** �ǰ� ���⿡ ���� �ǰ� �ִϸ��̼��� currentMontage�� �����Ѵ�. */
	void SetHitMontage();

	/** CombatActions �迭�� �ʱ�ȭ�ϰ� bOnAttack�� false�� �����Ѵ�. (���� �ʱ�ȭ) */
	void ReSetAttack();

	/** ����, ������ �̵� �� ������ ������ DeltaTime��ŭ �� ƽ���� �����Ѵ� */
	void UpdateDelay(float DeltaTime);

	/** ���� ���� */
	virtual void Die();

	void Ragdoll();

	/** ����ִ��� */
	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	/** ���� ������ ����, ���׵� ���� */
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void SetAttackArea(float Range, float Angle);

	int GenerateCombatRandNum();

	/** ó�� ��� ���� */
	virtual void Executed(AActor* Causer, FVector Location) {}

	/** ó�� ��� �� ������ ���� */
	virtual void ExecutedTakeDamage(float DamageAmount) {}

	/** ó�� ��� �� */
	UFUNCTION(BlueprintCallable)
	virtual void ExecutedEnd() {}

	virtual void Groggy(AActor* Causer) {}

	/** ó�� ��� �������� ��ȯ */
	virtual bool IsExecuteReady() { return 0; }
#pragma endregion


#pragma region Movement
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bIsStopMovement;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool bIsMoveToTarget;

	bool bIsRun;

	// ������ �������� �̵��ϱ� ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RandomMoveDelay;

	float MaxRandomMoveDelay;
	float MinRandomMoveDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	/** AI �̵� ������ �����ϱ����� Vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	/** �̵� ���� ���� ��Ƽ���� �̺�Ʈ���� ���� ����*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMoveDirectionStatus MoveDirectionStatus;

	// Vertical, Horizontal
	float v;
	float h;

	/** AI�� �̵��� ���ߴ� Ÿ�ٰ��� �Ÿ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float AIStopDistance;

public:
	UFUNCTION(BlueprintCallable)
	void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }

	UFUNCTION(BlueprintCallable)
	EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UFUNCTION(BlueprintCallable)
	void SetVelocityToDirection(FVector Direction, float Value);

	UFUNCTION(BlueprintCallable)
	void SetMoveDirection(EMoveDirectionStatus Status);

	void SetMoveDirectionLerp(EMoveDirectionStatus Status);

	UFUNCTION(BlueprintCallable)
	void SetRandomMoveDirection();

	/** �޸��� ���� ���� */
	void SetIsRunRandom();

	/** AI MaxWalkSpeed Tick���� ������Ʈ */
	void UpdateMaxWalkSpeed();
#pragma endregion


#pragma region UI
public:
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "UI")
	class UCS_TargetPointComponent* TargetPointComponent;

	/** UMG ���� / ���� ü�¹� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	class UWidgetComponent* AIHealthBarComponenet;

	UUserWidget* WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UCanvasPanel* CanvasPanel;

	ACS_Player* DamageCausers;
public:
	void HideCanvasPanel(float DeltaTime);

	void VisibleCanvasPanel(float DeltaTime);

	void DisplayWidgetWhenTakeDamage(AActor* DamageCauser);

	FORCEINLINE UCS_TargetPointComponent* GetTargetPointComponent() const { return TargetPointComponent; }

	/** ü�¹� ���߱�, ���̱� */
	void SetWidgetVisible(bool Value);
#pragma endregion


#pragma region EnemyStats
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float HealthCover;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MaxGroggyPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float GroggyPoint;

public:
	/** ü�� Amount��ŭ ���� */
	void DecrementHealth(float Amount);

	void IncrementGroggyPoint(float Amount);

	/** ���� ���� ���� (UI ���� �ݿ�) */
	void UpdateStats(float DeltaTime);
#pragma endregion


#pragma region Timer
public:
	FTimerHandle WidgetDisplayTimerHandle;
	FTimerHandle ReSetOnHitTimerHandle;
	FTimerHandle ChangeMatTimerHandle;

	bool bShowHealthBar;

	float WidgetDisplayTime;
	float ReSetOnHitTime;
	float ChangeMatTime;

public:
	void WidgetDisplayTimer();

	void ReSetOnHitTimer();

	void ChangeMatTimer();

	void ChangeMatTimerActive();
#pragma endregion


#pragma region Other
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCS_EnemyAnim* EnemyAnim;

	/** ���� ���� ������ �迭 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Montage")
	TArray<UAnimMontage*> CombatActions;

	int32 MaxAttackRandomRange;

	UMaterial* Mat_Origin;

	UPROPERTY(EditAnywhere)
	UMaterial* Mat_Grid;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* Mat_GridInst;

	ACS_EnemySpawner* Spawner;

	UPROPERTY(EditAnywhere)
	bool bIsDummy;

	

public:
	void SetMeshMaterial(UMaterial* Mat);

	void SetSpawner(ACS_EnemySpawner* _Spawner) { Spawner = _Spawner; }
	ACS_EnemySpawner* GetSpawner() { return Spawner; }
#pragma endregion


#pragma region DataTable
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_AICombatData;

	TArray<FName> RowNames;

	TArray<FAICombatStruct> AICombatDataAry;

#pragma endregion

};
