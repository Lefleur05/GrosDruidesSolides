// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/NewGameMode/PartyManager/PartyManager.h"
#include "ClassicBattlePartyManager.generated.h"

UCLASS()
class AClassicBattlePartyManager : public APartyManager
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) TArray<FVector> spawnPoint;

	UPROPERTY(EditAnywhere) int pointForWin = 3;

	UPROPERTY(EditAnywhere) float timeStartCurrentLevel = 0.0f;

	UPROPERTY(EditAnywhere) float timeForStartBattle = 5.0f;
	UPROPERTY(EditAnywhere) FTimerHandle timerForStartBattle;
	UPROPERTY(EditAnywhere) FTimerHandle timerEndBattle;
	UPROPERTY(EditAnywhere) FTimerHandle timerBackToLobby;


	UPROPERTY(EditAnywhere) FTimerHandle startBattleTimer;
	UPROPERTY(EditAnywhere) float timeStartBattle = 10.0f;

	UPROPERTY(EditAnywhere) bool roundEnded = false;


public:	
	AClassicBattlePartyManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void InitParty(const TArray<TObjectPtr<APlayerCharacter>>& _players) override;
	virtual void StartParty()override;
	virtual void EndParty()override;

	void FreeForBattle();

	int GetCountPlayerAlive(TObjectPtr<APlayerCharacter>& _lastPlayerStanding);
	bool AddPlayerPoint(TObjectPtr<APlayerCharacter> _lastPlayerStanding);

	UFUNCTION() void DeathPlayer(APlayerCharacter* _player);

	void BackToLobby();
	void SwitchToCarrousel();
};
