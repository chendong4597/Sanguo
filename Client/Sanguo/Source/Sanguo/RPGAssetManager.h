// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "RPGAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class SANGUO_API URPGAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	URPGAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Static types for items */
	static const FPrimaryAssetType	MapItemType;

	/** Returns the current AssetManager object */
	static URPGAssetManager& Get();

	void EntryConfigFiles();

	//URPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};
