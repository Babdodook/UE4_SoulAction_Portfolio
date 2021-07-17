// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_PlayerAnim.h"
#include "CustomStruct.h"
#include "EnumState.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "CS_Player.generated.h"

class ACS_Weapon;
class ACS_Enemy;
class UCS_TargetingSystem;

UCLASS()
class SOULSLIKEACTION_API ACS_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACS_Player();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Camera boom positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	// Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Second Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SecondCamera;

	// Ref Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* RefCamera;

	// Base turn rates to scale turning function for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
#pragma region Movement
	FVector MoveDirection;
	FVector ForwardDirection;
	FVector RightDirection;
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector NotifyMoveDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float InputVertical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float InputHorizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NotifyMoveVertical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NotifyMoveHorizontal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsRolling;

	// 이동속도 조절 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginMoveSpeed;

	// 선형보간된 이동속도 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LerpMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsCanSprint;

	// 원래 카메라 Lag속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float OriginCameraLagSpeed;

	// true일 경우 이동 가능, false일 경우 이동 불가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMoveable;

	// 공격 키 선입력 체크하기 위한 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsInputAttackKey;

	// 선입력 받았을 시 True
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bPlayNextAttackAuto;

	// 구르기 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector EvadeDirection;

	bool bIsWalk;
public:
	void Walk();

	void WalkEnd();
#pragma endregion


#pragma region Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* currentMontage;

	// 노티파이 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanApplyDmg;

	// 공격 가능한 상태 체크하기 위한 boolean
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyDamageCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnDefense;

	// 무적 상태, 회피할때 짧은 시간동안 활성화한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInvincibility;

	// 약 공격 번호
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int LightAttackNum;

	// 강 공격 번호
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int HeavyAttackNum;

	// 약 공격 자동 실행
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPlayLightAttackAuto;

	// 강 공격 자동 실행
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPlayHeavyAttackAuto;

	// 공격 중 강 공격 눌렀음
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsHeavyAttack;

	// 쉬프트키를 눌렀는지.. 강 공격 준비하기 위함
	bool bPressShift;
#pragma endregion


#pragma region PlayerStats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float HealthCover;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;
#pragma endregion


#pragma region Timer
	FTimerHandle StaminaRestoreTimerHandle;
	float StaminaRestoreTime;
	bool bIsStaminaTimerEnd;
	
	FTimerHandle ComboResetTimerHandle;
	float ComboResetTime;
	
	FTimerHandle ChangeMatTimerHandle;
	float ChangeMatTime;
#pragma endregion
	

#pragma region TargetingSystem
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "TargetSystem")
	UCS_TargetingSystem* TargetSystemComponent;
#pragma endregion


#pragma region ClimbSystem
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UCS_ClimbSystem* ClimbSystemComponent;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UArrowComponent* LeftArrow;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UArrowComponent* RightArrow;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UArrowComponent* LeftEdge;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UArrowComponent* RightEdge;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "ClimbSystem")
	class UArrowComponent* UpArrow;

	void ClimbUp();

	void ClimbTurn();

	void ClimbTurnBack();

	void ClimbTurnBackEnd();
#pragma endregion


#pragma region IKFootSystem
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = "IKFootSystem")
	class UCS_IKFootSystem* IKFootSystemComponent;

#pragma endregion


#pragma region Other
	bool bCheckTranslate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UCanvasPanel* BossCanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UCanvasPanel* TutorialTextCanvasPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShake> AttackCameraShake;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShake> OnHitCameraShake;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShake> GuardCameraShake;

	UMaterial* Mat_Origin;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* Mat_GridInst;

	ACS_Enemy* ExecuteTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnExecute;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	UCS_PlayerAnim* playerAnim;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Combat")
	ACS_Enemy* BossEnemy;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Combat")
	class ACS_TutorialAI* TutorialEnemy;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACS_Weapon> WeaponClass;

public:
	ACS_Weapon* Weapon;
