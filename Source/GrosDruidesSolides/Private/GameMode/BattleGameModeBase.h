// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MultyGameModeBase.h"
#include "BattleGameModeBase.generated.h"

class ACarrousel;

UCLASS()
class ABattleGameModeBase : public AMultyGameModeBase
{
	GENERATED_BODY()
	//TODO past in game instance of player state
	UPROPERTY(EditAnywhere) TArray<FVector> spawnPoint;
	
	UPROPERTY(EditAnywhere) int pointForWin = 3;

	UPROPERTY(EditAnywhere) float timeStartCurrentLevel = 0.0f;

	UPROPERTY(EditAnywhere) float timeForStartBattle = 5.0f;
	UPROPERTY(EditAnywhere) FTimerHandle timerForStartBattle;
	UPROPERTY(EditAnywhere) FTimerHandle timerEndBattle;


	UPROPERTY(EditAnywhere) FTimerHandle startBattleTimer;
	UPROPERTY(EditAnywhere) float timeStartBattle = 10.0f;

	UPROPERTY(EditAnywhere) bool roundEnded = false;
	
public:	
	ABattleGameModeBase();

protected:
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player) override;

	void Init();

	//Battle
	void StartBattle();
	void EndBattle();
	void FreeForBattle();

	UFUNCTION() void DeathPlayer(APlayerCharacter* _player);

	void BackToLobby();
	void SwitchToCarrousel();

};
