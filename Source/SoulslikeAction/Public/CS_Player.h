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

	// �̵��ӵ� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float OriginMoveSpeed;

	// ���������� �̵��ӵ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float LerpMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsCanSprint;

	// ���� ī�޶� Lag�ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float OriginCameraLagSpeed;

	// true�� ��� �̵� ����, false�� ��� �̵� �Ұ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMoveable;

	// ���� Ű ���Է� üũ�ϱ� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsInputAttackKey;

	// ���Է� �޾��� �� True
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bPlayNextAttackAuto;

	// ������ ����
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

	// ��Ƽ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanApplyDmg;

	// ���� ������ ���� üũ�ϱ� ���� boolean
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyDamageCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bOnDefense;

	// ���� ����, ȸ���Ҷ� ª�� �ð����� Ȱ��ȭ�Ѵ�.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInvincibility;

	// �� ���� ��ȣ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int LightAttackNum;

	// �� ���� ��ȣ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	int HeavyAttackNum;

	// �� ���� �ڵ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPlayLightAttackAuto;

	// �� ���� �ڵ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bPlayHeavyAttackAuto;

	// ���� �� �� ���� ������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsHeavyAttack;

	// ����ƮŰ�� ��������.. �� ���� �غ��ϱ� ����
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

	/** ����Ű �Է����� ĳ���͸� �����̷��� �ߴٸ� true�� ��ȯ�Ѵ�. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsTryToMoveActor();

	/** ���� �������� �ִϸ��̼��� �����ϰ� Movement �ִϸ��̼��� �����Ѵ�. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void PlayMovementAnimation();

	/** ĳ���͸� ��ǲ Value�� ���� MoveDirection���� �̵��Ѵ� */
	void Move();

	/** ĳ���Ͱ� �ش� Direction���� Value��ŭ �̵��Ѵ� */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetVelocityToDirection(FVector Direction, float Value);
#pragma endregion


#pragma region Combat
	/** ������ */
	void Dodge();
	void Slide();

	/** ���� */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

	/** Ư�� ���� */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpecialAttack();

	/** �������� */
	void Sprint();
	void StopSprint();

	/** ���� ��� */
	void EquipWeapon();

	/** ��Ʈ����� �����̼� ��� */
	void SetRootRotation(bool Value);

	/** bOnAttack �� ���� */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetOnAttack(bool Value);

	/** ������ ���� */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void DecrementHealth(float Amount);

	void DecrementStamina(float Amount);

	/** �ǰ� ������ ���� �ִϸ��̼� ���� */
	void SetHitMontage(AActor* DamageCauser);

	/** ���� ���·� */
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
	/** ���� ���콺 �Է� */
	void InputLMB();

	/** �����̽� �� �Է� */
	void InputSpaceBar();

	/** ����Ʈ �Է� */
	void InputShift();
	/** ����Ʈ �� */
	void ReleaseShift();

	/** RŰ ���� */
	void InputR();

	/** ���콺�� Ŭ�� */
	void InputMWheel();

	/** Tab */
	void InputTab();

	void InputTestBtn();

	void InputRMB();

	void InputQ();

	// ���� ���� ����
	void DefenseLoop();

	// ������
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

	/** ���� ����, ���̱� */
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

	/** ���� ���� ���� (UI ���� �ݿ�) */
	void UpdateStats(float DeltaTime);
#pragma endregion


#pragma region DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_DamageType;

	TArray<FName> RowNames;

	TArray<FDamageTypeStruct> DamageTypeAry;

#pragma endregion
};
