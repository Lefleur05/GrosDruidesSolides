// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MultyGameModeBase.h"
#include "LobbyGameModeBase.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class ALobbyGameModeBase : public AMultyGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) FTimerHandle timerRespawn1;
	UPROPERTY(EditAnywhere) FTimerHandle timerRespawn2;
	UPROPERTY(EditAnywhere) FTimerHandle timerRespawn3;
	UPROPERTY(EditAnywhere) FTimerHandle timerRespawn4;
	UPROPERTY(EditAnywhere) float respawnTime = 5.0f;
	UPROPERTY(EditAnywhere) bool pointIsReset = false;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void NotifyPawnPossessed(TObjectPtr<AController> _playerController, TObjectPtr<APlayerCharacter> _player);
	UFUNCTION() void DoRespawnPlayer(APlayerCharacter* _player);

public:
	UFUNCTION() void RespawnPlayer(APlayerCharacter* _player);
	void ClearItem();

};
