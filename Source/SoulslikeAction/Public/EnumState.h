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

/**
 * 
 */
class SOULSLIKEACTION_API EnumState
{
public:
	EnumState();
	~EnumState();
};
