// Fill out your copyright notice inQ the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CS_Weapon.generated.h"

UENUM(BlueprintType)
enum class EAttackType :uint8
{
	EDT_Light			UMETA(DeplayName = "Light"),
	EDT_Heavy			UMETA(DeplayName = "Heavy"),

	EDT_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UCLASS()
class SOULSLIKEACTION_API ACS_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACS_Weapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Equip(class ACS_Player* Character);

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponMesh2;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponHandle;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponBlade;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponHandleCopy;

	UPROPERTY(VisibleDefaultsOnly, Category = Weapon)
	UStaticMeshComponent* WeaponBladeCopy;

	float DamageAmount;

public:
	// ���� ���� ���� �÷���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttack;

	/** �÷��̾� ���� �浹 üũ, ���� Ʈ���̽� */
	UFUNCTION(BlueprintCallable)
	void PlayerWeaponCollisionCheck();

	/** �� ���� �浹 üũ, ���� Ʈ���̽� */
	UFUNCTION(BlueprintCallable)
	void EnemyWeaponCollisionCheck();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAttackType AttackType;

	/** ������ �����ϱ� */
	UFUNCTION(BlueprintCallable)
	void SetDamageAmount(float Value) { DamageAmount = Value; }

	void SetBladeOnOff(bool Value);

	FTimerHandle BladeOnOffTimerHandle;
	float BladeOnOffTime;
	float BladeZScale;
	void BladeOnOffTimer();

	void SetVisibility(bool Value);
};