#pragma endregion


	// Functions
public:		
#pragma region Movement
	/** Called for forwards/backwards input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RotateActor();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMoveable(bool Value) { bMoveable = Value; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetForwardDirection(FVector Value) { ForwardDirection = Value; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetRightDirection(FVector Value) { RightDirection = Value; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetForwardDirection() { return ForwardDirection; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetRightDirection() { return RightDirection; }

	/** 방향키 입력으로 캐릭터를 움직이려고 했다면 true를 반환한다. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsTryToMoveActor();

	/** 현재 실행중인 애니메이션을 중지하고 Movement 애니메이션을 실행한다. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void PlayMovementAnimation();

	/** 캐릭터를 인풋 Value에 따라 MoveDirection으로 이동한다 */
	void Move();

	/** 캐릭터가 해당 Direction으로 Value만큼 이동한다 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetVelocityToDirection(FVector Direction, float Value);
#pragma endregion


#pragma region Combat
	/** 구르기 */
	void Dodge();
	void Slide();

	/** 공격 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

	/** 특수 공격 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpecialAttack();

	/** 전력질주 */
	void Sprint();
	void StopSprint();

	/** 무기 장비 */
	void EquipWeapon();

	/** 루트모션중 로테이션 사용 */
	void SetRootRotation(bool Value);

	/** bOnAttack 값 설정 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetOnAttack(bool Value);

	/** 데미지 적용 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void DecrementHealth(float Amount);

	void DecrementStamina(float Amount);

	/** 피격 각도에 따라 애니메이션 지정 */
	void SetHitMontage(AActor* DamageCauser);

	/** 가드 상태로 */
	void Defense();

	void EndDefense();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetLightAttackMontage();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetHeavyAttackMontage();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsDefenseSucceed(AActor* DamageCauser);
#pragma endregion


#pragma region InputManagement
	/** 왼쪽 마우스 입력 */
	void InputLMB();

	/** 스페이스 바 입력 */
	void InputSpaceBar();

	/** 쉬프트 입력 */
	void InputShift();
	/** 쉬프트 뗌 */
	void ReleaseShift();

	/** R키 누름 */
	void InputR();

	/** 마우스휠 클릭 */
	void InputMWheel();

	/** Tab */
	void InputTab();

	void InputTestBtn();

	void InputRMB();

	void InputQ();

	// 가드 지속 상태
	void DefenseLoop();

	// 강공격
	UFUNCTION(BlueprintCallable)
	void HeavyAttack();

	void MWheelUp();

	void MWheelDown();

#pragma endregion


#pragma region Timer
	void StaminaRestoreTimer();
	void StaminaTimerActive();

	void ComboResetTimer();
	void ComboResetTimerActive();

	void ChangeMatTimer();
	void ChangeMatTimerActive();
#pragma endregion


#pragma region Other
	UFUNCTION(BlueprintCallable, Category = "TargetSystem")
	FORCEINLINE UCS_TargetingSystem* GetTargetSystemComponent() const { return TargetSystemComponent; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ACS_Weapon* GetWeapon() { return Weapon; }

	/** 무기 생성, 붙이기 */
	void AttachWeapon();

	void SetWeaponLocation(FName SocketName);

	UFUNCTION(BlueprintCallable)
	FVector GetNewMoveDirection(MoveDirectionStatus Status);

	UFUNCTION(BlueprintCallable)
	FVector GetUnitAxisForward();

	UFUNCTION(BlueprintCallable)
	FVector GetUnitAxisRight();

	void PlayCamShake();

	UFUNCTION(BlueprintCallable)
	void ExecuteApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void ExecuteRotateToTarget();

	/** 스탯 정보 갱신 (UI 정보 반영) */
	void UpdateStats(float DeltaTime);
#pragma endregion


#pragma region DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_DamageType;

	TArray<FName> RowNames;

	TArray<FDamageTypeStruct> DamageTypeAry;

#pragma endregion
};
