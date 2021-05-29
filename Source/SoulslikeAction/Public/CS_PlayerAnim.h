// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CS_PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API UCS_PlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

	// Maontages
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* TestMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* LightAttack1Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* LightAttack2Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HeavyAttack1Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HeavyAttack2Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HeavyAttack3Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SAttack2Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* RollForwardMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* UnarmMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitFrontMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitLeftMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitRightMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitBackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DefenseMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DefenseHitMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DefenseLoop_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SlideF_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SlideR_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SlideL_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* SlideB_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* LargetHit_Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DefenseBroken_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Climb_Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* ClimbUp_Montage;

	UAnimMontage* nowPlayingMontage;

	TArray<UAnimMontage*> LightAttackAry;
	TArray<UAnimMontage*> HeavyAttackAry;

	// Functions
private:

	/** 몽타주 에셋 불러오기 */
	void LoadMontageAssets();

	// Functions
public:		
	virtual void NativeInitializeAnimation() override;

	UCS_PlayerAnim();
	
	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties();

	/** 구르기 몽타주 실행 */
	UFUNCTION(BlueprintCallable, Category = Attack)
	void PlayRollMontage();

	/** 몽타주 실행 */
	UFUNCTION(BlueprintCallable, Category = Attack)
	void PlayMontage(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	void StopNowPlayingMontage();

	UFUNCTION(BlueprintCallable)
	void StopCurrentMontage(UAnimMontage* Montage);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UFUNCTION(BlueprintCallable, Category = Montage)
	void SetAnimSpeed(float Value);

	// Variables
public:		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsRolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float CombatMoveAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float CombatMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsLockOn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsHanging;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bClimbJump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsClimbingUp;
};
