// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CS_EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API UCS_EnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
	// Functions
protected:
	virtual void NativeInitializeAnimation() override;

	/** 몽타주 애니메이션 로드 */
	virtual void LoadMontageAssets();

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	virtual void UpdateAnimationProperties();

public:

	UCS_EnemyAnim();

	UFUNCTION(BlueprintCallable, Category = Attack)
	void PlayMontage(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = Notify)
	void NextSection();

	int SectionNumber;

	// Variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class ACS_Enemy* Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float CombatMoveAngle;

	/** Combat과 General Movement 변경위한 boolean*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsTargetExist;

	UFUNCTION(BlueprintCallable, Category = Movement)
	void SetTargetExist(bool Value) { bIsTargetExist = Value; }
	
	UFUNCTION(BlueprintCallable, Category = Montage)
	void SetAnimSpeed(float Value);

	// Montage
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Attack1Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Attack2Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Attack3Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Attack4Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* Attack5Montage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DamageMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitFrontMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitLeftMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitRightMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* HitBackMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* ExecutedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	UAnimMontage* GroggyMontage;

	UAnimMontage* nowPlayingMontage;

	UFUNCTION(BlueprintCallable)
	void StopNowPlayingMontage();

	UFUNCTION(BlueprintCallable)
	void StopCurrentMontage(UAnimMontage* Montage);
};
