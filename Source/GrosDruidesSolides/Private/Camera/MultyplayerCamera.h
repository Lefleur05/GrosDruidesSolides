// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Player/PlayerCharacter.h>
#include "MultyplayerCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;
//class APlayerCharacter;

UCLASS()
class AMultyplayerCamera : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) TObjectPtr<USpringArmComponent> springArm = nullptr;
	UPROPERTY(EditAnywhere) TObjectPtr<UCameraComponent> camera = nullptr;

	UPROPERTY(EditAnywhere) bool isViewTarget = false; //TODO temp need better solution
	UPROPERTY(EditAnywhere) bool setHasViewTarget = false;

	UPROPERTY(EditAnywhere) float minZoom = 600.0f;
	UPROPERTY(EditAnywhere) FVector2D rangeDistance = FVector2D(600.0f,4600.0f);
	UPROPERTY(EditAnywhere) FVector2D rangeZoom = FVector2D(3.75f,6.8f);

	UPROPERTY(EditAnywhere)float moveSpeed = 500.0f;
	UPROPERTY(EditAnywhere)float moveSpeedOnePlayer = 1000.0f;
	UPROPERTY(EditAnywhere)float zoomSpeed = 5000.0f;

protected:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite) TArray<TObjectPtr<APlayerCharacter>> playerList = TArray<TObjectPtr<APlayerCharacter>>();

public:
	FORCEINLINE void SetPlayerList(const TArray<TObjectPtr<APlayerCharacter>>& _playerList) { playerList = _playerList; }

public:	
	AMultyplayerCamera();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CameraLocationWithAverage();
	void CameraZoom();

};
