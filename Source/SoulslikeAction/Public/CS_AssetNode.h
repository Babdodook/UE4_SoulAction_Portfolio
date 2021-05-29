// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SOULSLIKEACTION_API CS_AssetNode
{
public:
	CS_AssetNode();
	~CS_AssetNode();

private:
	ConstructorHelpers::FObjectFinder<UObject>* m_asset;

public:
	static CS_AssetNode* create(const TCHAR* Path);

	ConstructorHelpers::FObjectFinder<UObject>* getAsset() { return m_asset; }
};
