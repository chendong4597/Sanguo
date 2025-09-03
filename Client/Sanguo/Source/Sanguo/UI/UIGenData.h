// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIGenData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SANGUO_API UUIGenData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 m_Id{ 0 };
};
