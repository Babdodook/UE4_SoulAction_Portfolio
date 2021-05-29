// Copyright Epic Games, Inc. All Rights Reserved.


#include "SoulslikeActionGameModeBase.h"
#include "CS_TargetPointHUD.h"

ASoulslikeActionGameModeBase::ASoulslikeActionGameModeBase()
{
	// HUD ¼³Á¤
	static ConstructorHelpers::FClassFinder<ACS_TargetPointHUD> BP_TargetPointHUD(TEXT("BlueprintHUD'/Game/HUD/BP_TargetPointHUD.BP_TargetPointHUD_C'"));
	if (BP_TargetPointHUD.Class != NULL)
	{
		HUDClass = BP_TargetPointHUD.Class;
	}
}