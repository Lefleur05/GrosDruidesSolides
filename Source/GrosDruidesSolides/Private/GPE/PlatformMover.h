// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformMover.generated.h"


USTRUCT(BlueprintType)
struct FPlateformToMoveInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) TObjectPtr<AActor> plateform = nullptr;
	UPROPERTY(EditAnywhere) float startMovingTime = 0.0f;
	UPROPERTY(EditAnywhere) float endMovingTime = 0.0f;
	UPROPERTY(EditAnywhere) FVector startLocation = FVector(0.0f);
	UPROPERTY(EditAnywhere) FVector endLocation = FVector(0.0f);
};

UCLASS()
class APlatformMover : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) TArray<FPlateformToMoveInfo> allPlateformToMove;
	UPROPERTY(EditAnywhere, Replicated) float startTime = 0.0f;
	UPROPERTY(EditAnywhere, Replicated) float currentTime = 0.0f;

public:	
	APlatformMover();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MovePlateform();

};
