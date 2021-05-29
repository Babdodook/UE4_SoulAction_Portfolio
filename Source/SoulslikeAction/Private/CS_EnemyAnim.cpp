// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_EnemyAnim.h"
#include "CS_Enemy.h"

UCS_EnemyAnim::UCS_EnemyAnim()
{
	
}

void UCS_EnemyAnim::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			Enemy = Cast<ACS_Enemy>(Pawn);
		}
	}
}

void UCS_EnemyAnim::LoadMontageAssets()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITFRONT_MONTAGE(TEXT("HitFrontMontage'/Game/Actors/Enemy/Base/Animation/Montage/EHitFront_Montage.EHitFront_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITLEFT_MONTAGE(TEXT("HitLeftMontage'/Game/Actors/Enemy/Base/Animation/Montage/EHitLeft_Montage.EHitLeft_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITRIGHT_MONTAGE(TEXT("HitRightMontage'/Game/Actors/Enemy/Base/Animation/Montage/EHitRight_Montage.EHitRight_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HITBACK_MONTAGE(TEXT("HitBackMontage'/Game/Actors/Enemy/Base/Animation/Montage/EHitBack_Montage.EHitBack_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DEATH_MONTAGE(TEXT("DeathMontage'/Game/Actors/Enemy/Base/Animation/Montage/EDeath_Montage.EDeath_Montage'"));

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
	if (DEATH_MONTAGE.Succeeded())
	{
		DeathMontage = DEATH_MONTAGE.Object;
	}
}

void UCS_EnemyAnim::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			Enemy = Cast<ACS_Enemy>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();
	}
}

void UCS_EnemyAnim::PlayMontage(UAnimMontage* Montage)
{
	Montage_Play(Montage);
	nowPlayingMontage = Montage;
}

void UCS_EnemyAnim::StopNowPlayingMontage()
{
	Montage_Stop(0.3f, nowPlayingMontage);
}

void UCS_EnemyAnim::StopCurrentMontage(UAnimMontage* Montage)
{
	Montage_Stop(0.3f, Montage);
}

void UCS_EnemyAnim::NextSection()
{
	FName SectionName;

	switch (++SectionNumber)
	{
	case 1:
		SectionName = "Section1";
		break;
	case 2:
		SectionName = "Section2";
		break;
	case 3:
		SectionName = "Section3";
		break;
	case 4:
		SectionName = "Section4";
		break;
	}

	StopAllMontages(0.25f);
	Montage_Play(nowPlayingMontage);
	Montage_JumpToSection(SectionName, nowPlayingMontage);
}

void UCS_EnemyAnim::SetAnimSpeed(float Value)
{
	Montage_SetPlayRate(nowPlayingMontage, Value);
}