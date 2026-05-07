// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MultyGameModeBase.h"
#include "CarrouselGameModeBase.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class ACarrouselGameModeBase : public AMultyGameModeBase
{
	GENERATED_BODY()
	//TODO past in game instance of player state
	//UPROPERTY(EditAnywhere) TMap<TObjectPtr<APlayerCharacter>, int> pointPerPlayer;
	UPROPERTY(EditAnywhere) TMap<FString, int> pointPerPlayer;

	UPROPERTY(EditAnywhere) TArray<FVector> carrouselSpawnPoint;
	UPROPERTY(EditAnywhere) int countObjectTake = 0;

	UPROPERTY(EditAnywhere) float timeForLastPlayer = 2.0f;
	UPROPERTY(EditAnywhere) float timeForMidPlayer = 4.0f;
	UPROPERTY(EditAnywhere) float timeForFirstPlayer = 6.0f;
	UPROPERTY(EditAnywhere) FTimerHandle timerFreeLastPlayer;
	UPROPERTY(EditAnywhere) FTimerHandle timerFreeMidPlayer;
	UPROPERTY(EditAnywhere) FTimerHandle timerFreeFirstPlayer;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> firstPlayers;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> midPlayers;
	UPROPERTY(EditAnywhere) TArray<TObjectPtr<APlayerCharacter>> lastPlayers;

public:
	ACarrouselGameModeBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player) override;

	void Init();

	void StartCarrousel();

	void TriPlayerPerPoint();
	void FreeLastPlayer();
	void FreeMidPlayer();
	void FreeFirstPlayer();
	UFUNCTION() void ObjectTake(APlayerCharacter* _player);

	void SwitchToBattle();
};
