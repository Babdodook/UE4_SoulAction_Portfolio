// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	EMS_Idle			UMETA(DeplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DeplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DeplayName = "Attacking"),
	EMS_Groggy			UMETA(DeplayName = "Groggy"),
	EMS_CastSpell		UMETA(DeplayName = "CastSpell"),
	EMS_Dead			UMETA(DeplayName = "Dead"),

	EMS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EMoveDirectionStatus :uint8
{
	MDS_Forward			UMETA(DeplayName = "Forward"),
	MDS_ForwardLeft		UMETA(DeplayName = "MDS_ForwardLeft"),
	MDS_ForwardRight	UMETA(DeplayName = "MDS_ForwardRight"),
	MDS_Left			UMETA(DeplayName = "Left"),
	MDS_Right			UMETA(DeplayName = "Right"),
	MDS_Backward		UMETA(DeplayName = "Backward"),

	MDS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EEnemyType :uint8
{
	ET_Normal			UMETA(DeplayName = "Normal"),
	ET_Boss				UMETA(DeplayName = "Boss"),
	ET_TutoAI			UMETA(DeplayName = "TutorialAI"),

	MDS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class MoveDirectionStatus :uint8
{
	MDS_Forward			UMETA(DeplayName = "Forward"),
	MDS_Left			UMETA(DeplayName = "Left"),
	MDS_Right			UMETA(DeplayName = "Right"),
	MDS_Backward		UMETA(DeplayName = "Backward"),

	MDS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ESpawnLocatorStatus :uint8
{
	SLS_Point_1			UMETA(DeplayName = "Point_1"),
	SLS_Point_2			UMETA(DeplayName = "Point_2"),
	SLS_Point_3			UMETA(DeplayName = "Point_3"),
	SLS_BossPoint_1		UMETA(DeplayName = "BossPoint_1"),
	SLS_OriginPoint		UMETA(DeplayName = "SLS_OriginPoint"),

	SLS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ETutorialAIStatus :uint8
{
	TAS_AttackGuide		UMETA(DeplayName = "AttackGuide"),
	TAS_DodgeGuide		UMETA(DeplayName = "DodgeGuide"),
	TAS_DefenseGuide	UMETA(DeplayName = "DefenseGuide"),
	TAS_RecoveryGuide	UMETA(DeplayName = "RecoveryGuide"),

	TAS_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ETargetSearchType :uint8
{
	TST_None			UMETA(DeplayName = "None"),
	TST_WheelUp			UMETA(DeplayName = "WheelUp"),
	TST_WheelDown		UMETA(DeplayName = "WheelDown"),

	TST_MAX				UMETA(DeplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EDamageType :uint8
{
	DT_LightAttack		UMETA(DeplayName = "LightAttack"),
	DT_HeavyAttack		UMETA(DeplayName = "HeavyAttack"),
	DT_SpAttack			UMETA(DeplayName = "SpAttack"),

	DT_MAX				UMETA(DeplayName = "DefaultMAX"),
};

/**
 * 
 */
class SOULSLIKEACTION_API EnumState
{
public:
	EnumState();
	~EnumState();
};
