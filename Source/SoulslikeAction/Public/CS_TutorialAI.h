// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CS_Enemy.h"
#include "EnumState.h"
#include "CS_TutorialAI.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKEACTION_API ACS_TutorialAI : public ACS_Enemy
{
	GENERATED_BODY()

public:

	ACS_TutorialAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	ETutorialAIStatus TutorialAIStatus;

	bool bIsSetCombatTarget;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Executed(AActor* Causer, FVector Location) override;

	virtual void ExecutedTakeDamage(float DamageAmount) override;

	virtual void ExecutedEnd() override;

	virtual bool IsExecuteReady() override;

	void SetTutorialAIStatus(ETutorialAIStatus Status) { TutorialAIStatus = Status; }
	ETutorialAIStatus GetTutorialAIStatus() { return TutorialAIStatus; }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

#pragma region UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TutorialText;

	void SetTutorialQuest();
#pragma endregion

	// Functions
protected:
	virtual void AttachWeapon() override;

	virtual void CombatBehaviour() override;

	virtual void Attack() override;

	virtual void Die() override;
};
