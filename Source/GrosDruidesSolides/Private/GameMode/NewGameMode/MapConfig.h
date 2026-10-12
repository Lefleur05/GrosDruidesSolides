// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapConfig.generated.h"

UCLASS()
class AMapConfig : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawns") TArray<TObjectPtr<AActor>> spawPoint;

public:

	TArray<TObjectPtr<AActor>> GetSpawnTransform() { return spawPoint; }

public:	
	AMapConfig();

};
