// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraParamDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FCameraParam
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) FRotator initialRotation = FRotator(-70.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere) float minZoom = 600.0f;
	UPROPERTY(EditAnywhere) FVector2D rangeDistance = FVector2D(600.0f, 4600.0f);
	UPROPERTY(EditAnywhere) FVector2D rangeZoom = FVector2D(3.75f, 6.8f);

	UPROPERTY(EditAnywhere)float moveSpeed = 500.0f;
	UPROPERTY(EditAnywhere)float moveSpeedOnePlayer = 1000.0f;
	UPROPERTY(EditAnywhere)float zoomSpeed = 5000.0f;

};

UCLASS()
class UCameraParamDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere) TMap<FString, FCameraParam> cameraParamPerMap;
	
};
