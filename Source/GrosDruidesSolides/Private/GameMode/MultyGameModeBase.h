// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultyGameModeBase.generated.h"

class AMultyplayerCamera;

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class AMultyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> allPlayers = TArray<TObjectPtr<APlayerCharacter>>();
	UPROPERTY(EditAnywhere) TArray<FVector> allSpawnPoint;
	UPROPERTY(EditAnywhere) TObjectPtr<AMultyplayerCamera> currentCamera;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	UFUNCTION() void RequestRespawn(AController* _controller);
	virtual void NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player);
	virtual void PostSeamlessTravel() override;


};
