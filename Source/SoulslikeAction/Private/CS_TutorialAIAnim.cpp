// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_TutorialAIAnim.h"

UCS_TutorialAIAnim::UCS_TutorialAIAnim()
{
	LoadMontageAssets();
}

void UCS_TutorialAIAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCS_TutorialAIAnim::UpdateAnimationProperties()
{
	Super::UpdateAnimationProperties();
}

void UCS_TutorialAIAnim::LoadMontageAssets()
{
	Super::LoadMontageAssets();

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK1_MONTAGE(TEXT("Attack1Montage'/Game/Actors/Enemy/Base/Animation/Montage/EAttack1_Montage.EAttack1_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK2_MONTAGE(TEXT("Attack2Montage'/Game/Actors/Enemy/Base/Animation/Montage/EAttack2_Montage.EAttack2_Montage'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DAMAGE_MONTAGE(TEXT("DamageMontage'/Game/Actors/Enemy/Base/Animation/Montage/DamageMontage.DamageMontage'"));

	if (ATTACK1_MONTAGE.Succeeded())
	{
		Attack1Montage = ATTACK1_MONTAGE.Object;
	}
	if (ATTACK2_MONTAGE.Succeeded())
	{
		Attack2Montage = ATTACK2_MONTAGE.Object;
	}
	if (DAMAGE_MONTAGE.Succeeded())
	{
		DamageMontage = DAMAGE_MONTAGE.Object;
	}


}