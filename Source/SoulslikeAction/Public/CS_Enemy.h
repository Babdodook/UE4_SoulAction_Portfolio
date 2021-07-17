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
	/** 무기 생성, 붙이기 */
	virtual void AttachWeapon() {}

public:
	UFUNCTION(BlueprintCallable)
	ACS_Weapon* GetWeapon() { return Weapon; }
#pragma endregion


#pragma region AI
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EEnemyType EnemyType;

	// 어그로용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

public:
	UFUNCTION(BlueprintCallable)
	void SetEnemyType(EEnemyType Type) { EnemyType = Type; }

	UFUNCTION(BlueprintCallable)
	EEnemyType GetEnemyType() { return EnemyType; }

	/** 어그로 원 오버랩 시작*/
	UFUNCTION()
	virtual void AgroSphere_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActiveAIBehaviour(AActor* Target);
#pragma endregion


#pragma region Combat
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* currentMontage;

	/** 시야 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SightRange;

	/** 시야 각 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SightAngle;

	/** 공격 사거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	/** 공격 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDelay;

	float MaxAttackDelay;
	float MinAttackDelay;

	/** 공격 각도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHit;

	/** 피격받은 방향 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector HitDirection;

	/** 공격패턴 중복 확인용, 이전 패턴 번호를 저장해둔다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int PrevCombatRandNum;

	/** 피격 몽타주를 실행하는지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHitReaction;

	bool bOnDeathEnd;

	// 플레이어가 죽을때 처형으로 죽었는지 여부, 일어서서 죽는 몽타주를 실행하지 않는다
	bool bIsExecuted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	ACS_Player* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float m_targetDistance;

	/** 그로기 상태로 돌입시 true, 활성화 시 Attack() 실행 못함*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnGroggy;

	/** 플레이어가 처형 액션 사용시 체크용으로 사용 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsCanExecute;

	bool bOnExecution;

protected:
	/** 몬스터 공격패턴을 결정한다. */
	virtual void CombatBehaviour() {}

	/** 공격 */
	UFUNCTION(BlueprintCallable)
	virtual void Attack();

public:
	/** 타겟으로 이동 */
	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class ACS_Player* Target);

	/** 전투 시 캐릭터 이동 */
	void CombatMovement();

	/** 데미지 적용 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/**	타겟과의 거리 반환 */
	float GetTargetDistance();

	/** 타겟으로 회전한다. */
	UFUNCTION(BlueprintCallable)
	void RotateToTarget(float RotateSpeed);

	/** 타겟이 시야각과 거리안에 있는지? */
	bool IsTargetExistInSight();

	/** 타겟이 공격 사정거리와 공격 각도안에 있는지? */
	bool IsTargetExistInAttackArea();

	/** CombatAction 배열이 비어있는지 결과를 반환한다. */
	UFUNCTION(BlueprintCallable)
	bool IsEmptyCombatActions();

	/** 공격 딜레이를 지정한다. */
	void SetAttackDelay();

	/** 공격 딜레이가 0이하라면 true를 반환한다. */
	bool IsAttackDelayZero();

	/** 무작위 이동 딜레이를 지정한다. */
	void SetRandomMoveDelay();

	/** 무작위 이동 딜레이가 0이하라면 true를 반환한다. */
	bool IsRandomMoveDelayZero();

	/** 피격 방향에 따라서 피격 애니메이션을 currentMontage에 지정한다. */
	void SetHitMontage();

	/** CombatActions 배열을 초기화하고 bOnAttack을 false로 지정한다. (공격 초기화) */
	void ReSetAttack();

	/** 공격, 무작위 이동 등 딜레이 변수를 DeltaTime만큼 매 틱마다 감소한다 */
	void UpdateDelay(float DeltaTime);

	/** 죽음 상태 */
	virtual void Die();

	void Ragdoll();

	/** 살아있는지 */
	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	/** 죽음 끝나는 상태, 래그돌 적용 */
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void SetAttackArea(float Range, float Angle);

	int GenerateCombatRandNum();

	/** 처형 모션 시작 */
	virtual void Executed(AActor* Causer, FVector Location) {}

	/** 처형 모션 중 데미지 입음 */
	virtual void ExecutedTakeDamage(float DamageAmount) {}

	/** 처형 모션 끝 */
	UFUNCTION(BlueprintCallable)
	virtual void ExecutedEnd() {}

	virtual void Groggy(AActor* Causer) {}

	/** 처형 모션 가능한지 반환 */
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

	// 무작위 방향으로 이동하기 위한 딜레이
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

	/** AI 이동 방향을 정의하기위한 Vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection;

	/** 이동 방향 상태 노티파이 이벤트에서 수정 가능*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMoveDirectionStatus MoveDirectionStatus;

	// Vertical, Horizontal
	float v;
	float h;

	/** AI가 이동을 멈추는 타겟과의 거리 */
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

	/** 달리기 랜덤 설정 */
	void SetIsRunRandom();

	/** AI MaxWalkSpeed Tick에서 업데이트 */
	void UpdateMaxWalkSpeed();
#pragma endregion


#pragma region UI
public:
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "UI")
	class UCS_TargetPointComponent* TargetPointComponent;

	/** UMG 에셋 / 유저 체력바 */
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

	/** 체력바 감추기, 보이기 */
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
	/** 체력 Amount만큼 감소 */
	void DecrementHealth(float Amount);

	void IncrementGroggyPoint(float Amount);

	/** 스탯 정보 갱신 (UI 정보 반영) */
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

	/** 공격 패턴 저장할 배열 */
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
