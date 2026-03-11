// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CrustyPirateGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PG_CP_GDTV_API UCrustyPirateGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 

	UPROPERTY(VisibleAnywhere, BLueprintReadWrite)
	int PlayerHP = 100;

	void SetPlayerHP(int NewHP);
	
};
