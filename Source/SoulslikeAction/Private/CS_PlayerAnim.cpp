// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_PlayerAnim.h"
#include "CS_Player.h"
#include "CS_ClimbSystem.h"
#include "GameFramework/CharacterMovementComponent.h"


UCS_PlayerAnim::UCS_PlayerAnim()
{
	LoadMontageAssets();

	bIsSprint = false;
	bIsEquip = false;
}

/** 몽타주 에셋 불러오기 */
void UCS_PlayerAnim::LoadMontageAssets()
{
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AttackMontage'/Game/Actors/Player/Animation/Montage/Attack1_Montage.Attack1_Montage'"));
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> ROLL_MONTAGE(TEXT("RollMontage'/Game/Actors/Player/Animation/Montage/RollForward_Montage.RollForward_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> EQUIP_MONTAGE(TEXT("EquipMontage'/Game/Actors/Player/Animation/Montage/Equip_Montage.Equip_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> UNARM_MONTAGE(TEXT("UnarmMontage'/Game/Actors/Player/Animation/Montage/Unarm_Montage.Unarm_Montage'"));


	/*if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}*/

	/*if (ROLL_MONTAGE.Succeeded())
	{
		RollForwardMontage = ROLL_MONTAGE.Object;
	}*/

	if (EQUIP_MONTAGE.Succeeded())
	{
		EquipMontage = EQUIP_MONTAGE.Object;
	}

	if (UNARM_MONTAGE.Succeeded())
	{
		UnarmMontage = UNARM_MONTAGE.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITFRONT_MONTAGE(TEXT("HitFrontMontage'/Game/Actors/Player/Animation/Montage/HitFront_Montage.HitFront_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITLEFT_MONTAGE(TEXT("HitLeftMontage'/Game/Actors/Player/Animation/Montage/HitLeft_Montage.HitLeft_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITRIGHT_MONTAGE(TEXT("HitRightMontage'/Game/Actors/Player/Animation/Montage/HitRight_Montage.HitRight_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITBACK_MONTAGE(TEXT("HitBackMontage'/Game/Actors/Player/Animation/Montage/HitBack_Montage.HitBack_Montage'"));

	if (HITFRONT_MONTAGE.Succeeded())
	{
		HitFrontMontage = HITFRONT_MONTAGE.Object;
	}
	if (HITLEFT_MONTAGE.Succeeded())
	{
		HitLeftMontage = HITLEFT_MONTAGE.Object;
	}
	if (HITRIGHT_MONTAGE.Succeeded())
	{
		HitRightMontage = HITRIGHT_MONTAGE.Object;
	}
	if (HITBACK_MONTAGE.Succeeded())
	{
		HitBackMontage = HITBACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEFENSE_MONTAGE(TEXT("DefenseMontage'/Game/Actors/Player/Animation/Montage/Defense_Montage.Defense_Montage'"));

	if (DEFENSE_MONTAGE.Succeeded())
	{
		DefenseMontage = DEFENSE_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEFENSE_HIT_MONTAGE(TEXT("DefenseHitMontage'/Game/Actors/Player/Animation/Montage/DefenseHit_Montage.DefenseHit_Montage'"));

	if (DEFENSE_HIT_MONTAGE.Succeeded())
	{
		DefenseHitMontage = DEFENSE_HIT_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEFENSE_LOOP_MONTAGE(TEXT("DefenseLoop_Montage'/Game/Actors/Player/Animation/Montage/DefenseLoop_Montage.DefenseLoop_Montage'"));

	if (DEFENSE_LOOP_MONTAGE.Succeeded())
	{
		DefenseLoop_Montage = DEFENSE_LOOP_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SLIDEF_MONTAGE(TEXT("SlideF_Montage'/Game/Actors/Player/Animation/Montage/SlideF_Montage.SlideF_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SLIDER_MONTAGE(TEXT("SlideR_Montage'/Game/Actors/Player/Animation/Montage/SlideR_Montage.SlideR_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SLIDEL_MONTAGE(TEXT("SlideL_Montage'/Game/Actors/Player/Animation/Montage/SlideL_Montage.SlideL_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SLIDEB_MONTAGE(TEXT("SlideB_Montage'/Game/Actors/Player/Animation/Montage/SlideB_Montage.SlideB_Montage'"));

	if (SLIDEF_MONTAGE.Succeeded())
	{
		SlideF_Montage = SLIDEF_MONTAGE.Object;
	}
	if (SLIDER_MONTAGE.Succeeded())
	{
		SlideR_Montage = SLIDER_MONTAGE.Object;
	}
	if (SLIDEL_MONTAGE.Succeeded())
	{
		SlideL_Montage = SLIDEL_MONTAGE.Object;
	}
	if (SLIDEB_MONTAGE.Succeeded())
	{
		SlideB_Montage = SLIDEB_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> LARGE_HIT_MONTAGE(TEXT("LargetHit_Montage'/Game/Actors/Player/Animation/Montage/LargetHit_Montage.LargetHit_Montage'"));

	if (LARGE_HIT_MONTAGE.Succeeded())
	{
		LargetHit_Montage = LARGE_HIT_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEFENSE_BROKEN_MONTAGE(TEXT("DefenseBroken_Montage'/Game/Actors/Player/Animation/Montage/DefenseBroken_Montage.DefenseBroken_Montage'"));

	if (DEFENSE_BROKEN_MONTAGE.Succeeded())
	{
		DefenseBroken_Montage = DEFENSE_BROKEN_MONTAGE.Object;
	}
}

void UCS_PlayerAnim::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		playerClass = Cast<ACS_Player>(Pawn);
	}

	LightAttackAry.Add(LightAttack1Montage);
	LightAttackAry.Add(LightAttack2Montage);
	HeavyAttackAry.Add(HeavyAttack1Montage);
	HeavyAttackAry.Add(HeavyAttack2Montage);
	HeavyAttackAry.Add(HeavyAttack3Montage);
}

void UCS_PlayerAnim::UpdateAnimationProperties()
{
	
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if ((!playerClass->ClimbSystemComponent->bCanMoveLeft && playerClass->InputHorizontal < 0.f) ||
			(!playerClass->ClimbSystemComponent->bCanMoveRight && playerClass->InputHorizontal > 0.f))
		{
			InputHorizontal = 0.f;
		}
		else
			InputHorizontal = playerClass->InputHorizontal;
	}
}

void UCS_PlayerAnim::PlayRollMontage()
{
	/*if (!Montage_IsPlaying(RollForwardMontage))
	{
		Montage_Play(RollForwardMontage);
	}*/
}

void UCS_PlayerAnim::PlayMontage(UAnimMontage* Montage)
{
	Montage_Play(Montage);
	nowPlayingMontage = Montage;
}

void UCS_PlayerAnim::StopNowPlayingMontage()
{
	Montage_Stop(0.3f, nowPlayingMontage);
}

void UCS_PlayerAnim::StopCurrentMontage(UAnimMontage* Montage)
{
	Montage_Stop(0.3f, Montage);
}

void UCS_PlayerAnim::SetAnimSpeed(float Value)
{
	Montage_SetPlayRate(nowPlayingMontage, Value);
}